#pragma once

#include <atomic>
#include <filesystem>
#include <mutex>
#include <string>
#include "Vec3.h"

namespace Computation {

struct Transducer {
  std::string id;

  Vec3<double> position;
  Vec3<double> target;

  double radius, phase_shift, loss_factor, output_power;

  [[nodiscard]] std::string checkInvalidParameter() const;
};

struct SimulationParameter {
  Vec3<double> begin;
  Vec3<double> end;

  double cell_size, frequency, particle_radius, air_density, wave_speed;

  [[nodiscard]] std::string checkInvalidParameter() const;
};

void simulationProcess(std::atomic<bool>* process_lock_simulation_running,
                       std::mutex* simulation_logging_lock,
                       std::string* simulation_logging,
                       std::filesystem::path export_directory,
                       std::vector<Computation::Transducer> transducers,
                       Computation::SimulationParameter simulation_parameter);

}  // namespace Computation
