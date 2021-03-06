#include <fmt/format.h>
#include <imgui.h>
#include <atomic>
#include <filesystem>
#include <thread>
#include "../Computation/Config.h"
#include "../Computation/Simulator.h"
#include "../Utilities/AtomicLogger.h"
#include "../imgui_stdlib/imgui_stdlib.h"
#include "Colors.h"
#include "Widgets.h"

void Widgets::SimulationRunner(
    const std::vector<Config::Transducer>& transducers,
    const Config::SimulationParameter& simulation_parameters) {
  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Run Simulation", nullptr, window_flags);

  static auto export_directory_name = std::string("simulation_result");
  static auto simulation_thread_forking_error = std::string();

  static auto simulation_running = std::atomic<bool>(false);
  static auto simulation_logging = AtomicLogger::AtomicLogger();

  // Let user specify where to export simulation result
  ImGui::TextUnformatted("Export folder path");
  ImGui::PushItemWidth(350);
  ImGui::InputText("##exportname", &export_directory_name);
  ImGui::PopItemWidth();

  const auto export_directory = std::filesystem::current_path() / export_directory_name;

  // Preliminary check if the specified export directory is ok
  ImGui::PushTextWrapPos(350);
  try {
    ImGui::TextUnformatted("Resolved path: ");
    ImGui::Text("%ls", std::filesystem::weakly_canonical(export_directory).c_str());
    ImGui::TextColored(Colors::Amber300, "Note: Existing files may be overwritten.");
  } catch (const std::exception& e) {
    ImGui::TextColored(Colors::Red300, "Filesystem error: %s", e.what());
  }
  ImGui::PopTextWrapPos();

  ImGui::Separator();

  auto run_simulation_button = ImGui::Button("Run Simulation", ImVec2(350, 20));

  if (simulation_running.load()) {
    ImGui::TextColored(Colors::Blue300, "Simulation is running.");
  } else {
    ImGui::TextUnformatted("Simulation is not running.");

    if (run_simulation_button) {
      // Simulation is not running and the user pressed the button to start

      try {
        // Checks if transducers are invalid
        for (const auto& transducer : transducers) {
          const auto invalid_transducer = transducer.checkInvalidParameter();
          if (not invalid_transducer.empty()) {
            throw std::exception(invalid_transducer.c_str());
          }
        }

        // Checks if parameters are invalid
        const auto invalid_simulation_parameter =
            simulation_parameters.checkInvalidParameter();
        if (not invalid_simulation_parameter.empty()) {
          throw std::exception(invalid_simulation_parameter.c_str());
        }

        // Create export directory
        std::filesystem::create_directories(export_directory);

        // Start simulation
        simulation_running.store(true);
        simulation_thread_forking_error = std::string();
        simulation_logging.clear();

        // Fork simulation thread
        auto simulation_thread = std::thread(
            Computation::simulationProcess, &simulation_running, &simulation_logging,
            export_directory, transducers,
            simulation_parameters);
        simulation_thread.detach();

      } catch (const std::exception& e) {
        // Error when forking thread
        simulation_running.store(false);
        simulation_thread_forking_error =
            fmt::format(FMT_STRING("Unable to start simulation: {:s}"), e.what());
      }
    }
  }

  // Report error to user
  ImGui::PushTextWrapPos(350);
  if (not simulation_thread_forking_error.empty()) {
    ImGui::TextColored(Colors::Red300, "%s", simulation_thread_forking_error.c_str());
  }
  {
    const auto result = simulation_logging.read();
    ImGui::TextUnformatted(result.second.data());
  }
  ImGui::PopTextWrapPos();

  ImGui::End();
}