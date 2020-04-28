#include "Simulator.h"

namespace Simulator {

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

}  // namespace Simulator