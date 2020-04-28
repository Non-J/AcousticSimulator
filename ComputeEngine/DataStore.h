#ifndef ACOUSTICSIMULATOR_DATASTORE_H
#define ACOUSTICSIMULATOR_DATASTORE_H

#include <nlohmann/json.hpp>
#include <vector>
#include "Simulator.h"

namespace DataStore {

// Store which tool widget is opened
struct ToolboxOpen {
  bool ImGuiDemo = false;
  bool TransducerConfiguration = true;
  bool SimulationControl = true;
  bool BenchmarkingWidget = false;
};

// Store data for Simulation
struct SimulationConfiguration {
  std::vector<Simulator::Transducer> transducers;
  Simulator::SimulationParameter simulation_parameter;
};

// Structure for passing around
struct GlobalDataStore {
  ToolboxOpen toolbox_open;
  SimulationConfiguration simulation_data;
};

namespace JSONConvert {

[[nodiscard]] Simulator::Vec3 to_vec3(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_vec3(const Simulator::Vec3& vec3);

[[nodiscard]] Simulator::Transducer to_transducer(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_transducer(const Simulator::Transducer& transducer);

[[nodiscard]] Simulator::SimulationParameter to_simulation_parameter(
    const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_simulation_parameter(
    const Simulator::SimulationParameter& simulation_parameter);

}  // namespace JSONConvert

}  // namespace DataStore

#endif  // ACOUSTICSIMULATOR_DATASTORE_H
