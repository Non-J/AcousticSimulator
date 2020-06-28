#pragma once

#include <vector>
#include "../Computation/Config.h"

namespace Widgets {

void TransducerConfig(std::vector<Config::Transducer>& transducers);
void SimulationConfig(Config::SimulationParameter& simulation_parameters);
void SimulationRunner(const std::vector<Config::Transducer>& transducers,
                      const Config::SimulationParameter& simulation_parameters);

}  // namespace Widgets
