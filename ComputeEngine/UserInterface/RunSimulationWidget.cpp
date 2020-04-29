#include <fmt/format.h>
#include <imgui.h>
#include <thread>
#include <utility>
#include "../Computation/Simulator.h"
#include "../DataStore.h"
#include "../SourceConfig.h"
#include "../imgui_stdlib/imgui_stdlib.h"
#include "UserInterface.h"

void UserInterface::RunSimulationWidget(DataStore::GlobalDataStore& global_data_store) {
  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Run Simulation",
               &global_data_store.toolbox_open.TransducerConfigurationWidget,
               window_flags);

  static auto export_name = std::string();
  static auto simulation_thread_forking_error = std::string();

  ImGui::TextUnformatted("Export name");
  ImGui::InputText("##exportname", &export_name);
  // TODO: Implement check for valid export name
  // TODO: Create directory for export here so that we can ask if the user want to
  // overwrite
  ImGui::TextUnformatted("NOTE: Export name must be valid filename.");

  ImGui::Separator();

  auto run_simulation_button = ImGui::Button("Run Simulation", ImVec2(250, 20));

  if (global_data_store.process_locks.simulation_running.load()) {
    ImGui::TextColored(Colors::blue400, "Simulation is running.");
  } else {
    ImGui::TextUnformatted("Simulation is not running.");

    if (run_simulation_button) {
      // Lock process
      global_data_store.process_locks.simulation_running.store(true);
      simulation_thread_forking_error = std::string();

      try {
        // Checks
        if (export_name.empty()) {
          throw std::exception("Export name is empty");
        }
        if (global_data_store.simulation_data.transducers.empty()) {
          throw std::exception("Transducers list is empty");
        }
        for (const Computation::Transducer& transducer :
             global_data_store.simulation_data.transducers) {
          const auto invalid_transducer = transducer.checkInvalidParameter();
          if (not invalid_transducer.empty()) {
            throw std::exception(invalid_transducer.c_str());
          }
        }
        const auto invalid_simulation_parameter =
            global_data_store.simulation_data.simulation_parameter
                .checkInvalidParameter();
        if (not invalid_simulation_parameter.empty()) {
          throw std::exception(invalid_simulation_parameter.c_str());
        }

        // Start simulation
        auto simulation_thread =
            std::thread(Computation::simulationProcess,
                        &global_data_store.process_locks.simulation_running,
                        export_name, global_data_store.simulation_data.transducers,
                        global_data_store.simulation_data.simulation_parameter);
        simulation_thread.detach();

      } catch (const std::exception& e) {
        global_data_store.process_locks.simulation_running.store(false);
        simulation_thread_forking_error =
            fmt::format(FMT_STRING("Unable to start simulation: {:s}"), e.what());
      }
    }
  }

  if (not simulation_thread_forking_error.empty()) {
    ImGui::TextColored(Colors::amber400, "%s", simulation_thread_forking_error.c_str());
  }

  ImGui::End();
}