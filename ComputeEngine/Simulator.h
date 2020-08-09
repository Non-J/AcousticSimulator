#pragma once

#include <complex>
#include <cstddef>
#include <filesystem>
#include <mutex>
#include <string>
#include <vector>
#include "BlockStorage.h"
#include "SimulationParameter.h"
#include "Transducer.h"

namespace Simulator {

struct Progress {
  // lock whenever the simulation process is running
  std::mutex process_lock;
  // lock whenever trying to read or write to logs
  std::mutex logs_lock;
  std::string logs;
};

class PressureResult {
  const Vec3<double> begin;
  const Vec3<double> end;
  const Vec3<std::size_t> count;
  const BlockStorage::Interpolation interpolate;
  BlockStorage::Memory<std::complex<double>> value;

 protected:
  PressureResult(const Vec3<double>& Begin,
                 const Vec3<double>& End,
                 double cell_size);

 public:
  static PressureResult fromTransducersDirectRayCast(
      const std::vector<Transducer::ITransducer>& transducers,
      const StaticSimulationParameter& static_simulation_parameter);

  [[nodiscard]] const Vec3<double>& getBegin() const;
  [[nodiscard]] const Vec3<double>& getEnd() const;
  [[nodiscard]] const Vec3<std::size_t>& getCount() const;
  [[nodiscard]] const BlockStorage::Interpolation& getInterpolate() const;
  [[nodiscard]] const BlockStorage::Memory<std::complex<double>>& getValue() const;
};

class PotentialResult {
  const Vec3<double> begin;
  const Vec3<double> end;
  const Vec3<std::size_t> count;
  const BlockStorage::Interpolation interpolate;
  BlockStorage::Memory<double> value;

 protected:
  PotentialResult(const Vec3<double>& Begin,
                  const Vec3<double>& End,
                  double cell_size);

 public:
  static PotentialResult fromPressureResult(
      const PressureResult& pressure_result,
      const StaticSimulationParameter& static_simulation_parameter);

  [[nodiscard]] const Vec3<double>& getBegin() const;
  [[nodiscard]] const Vec3<double>& getEnd() const;
  [[nodiscard]] const Vec3<std::size_t>& getCount() const;
  [[nodiscard]] const BlockStorage::Interpolation& getInterpolate() const;
  [[nodiscard]] const BlockStorage::Memory<double>& getValue() const;
};

class ForceResult {
  const Vec3<double> begin;
  const Vec3<double> end;
  const Vec3<std::size_t> count;
  const BlockStorage::Interpolation interpolate;
  BlockStorage::Memory<double> x_value;
  BlockStorage::Memory<double> y_value;
  BlockStorage::Memory<double> z_value;

 protected:
  ForceResult(const Vec3<double>& Begin,
              const Vec3<double>& End,
              double cell_size);

 public:
  static ForceResult fromPotentialResult(
      const PotentialResult& potential_result,
      const StaticSimulationParameter& static_simulation_parameter);

  [[nodiscard]] const Vec3<double>& getBegin() const;
  [[nodiscard]] const Vec3<double>& getEnd() const;
  [[nodiscard]] const Vec3<std::size_t>& getCount() const;
  [[nodiscard]] const BlockStorage::Interpolation& getInterpolate() const;
  [[nodiscard]] const BlockStorage::Memory<double>& getXValue() const;
  [[nodiscard]] const BlockStorage::Memory<double>& getYValue() const;
  [[nodiscard]] const BlockStorage::Memory<double>& getZValue() const;
};

struct LegacyExportMetadataV1 {
  static const std::uint16_t version = 0xacb;
  const Vec3<std::size_t> pressure_cnt;
  const Vec3<double> pressure_beg;
  const Vec3<double> pressure_end;
  const Vec3<std::size_t> potential_cnt;
  const Vec3<double> potential_beg;
  const Vec3<double> potential_end;
  const Vec3<std::size_t> force_cnt;
  const Vec3<double> force_beg;
  const Vec3<double> force_end;
};

void LegacySimulation(Progress& simulation_progress,
                      const std::filesystem::path& export_directory,
                      const std::vector<Transducer::ITransducer>& transducers,
                      StaticSimulationParameter static_simulation_parameter);

}  // namespace Simulator
