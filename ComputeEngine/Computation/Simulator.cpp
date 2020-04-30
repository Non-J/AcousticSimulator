#include "Simulator.h"
#include <fmt/format.h>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string_view>

using namespace std::literals::string_view_literals;

namespace Computation {

// region Math section

template <typename T>
T CellBlock<T>::get_cell_by_id(std::size_t id) const {
  return data[id];
}

template <typename T>
void CellBlock<T>::set_cell_by_id(std::size_t id, T value) {
  data[id] = value;
}

template <typename T>
Vec3<T> RealSpaceInterpolation<T>::get_real_vec(std::size_t id) const {
  const auto pos =
      get_int_vec(id).template cast<T>().elem_division(dimension_size.cast<T>());
  return Vec3<T>{std::lerp(begin.x, end.x, pos.x), std::lerp(begin.y, end.y, pos.y),
                 std::lerp(begin.z, end.z, pos.z)};
}

template <typename T>
Vec3<std::size_t> RealSpaceInterpolation<T>::get_int_vec(std::size_t id) const {
  const auto x_idx = (id / dimension_size.z / dimension_size.y) % dimension_size.x;
  const auto y_idx = (id / dimension_size.z) % dimension_size.y;
  const auto z_idx = id % dimension_size.z;
  return Vec3<std::size_t>{x_idx, y_idx, z_idx};
}

// endregion

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
                       std::string export_name,
                       std::vector<Computation::Transducer> transducers,
                       Computation::SimulationParameter simulation_parameter) {
  // The size is computed based on cell_size
  // To allow for derivation, extra cells must be padded
  const auto dimension_size =
      ((simulation_parameter.end - simulation_parameter.begin).elem_abs() /
       simulation_parameter.cell_size)
          .cast<std::size_t>() +
      3;

  // Shift by one cell (padding)
  const auto space_begin = simulation_parameter.begin - simulation_parameter.cell_size;
  const auto space_end =
      space_begin + (dimension_size.cast<double>() * simulation_parameter.cell_size);

  const auto space_interpolation =
      RealSpaceInterpolation<double>(dimension_size, space_begin, space_end);
  auto test_cell_block = CellBlock<double>(dimension_size);

  /* OpenMP 2.0 (latest supported by MSVC) doesn't allow for unsigned loop counter
   * for some reason. Making this project works with clang-cl should resolve this. */
  // TODO: Upgrade OpenMP
  // TODO: Make this loop index by std::size_t (see comment)
#pragma omp parallel for
  for (int64_t id = 0; id < int64_t(dimension_size.product()); ++id) {
    test_cell_block.set_cell_by_id(id, space_interpolation.get_real_vec(id).x);
  }

  auto output_dir = std::filesystem::current_path() / export_name;
  std::filesystem::create_directory(output_dir);

  // TODO: Remove test dump
  auto test_dump =
      std::ofstream(output_dir / "test_cell_block.blob"sv,
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);

  test_dump.write(test_cell_block.get_raw_bytes(),
                  test_cell_block.size() * sizeof(double));

  test_dump.flush();
  test_dump.close();

  // Unlock process
  process_lock_simulation_running->store(false);
}

// endregion

}  // namespace Computation