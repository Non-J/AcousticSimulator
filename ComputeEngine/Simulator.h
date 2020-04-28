#ifndef ACOUSTICSIMULATOR_SIMULATOR_H
#define ACOUSTICSIMULATOR_SIMULATOR_H

#include <string>
#include <vector>

namespace Simulator {

struct Vec3 {
  double x, y, z;

  bool operator==(const Vec3& rhs) const {
    return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
  }
  bool operator!=(const Vec3& rhs) const { return !(rhs == *this); }
};

struct Transducer {
  std::string id;

  Vec3 position;
  Vec3 target;

  double radius, phase_shift, loss_factor, output_power;

  [[nodiscard]] std::string checkInvalidParameter() const;
};

struct SimulationParameter {
  Vec3 begin;
  Vec3 end;

  double cell_size, frequency, particle_radius, air_density = 1.225, wave_speed = 340;

  [[nodiscard]] std::string checkInvalidParameter() const;
};

}  // namespace Simulator

#endif  // ACOUSTICSIMULATOR_SIMULATOR_H
