#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include "Computation/Simulator.h"
#include "Computation/Vec3.h"

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

namespace JSONConvert {

template <typename T>
[[nodiscard]] Vec3<T> to_vec3(const nlohmann::json& json);
template <typename T>
[[nodiscard]] nlohmann::json from_vec3(const Vec3<T>& vec3);

[[nodiscard]] Computation::Transducer to_transducer(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_transducer(const Computation::Transducer& transducer);

[[nodiscard]] Computation::SimulationParameter to_simulation_parameter(
    const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_simulation_parameter(
    const Computation::SimulationParameter& simulation_parameter);

}  // namespace JSONConvert

}  // namespace DataStore
