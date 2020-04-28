#include "DataStore.h"

namespace DataStore {

void ControlLoop(GlobalDataStore& global_data_store) {
  auto& tc_ui = global_data_store.transducer_configuration_ui;

  // Parse Transducer Configuration
  if (not tc_ui.parse_text_attempted) {
    tc_ui.parse_text_attempted = true;
    tc_ui.parse_text_result = "Parsing input...";

    try {
      if(tc_ui.user_input_text.empty()){
        throw std::exception("Input is empty");
      }

      const auto parsedResult = nlohmann::json::parse(tc_ui.user_input_text);

      if (not parsedResult.is_array()) {
        throw std::exception("Input root is not an array");
      }

      tc_ui.parse_text_success = true;
      tc_ui.parse_text_result = parsedResult.dump();
    } catch (const std::exception& e) {
      tc_ui.parse_text_success = false;
      tc_ui.parse_text_result = e.what();
    }
  }


}

}  // namespace DataStore
