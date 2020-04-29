#ifndef ACOUSTICSIMULATOR_DATASTORE_H
#define ACOUSTICSIMULATOR_DATASTORE_H

#include <atomic>
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

//
struct ProcessLocks {
  std::atomic<bool> simulation_running = false;
};

// Structure for passing around
struct GlobalDataStore {
  ToolboxOpen toolbox_open;
  SimulationConfiguration simulation_data;
  ProcessLocks process_locks;
};

namespace JSONConvert {

template <typename T>
[[nodiscard]] Computation::Vec3<T> to_vec3(const nlohmann::json& json);
template <typename T>
[[nodiscard]] nlohmann::json from_vec3(const Computation::Vec3<T>& vec3);

[[nodiscard]] Computation::Transducer to_transducer(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_transducer(const Computation::Transducer& transducer);

[[nodiscard]] Computation::SimulationParameter to_simulation_parameter(
    const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_simulation_parameter(
    const Computation::SimulationParameter& simulation_parameter);

}  // namespace JSONConvert

}  // namespace DataStore

#endif  // ACOUSTICSIMULATOR_DATASTORE_H
