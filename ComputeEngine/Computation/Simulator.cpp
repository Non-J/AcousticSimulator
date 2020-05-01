#include "Simulator.h"
#include <fmt/format.h>
#include <chrono>
#include <complex>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <numbers>
#include <string_view>
#include "../JSONConvert.h"
#include "BlockStorage.h"

namespace Computation {

std::complex<double> compute_pressure(const Vec3<double>& point,
                                      const Transducer& transducer,
                                      const SimulationParameter& simulation_parameter) {
  const auto i = std::complex<double>(0, 1);
  const auto angle = transducer.position.cosine_angle(transducer.target, point);
  const auto dist = transducer.position.euclidean_distance(point);
  const auto wave_number = 2.0 * std::numbers::pi * simulation_parameter.frequency /
                           simulation_parameter.wave_speed;

  const auto directivity = [&]() -> double {
    const auto intermediate = wave_number * transducer.radius * std::sin(angle);
    if (intermediate == 0.0) {
      return 1.0;
    }
    return 2.0 * std::cyl_bessel_j(1, intermediate) / intermediate;
  }();

  return std::exp(i * (wave_number * dist + transducer.phase_shift)) *
         (transducer.output_power * transducer.loss_factor * directivity / dist);
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
  log("Simulation process started");

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

  // TODO: Update openmp loops (see comment)
  /* OpenMP 2.0 (latest supported by MSVC) doesn't allow for unsigned loop counter
   * for some reason. Making this project works with clang-cl should resolve this
   * and allow loops to be indexed by std::size_t and some performance gain. */

  log("Computing pressure");

  auto pressure_block = CellBlock<std::complex<double>>(padded_size);

#pragma omp parallel for
  for (int64_t id = 0; id < int64_t(padded_interpolation.get_cell_count()); ++id) {
    auto pressure_result = std::complex<double>();
    for (const auto& transducer : transducers) {
      pressure_result += compute_pressure(padded_interpolation.get_real_vec(id),
                                          transducer, simulation_parameter);
    }
    pressure_block.set_cell(id, pressure_result);
  }

  log("Exporting pressure");

  auto pressure_export =
      std::ofstream(export_directory / std::string_view("pressure_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  pressure_export.write(pressure_block.get_raw_bytes(),
                        pressure_block.size() * sizeof(std::complex<double>));
  pressure_export.close();

  log("Exporting metadata");

  auto metadata = nlohmann::json();

  metadata["version"] = 1;
  metadata["pressure_result_dimension"] = JSONConvert::from_vec3(padded_size);
  metadata["pressure_result_begin"] = JSONConvert::from_vec3(padded_begin);
  metadata["pressure_result_end"] = JSONConvert::from_vec3(padded_end);

  auto metadata_export =
      std::ofstream(export_directory / std::string_view("metadata.json"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  metadata_export << metadata.dump();
  metadata_export.close();

  // Unlock process
  log("Simulation process done");
  process_lock_simulation_running->store(false);
}

}  // namespace Computation