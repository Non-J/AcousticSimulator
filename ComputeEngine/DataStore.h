#ifndef ACOUSTICSIMULATOR_DATASTORE_H
#define ACOUSTICSIMULATOR_DATASTORE_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "Simulator.h"

namespace DataStore {

// Store which tool widget is opened
struct ToolboxOpen {
  bool ImGuiDemo = false;
  bool TransducerConfiguration = true;
};

// Store UI data for Transducer Configuration Widget
struct TransducerConfigurationUI {
  std::string user_input_text;
  std::size_t last_text_size;
  bool parse_text_attempted;
  bool parse_text_success;
  std::string parse_text_result;
};

// Store data for Simulation
struct SimulationData {
  std::vector<Simulator::Transducer> transducers;
};

// Structure for passing around
struct GlobalDataStore {
  ToolboxOpen toolbox_open;
  TransducerConfigurationUI transducer_configuration_ui;
  SimulationData simulation_data;
};

void ControlLoop(GlobalDataStore& global_data_store);

}  // namespace DataStore

#endif  // ACOUSTICSIMULATOR_DATASTORE_H
