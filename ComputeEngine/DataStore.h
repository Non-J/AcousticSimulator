#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include "Computation/Simulator.h"

namespace DataStore {

// Store which tool widget is opened
struct ToolboxOpen {
  bool ImGuiDemo = false;
  bool TransducerConfigurationWidget = true;
  bool SimulationControlWidget = true;
  bool BenchmarkingWidget = false;
  bool RunSimulationWidget = true;
};

// Store data for Simulation
struct SimulationConfiguration {
  std::vector<Computation::Transducer> transducers;
  Computation::SimulationParameter simulation_parameter;
};

// Structure for passing around
struct GlobalDataStore {
  ToolboxOpen toolbox_open;
  SimulationConfiguration simulation_data;
};

}  // namespace DataStore

namespace JSONConvert {

[[nodiscard]] Computation::Transducer to_transducer(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_transducer(const Computation::Transducer& transducer);

[[nodiscard]] Computation::SimulationParameter to_simulation_parameter(
    const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_simulation_parameter(
    const Computation::SimulationParameter& simulation_parameter);

}  // namespace JSONConvert