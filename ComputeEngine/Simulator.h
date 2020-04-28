#ifndef ACOUSTICSIMULATOR_SIMULATOR_H
#define ACOUSTICSIMULATOR_SIMULATOR_H

#include <string>
#include <vector>

namespace Simulator {

struct Vec3 {
  double x, y, z;
};

struct Transducer {
  std::string id;

  Vec3 position;
  Vec3 target;

  double radius, phase_shift, loss_factor, output_power, wavelength;
};

}  // namespace Simulator

#endif  // ACOUSTICSIMULATOR_SIMULATOR_H
