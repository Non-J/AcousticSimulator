#include "Simulator.h"
#include <fmt/format.h>
#include <chrono>
#include <complex>
#include <fstream>
#include <nlohmann/json.hpp>
#include "BlockStorage.h"
#include "Vec3.h"

using namespace Simulator;

template <typename T, typename D>
constexpr T numerical_differentiation(const T& lhs, const T& rhs, const D& dist) {
  return (rhs - lhs) / (dist * 2);
}

constexpr double euclidean_norm_squared(const std::complex<double>& complex) {
  return complex.real() * complex.real() + complex.imag() * complex.imag();
}

PressureResult::PressureResult(const Vec3<double>& Begin,
                               const Vec3<double>& End,
                               const double cell_size)
    : begin(Begin),
      end(End),
      count(((Begin - End).elem_abs() / cell_size).cast<std::size_t>()),
      interpolate(count, begin, end),
      value(count) {}

PressureResult PressureResult::fromTransducersDirectRayCast(
    const std::vector<Transducer::ITransducer>& transducers,
    const StaticSimulationParameter& static_simulation_parameter) {
  const auto dynamic_parameter =
      Transducer::DynamicParameter{static_simulation_parameter.getMediumWaveSpeed()};

  auto result = PressureResult(static_simulation_parameter.getBegin(),
                               static_simulation_parameter.getEnd(),
                               static_simulation_parameter.getCellSize());

  const int64_t iterations = result.interpolate.get_cell_count();
#pragma omp parallel for
  for (int64_t id = 0; id < iterations; ++id) {
    const auto cell_id = BlockStorage::CellId{static_cast<size_t>(id)};
    auto pressure_temporary = std::complex<double>();
    for (const auto& transducer : transducers) {
      pressure_temporary += transducer.get_pressure(
          result.interpolate.get_real_vec(cell_id), dynamic_parameter);
    }
    result.value.set_cell(cell_id, pressure_temporary);
  }

  return result;
}

const Vec3<double>& PressureResult::getBegin() const {
  return begin;
}
const Vec3<double>& PressureResult::getEnd() const {
  return end;
}
const Vec3<std::size_t>& PressureResult::getCount() const {
  return count;
}
const BlockStorage::Interpolation& PressureResult::getInterpolate() const {
  return interpolate;
}
const BlockStorage::Memory<std::complex<double>>& PressureResult::getValue() const {
  return value;
}

PotentialResult::PotentialResult(const Vec3<double>& Begin,
                                 const Vec3<double>& End,
                                 const double cell_size)
    : begin(Begin),
      end(End),
      count(((Begin - End).elem_abs() / cell_size).cast<std::size_t>()),
      interpolate(count, begin, end),
      value(count) {}

PotentialResult PotentialResult::fromPressureResult(
    const PressureResult& pressure_result,
    const StaticSimulationParameter& static_simulation_parameter) {
  const auto cell_size = static_simulation_parameter.getCellSize();
  auto result = PotentialResult(pressure_result.getBegin() + cell_size,
                                pressure_result.getEnd() - cell_size, cell_size);

  int64_t iterations = result.interpolate.get_cell_count();
#pragma omp parallel for
  for (int64_t id = 0; id < iterations; ++id) {
    const auto cell_id = BlockStorage::CellId{static_cast<size_t>(id)};
    const auto id_lhs = result.interpolate.get_int_vec(cell_id);
    const auto id_mid = id_lhs + 1;
    const auto id_rhs = id_lhs + 2;

    const auto p = euclidean_norm_squared(pressure_result.getValue().get_cell(
        pressure_result.getInterpolate().get_id({id_mid.x, id_mid.y, id_mid.z})));

    const auto p_x = euclidean_norm_squared(numerical_differentiation(
        pressure_result.getValue().get_cell(
            pressure_result.getInterpolate().get_id({id_lhs.x, id_mid.y, id_mid.z})),
        pressure_result.getValue().get_cell(
            pressure_result.getInterpolate().get_id({id_rhs.x, id_mid.y, id_mid.z})),
        cell_size));
    const auto p_y = euclidean_norm_squared(numerical_differentiation(
        pressure_result.getValue().get_cell(
            pressure_result.getInterpolate().get_id({id_mid.x, id_lhs.y, id_mid.z})),
        pressure_result.getValue().get_cell(
            pressure_result.getInterpolate().get_id({id_mid.x, id_rhs.y, id_mid.z})),
        cell_size));
    const auto p_z = euclidean_norm_squared(numerical_differentiation(
        pressure_result.getValue().get_cell(
            pressure_result.getInterpolate().get_id({id_mid.x, id_mid.y, id_lhs.z})),
        pressure_result.getValue().get_cell(
            pressure_result.getInterpolate().get_id({id_mid.x, id_mid.y, id_rhs.z})),
        cell_size));

    const auto u =
        2.0 * static_simulation_parameter.getAcousticPotentialK1() * p -
        2.0 * static_simulation_parameter.getAcousticPotentialK2() * (p_x + p_y + p_z);
    result.value.set_cell(cell_id, u);
  }

  return result;
}

const Vec3<double>& PotentialResult::getBegin() const {
  return begin;
}
const Vec3<double>& PotentialResult::getEnd() const {
  return end;
}
const Vec3<std::size_t>& PotentialResult::getCount() const {
  return count;
}
const BlockStorage::Interpolation& PotentialResult::getInterpolate() const {
  return interpolate;
}
const BlockStorage::Memory<double>& PotentialResult::getValue() const {
  return value;
}

ForceResult::ForceResult(const Vec3<double>& Begin,
                         const Vec3<double>& End,
                         const double cell_size)
    : begin(Begin),
      end(End),
      count(((Begin - End).elem_abs() / cell_size).cast<std::size_t>()),
      interpolate(count, begin, end),
      x_value(count),
      y_value(count),
      z_value(count) {}

ForceResult ForceResult::fromPotentialResult(
    const PotentialResult& potential_result,
    const StaticSimulationParameter& static_simulation_parameter) {
  const auto cell_size = static_simulation_parameter.getCellSize();
  auto result = ForceResult(potential_result.getBegin() + cell_size,
                            potential_result.getEnd() - cell_size, cell_size);

  int64_t iterations = result.interpolate.get_cell_count();
#pragma omp parallel for
  for (int64_t id = 0; id < iterations; ++id) {
    const auto cell_id = BlockStorage::CellId{static_cast<size_t>(id)};
    const auto id_lhs = result.interpolate.get_int_vec(cell_id);
    const auto id_mid = id_lhs + 1;
    const auto id_rhs = id_lhs + 2;

    const auto f_x = -numerical_differentiation(
        potential_result.getValue().get_cell(
            potential_result.getInterpolate().get_id({id_lhs.x, id_mid.y, id_mid.z})),
        potential_result.getValue().get_cell(
            potential_result.getInterpolate().get_id({id_rhs.x, id_mid.y, id_mid.z})),
        cell_size);
    const auto f_y = -numerical_differentiation(
        potential_result.getValue().get_cell(
            potential_result.getInterpolate().get_id({id_mid.x, id_lhs.y, id_mid.z})),
        potential_result.getValue().get_cell(
            potential_result.getInterpolate().get_id({id_mid.x, id_rhs.y, id_mid.z})),
        cell_size);
    const auto f_z = -numerical_differentiation(
        potential_result.getValue().get_cell(
            potential_result.getInterpolate().get_id({id_mid.x, id_mid.y, id_lhs.z})),
        potential_result.getValue().get_cell(
            potential_result.getInterpolate().get_id({id_mid.x, id_mid.y, id_rhs.z})),
        cell_size);

    result.x_value.set_cell(cell_id, f_x);
    result.y_value.set_cell(cell_id, f_y);
    result.z_value.set_cell(cell_id, f_z);
  }

  return result;
}

const Vec3<double>& ForceResult::getBegin() const {
  return begin;
}
const Vec3<double>& ForceResult::getEnd() const {
  return end;
}
const Vec3<std::size_t>& ForceResult::getCount() const {
  return count;
}
const BlockStorage::Interpolation& ForceResult::getInterpolate() const {
  return interpolate;
}
const BlockStorage::Memory<double>& ForceResult::getXValue() const {
  return x_value;
}
const BlockStorage::Memory<double>& ForceResult::getYValue() const {
  return y_value;
}
const BlockStorage::Memory<double>& ForceResult::getZValue() const {
  return z_value;
}

/**
 * log message to simulation progress
 * @param simulation_progress
 * @param message message to be logged
 */
void logProgress(Progress& simulation_progress, const std::string_view message) {
  const auto logs_lock = std::scoped_lock(simulation_progress.logs_lock);
  simulation_progress.logs.append(message);
}

void DirectRayCastSimulation(Progress& simulation_progress,
                             const std::filesystem::path& export_directory,
                             const std::vector<Transducer::ITransducer>& transducers,
                             StaticSimulationParameter static_simulation_parameter) {
  const auto process_begin_time = std::chrono::steady_clock::now();

  // start locking the process
  const auto process_lock = std::scoped_lock(simulation_progress.process_lock);
  logProgress(simulation_progress, "Starting LegacySimulation.\n");

  const auto pressure_beg = static_simulation_parameter.getBegin() -
                            static_simulation_parameter.getCellSize() * 2;
  const auto pressure_end =
      (static_simulation_parameter.getEnd() +
       static_simulation_parameter.getCellSize() * 2)
          .snap_to_nearest_component_wise_integer_step(
              pressure_beg,
              Vec3<double>::fromValue(static_simulation_parameter.getCellSize()));

  static_simulation_parameter.setBegin(pressure_beg);
  static_simulation_parameter.setEnd(pressure_end);

  logProgress(simulation_progress, "Calculating direct pressure from transducers.\n");
  const auto pressure_calculation_begin_time = std::chrono::steady_clock::now();
  const auto pressure_result = PressureResult::fromTransducersDirectRayCast(
      transducers, static_simulation_parameter);
  logProgress(simulation_progress,
              fmt::format(FMT_STRING("Done. Took {} s.\n"),
                          std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::steady_clock::now() -
                              pressure_calculation_begin_time)
                              .count()));

  logProgress(simulation_progress, "Calculating potential from pressure complex.\n");
  const auto potential_calculation_begin_time = std::chrono::steady_clock::now();
  const auto potential_result =
      PotentialResult::fromPressureResult(pressure_result, static_simulation_parameter);
  logProgress(simulation_progress,
              fmt::format(FMT_STRING("Done. Took {} s.\n"),
                          std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::steady_clock::now() -
                              potential_calculation_begin_time)
                              .count()));

  logProgress(simulation_progress, "Calculating force from potential.\n");
  const auto force_calculation_begin_time = std::chrono::steady_clock::now();
  const auto force_result =
      ForceResult::fromPotentialResult(potential_result, static_simulation_parameter);
  logProgress(
      simulation_progress,
      fmt::format(FMT_STRING("Done. Took {} s.\n"),
                  std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::steady_clock::now() - force_calculation_begin_time)
                      .count()));

  logProgress(simulation_progress, "Exporting results.\n");

  auto pressure_exp =
      std::ofstream(export_directory / std::string_view("pressure_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  pressure_exp.write(pressure_result.getValue().unsafe_get_raw_bytes(),
                     pressure_result.getValue().size() * sizeof(std::complex<double>));
  pressure_exp.close();

  auto potential_exp =
      std::ofstream(export_directory / std::string_view("potential_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  potential_exp.write(potential_result.getValue().unsafe_get_raw_bytes(),
                      potential_result.getValue().size() * sizeof(double));
  potential_exp.close();

  auto force_x_exp =
      std::ofstream(export_directory / std::string_view("force_x_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  force_x_exp.write(force_result.getXValue().unsafe_get_raw_bytes(),
                    force_result.getXValue().size() * sizeof(double));
  force_x_exp.close();

  auto force_y_exp =
      std::ofstream(export_directory / std::string_view("force_y_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  force_y_exp.write(force_result.getYValue().unsafe_get_raw_bytes(),
                    force_result.getYValue().size() * sizeof(double));
  force_y_exp.close();

  auto force_z_exp =
      std::ofstream(export_directory / std::string_view("force_z_result.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  force_z_exp.write(force_result.getZValue().unsafe_get_raw_bytes(),
                    force_result.getZValue().size() * sizeof(double));
  force_z_exp.close();

  const auto metadata =
      LegacyExportMetadataV1{pressure_result.getCount(),  pressure_result.getBegin(),
                             pressure_result.getEnd(),    potential_result.getCount(),
                             potential_result.getBegin(), potential_result.getEnd(),
                             force_result.getCount(),     force_result.getBegin(),
                             force_result.getEnd()};

  auto metadata_export =
      std::ofstream(export_directory / std::string_view("metadata.bin"),
                    std::fstream::out | std::fstream::trunc | std::fstream::binary);
  metadata_export.write(
      reinterpret_cast<char*>(const_cast<LegacyExportMetadataV1*>(&metadata)),
      sizeof(metadata));
  metadata_export.close();

  logProgress(simulation_progress,
              fmt::format(FMT_STRING("Simulation process done. Took {} s.\n"),
                          std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::steady_clock::now() - process_begin_time)
                              .count()));
}
