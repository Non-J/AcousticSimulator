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

  [[nodiscard]] std::string checkInvalidParameter() const {
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
  };
};

struct SimulationParameter {
  Vec3<double> begin;
  Vec3<double> end;

  double cell_size, frequency, particle_radius, air_density, wave_speed;

  [[nodiscard]] std::string checkInvalidParameter() const {
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
  };
};

void simulationProcess(std::atomic<bool>* process_lock_simulation_running,
                       std::mutex* simulation_logging_lock,
                       std::string* simulation_logging,
                       std::filesystem::path export_directory,
                       std::vector<Computation::Transducer> transducers,
                       Computation::SimulationParameter simulation_parameter);

}  // namespace Computation
