#include "Simulator.h"
#include <fmt/format.h>
#include <chrono>
#include <complex>
#include <filesystem>
#include <fstream>
#include <numbers>
#include <string_view>
#include "../Utilities/AtomicLogger.h"
#include "BlockStorage.h"

namespace Computation {

// constant i
constexpr auto i = std::complex<double>(0, 1);

constexpr double SimulationParameter::particle_volume() const {
  return (4.0 / 3.0) * std::numbers::pi * this->particle_radius *
         this->particle_radius * this->particle_radius;
}
constexpr double SimulationParameter::angular_frequency() const {
  return this->frequency / (2.0 * std::numbers::pi);
}
constexpr double SimulationParameter::constant_k1() const {
  const auto particle_volume = this->particle_volume();

  const auto i1 = this->air_wave_speed * this->air_wave_speed * this->air_density;
  const auto i2 =
      this->particle_wave_speed * this->particle_wave_speed * this->particle_density;

  if (this->assume_large_particle_density) {
    return particle_volume / i1 / 4.0;
  }
  return particle_volume * (1.0 / i1 - 1.0 / i2) / 4.0;
}
constexpr double SimulationParameter::constant_k2() const {
  const auto i1 = this->particle_volume() * 3.0 / 4.0;
  const auto w = this->angular_frequency();
  const auto i2 = w * w * this->air_density;
  const auto i3 = this->air_density + 2.0 * this->particle_density;
  const auto i4 = this->air_density - this->particle_density;

  if (this->assume_large_particle_density) {
    return i1 / i2 / -2.0;
  }
  return i1 * (i4 / i3 / i2);
}

std::complex<double> compute_pressure(const Vec3<double>& point,
                                      const Transducer& transducer,
                                      const SimulationParameter& simulation_parameter) {
  const auto angle = transducer.position.cosine_angle(transducer.target, point);
  const auto dist = transducer.position.euclidean_distance(point);
  const auto wave_number = 2.0 * std::numbers::pi * simulation_parameter.frequency /
                           simulation_parameter.air_wave_speed;

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

template <typename T, typename D>
constexpr T numerical_differentiation(const T& lhs, const T& rhs, const D& dist) {
  return (rhs - lhs) / (dist * 2);
}

constexpr double euclidean_norm_squared(const std::complex<double>& complex) {
  return complex.real() * complex.real() + complex.imag() * complex.imag();
}

void simulationProcess(std::atomic<bool>* process_lock_simulation_running,
                       AtomicLogger::AtomicLogger* result_log,
                       std::filesystem::path export_directory,
                       std::vector<Computation::Transducer> transducers,
                       Computation::SimulationParameter simulation_parameter) {
  result_log->log("Simulation process started");

  // force result is the smallest which will be used as the baseline
  const auto force_cnt =
      ((simulation_parameter.end - simulation_parameter.begin).elem_abs() /
       simulation_parameter.cell_size)
          .cast<std::size_t>() +
      1;
  const auto force_beg = simulation_parameter.begin;
  const auto force_end = simulation_parameter.begin +
                         (force_cnt.cast<double>() * simulation_parameter.cell_size);
  const auto force_blk = CellBlockInterpolation(force_cnt, force_beg, force_end);
  const auto force_lpn = int64_t(force_blk.get_cell_count());

  // for pressure and potential result, padding is added for differentiation
  const auto potential_cnt = force_cnt + 2;
  const auto potential_beg = force_beg - simulation_parameter.cell_size;
  const auto potential_end = force_end + simulation_parameter.cell_size;
  const auto potential_blk =
      CellBlockInterpolation(potential_cnt, potential_beg, potential_end);
  const auto potential_lpn = int64_t(potential_blk.get_cell_count());

  const auto pressure_cnt = potential_cnt + 2;
  const auto pressure_beg = potential_beg - simulation_parameter.cell_size;
  const auto pressure_end = potential_end + simulation_parameter.cell_size;
  const auto pressure_blk =
      CellBlockInterpolation(pressure_cnt, pressure_beg, pressure_end);
  const auto pressure_lpn = int64_t(pressure_blk.get_cell_count());

  // TODO: Update openmp loops (see comment)
  /* OpenMP 2.0 (latest supported by MSVC) doesn't allow for unsigned loop counter
   * for some reason. Making this project works with clang-cl should resolve this
   * and allow loops to be indexed by std::size_t and some performance gain. */

  result_log->log("Computing pressure");

  auto pressure_val = CellBlock<std::complex<double>>(pressure_cnt);

#pragma omp parallel for
  for (int64_t id = 0; id < pressure_lpn; ++id) {
    auto pressure_result = std::complex<double>();
    for (const auto& transducer : transducers) {
      pressure_result += compute_pressure(pressure_blk.get_real_vec(id), transducer,
                                          simulation_parameter);
    }
    pressure_val.set_cell(id, pressure_result);
  }

  result_log->log("Computing potential");

  // constant used for potential computation
  const auto k1 = simulation_parameter.constant_k1();
  const auto k2 = simulation_parameter.constant_k2();

  auto potential_val = CellBlock<double>(potential_cnt);

#pragma omp parallel for
  for (int64_t id = 0; id < potential_lpn; ++id) {
    const auto idx_lhs = potential_blk.get_int_vec(id);
    const auto idx_mid = idx_lhs + 1;
    const auto idx_rhs = idx_lhs + 2;

    // TODO: Refactor this
    // Call below is unreadable and should be wrapped in a function or some kind of
    // abstraction
    const auto p_x = euclidean_norm_squared(numerical_differentiation(
        pressure_val.get_cell(pressure_blk.get_id({idx_lhs.x, idx_mid.y, idx_mid.z})),
        pressure_val.get_cell(pressure_blk.get_id({idx_rhs.x, idx_mid.y, idx_mid.z})),
        simulation_parameter.cell_size));
    const auto p_y = euclidean_norm_squared(numerical_differentiation(
        pressure_val.get_cell(pressure_blk.get_id({idx_mid.x, idx_lhs.y, idx_mid.z})),
        pressure_val.get_cell(pressure_blk.get_id({idx_mid.x, idx_rhs.y, idx_mid.z})),
        simulation_parameter.cell_size));
    const auto p_z = euclidean_norm_squared(numerical_differentiation(
        pressure_val.get_cell(pressure_blk.get_id({idx_mid.x, idx_mid.y, idx_lhs.z})),
        pressure_val.get_cell(pressure_blk.get_id({idx_mid.x, idx_mid.y, idx_rhs.z})),
        simulation_parameter.cell_size));

    const auto p = euclidean_norm_squared(
        pressure_val.get_cell(pressure_blk.get_id({idx_mid.x, idx_mid.y, idx_mid.z})));

    const auto u = 2.0 * k1 * p - 2.0 * k2 * (p_x + p_y + p_z);
    potential_val.set_cell(id, u);
  }

  result_log->log("Computing force");

  auto force_x_val = CellBlock<double>(force_cnt);
  auto force_y_val = CellBlock<double>(force_cnt);
  auto force_z_val = CellBlock<double>(force_cnt);

#pragma omp parallel for
  for (int64_t id = 0; id < force_lpn; ++id) {
    const auto idx_lhs = force_blk.get_int_vec(id);
    const auto idx_mid = idx_lhs + 1;
    const auto idx_rhs = idx_lhs + 2;

    // TODO: Refactor this (too)
    const auto f_x = -numerical_differentiation(
        potential_val.get_cell(potential_blk.get_id({idx_lhs.x, idx_mid.y, idx_mid.z})),
        potential_val.get_cell(potential_blk.get_id({idx_rhs.x, idx_mid.y, idx_mid.z})),
        simulation_parameter.cell_size);
    const auto f_y = -numerical_differentiation(
        potential_val.get_cell(potential_blk.get_id({idx_mid.x, idx_lhs.y, idx_mid.z})),
        potential_val.get_cell(potential_blk.get_id({idx_mid.x, idx_rhs.y, idx_mid.z})),
        simulation_parameter.cell_size);
    const auto f_z = -numerical_differentiation(
        potential_val.get_cell(potential_blk.get_id({idx_mid.x, idx_mid.y, idx_lhs.z})),
        potential_val.get_cell(potential_blk.get_id({idx_mid.x, idx_mid.y, idx_rhs.z})),
        simulation_parameter.cell_size);

    force_x_val.set_cell(id, f_x);
    force_y_val.set_cell(id, f_y);
    force_z_val.set_cell(id, f_z);
  }

  result_log->log("Exporting data");

  auto pressure_exp =
      std::ofstream(export_directory / std::string_view("pressure_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  pressure_exp.write(pressure_val.get_raw_bytes(),
                     pressure_val.size() * sizeof(std::complex<double>));
  pressure_exp.close();

  auto potential_exp =
      std::ofstream(export_directory / std::string_view("potential_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  potential_exp.write(potential_val.get_raw_bytes(),
                      potential_val.size() * sizeof(double));
  potential_exp.close();

  auto force_x_exp =
      std::ofstream(export_directory / std::string_view("force_x_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  force_x_exp.write(force_x_val.get_raw_bytes(), force_x_val.size() * sizeof(double));
  force_x_exp.close();

  auto force_y_exp =
      std::ofstream(export_directory / std::string_view("force_y_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  force_y_exp.write(force_y_val.get_raw_bytes(), force_y_val.size() * sizeof(double));
  force_y_exp.close();

  auto force_z_exp =
      std::ofstream(export_directory / std::string_view("force_z_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  force_z_exp.write(force_z_val.get_raw_bytes(), force_z_val.size() * sizeof(double));
  force_z_exp.close();

  result_log->log("Exporting metadata");

  auto metadata = nlohmann::json();

  metadata["version"] = 1;
  metadata["pressure_cnt"] = pressure_cnt.to_json();
  metadata["pressure_beg"] = pressure_beg.to_json();
  metadata["pressure_end"] = pressure_end.to_json();
  metadata["potential_cnt"] = potential_cnt.to_json();
  metadata["potential_beg"] = potential_beg.to_json();
  metadata["potential_end"] = potential_end.to_json();
  metadata["force_cnt"] = force_cnt.to_json();
  metadata["force_beg"] = force_beg.to_json();
  metadata["force_end"] = force_end.to_json();

  auto metadata_export =
      std::ofstream(export_directory / std::string_view("metadata.json"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  metadata_export << metadata.dump();
  metadata_export.close();

  // Unlock process
  result_log->log("Simulation process done");
  process_lock_simulation_running->store(false);
}

}  // namespace Computation