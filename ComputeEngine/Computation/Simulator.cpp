#include "Simulator.h"
#include <fmt/format.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string_view>
#include "BlockStorage.h"

namespace Computation {

// region Computer section

std::string Transducer::checkInvalidParameter() const {
  if (this->radius <= 0) {
    return "Radius is not positive";
  }
  if (this->loss_factor < 0 || this->loss_factor > 1) {
    return "Loss factor is not in range 0 and 1";
  }
  if (this->output_power < 0 || this->output_power > 1) {
    return "Output power is not in range 0 and 1";
  }

  return std::string();
}

std::string SimulationParameter::checkInvalidParameter() const {
  if (this->cell_size <= 0) {
    return "Cell size is not positive";
  }
  if (this->frequency <= 0) {
    return "Frequency is not positive";
  }
  if (this->particle_radius <= 0) {
    return "Particle radius is not positive";
  }
  if (this->air_density <= 0) {
    return "Air density is not positive";
  }
  if (this->wave_speed <= 0) {
    return "Wave speed is not positive";
  }

  return std::string();
}

void simulationProcess(std::atomic<bool>* process_lock_simulation_running,
                       std::mutex* simulation_logging_lock,
                       std::string* simulation_logging,
                       std::filesystem::path export_directory,
                       std::vector<Computation::Transducer> transducers,
                       Computation::SimulationParameter simulation_parameter) {
  // Logging
  const auto time_begin = std::chrono::steady_clock::now();
  auto time_begin_lap = time_begin;
  const auto log = [&](std::string_view message) {
    simulation_logging_lock->lock();
    simulation_logging->append(
        fmt::format(FMT_STRING("[{:d}/{:d} ms] {:s}\n"),
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - time_begin_lap)
                        .count(),
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - time_begin)
                        .count(),
                    message));
    simulation_logging_lock->unlock();
    time_begin_lap = std::chrono::steady_clock::now();
  };
  log("Simulation started");

  // The size is computed based on cell_size
  // To allow for derivation, extra cells must be padded
  const auto padded_size =
      ((simulation_parameter.end - simulation_parameter.begin).elem_abs() /
       simulation_parameter.cell_size)
          .cast<std::size_t>() +
      3;

  // Shift by one cell (padding)
  const auto padded_begin = simulation_parameter.begin - simulation_parameter.cell_size;
  const auto padded_end =
      padded_begin + (padded_size.cast<double>() * simulation_parameter.cell_size);
  const auto padded_interpolation =
      CellBlockInterpolation<double>(padded_size, padded_begin, padded_end);

  auto test_cell_block = CellBlock<double>(padded_size);

  // TODO: Make this loop index by std::size_t (see comment)
  /* OpenMP 2.0 (latest supported by MSVC) doesn't allow for unsigned loop counter
   * for some reason. Making this project works with clang-cl should resolve this. */
#pragma omp parallel for
  for (int64_t id = 0; id < int64_t(padded_size.product()); ++id) {
    test_cell_block.set_cell(id, padded_interpolation.get_real_vec(id).x);
  }

  // TODO: Remove test dump
  auto test_dump =
      std::ofstream(export_directory / "test_cell_block.blob",
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);

  test_dump.write(test_cell_block.get_raw_bytes(),
                  test_cell_block.size() * sizeof(double));

  test_dump.flush();
  test_dump.close();

  // Unlock process
  log("Simulation done");
  process_lock_simulation_running->store(false);
}

// endregion

}  // namespace Computation