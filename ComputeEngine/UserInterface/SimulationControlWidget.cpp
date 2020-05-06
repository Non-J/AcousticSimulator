#include <imgui.h>
#include "../Computation/Vec3.h"
#include "../Utilities/DataStore.h"
#include "Colors.h"
#include "UserInterface.h"

void UserInterface::SimulationControlWidget(
    DataStore::GlobalDataStore& global_data_store) {
  auto& sp = global_data_store.simulation_data.simulation_parameter;

  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Simulation Control",
               &global_data_store.toolbox_open.SimulationControlWidget, window_flags);

  // This marks if any input field changed
  auto input = false;

  ImGui::TextUnformatted("Start simulation region");
  input |= ImGui::InputDouble("X##begin", &sp.begin.x, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);
  input |= ImGui::InputDouble("Y##begin", &sp.begin.y, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);
  input |= ImGui::InputDouble("Z##begin", &sp.begin.z, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);

  ImGui::TextUnformatted("Stop simulation region");
  input |= ImGui::InputDouble("X##end", &sp.end.x, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);
  input |= ImGui::InputDouble("Y##end", &sp.end.y, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);
  input |= ImGui::InputDouble("Z##end", &sp.end.z, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);

  ImGui::TextUnformatted("Simulation cell size");
  input |= ImGui::InputDouble("##cell_size", &sp.cell_size, NULL, NULL, "%.3e m",
                              ImGuiInputTextFlags_CharsScientific);
  ImGui::Text("Estimated cell count\n%.0f",
              ((sp.end - sp.begin).elem_abs() / sp.cell_size).product());

  ImGui::TextUnformatted("Transducer frequency");
  input |= ImGui::InputDouble("##frequency", &sp.frequency, NULL, NULL, "%.3f Hz",
                              ImGuiInputTextFlags_CharsScientific);
  ImGui::Text("Wavelength\n%e m", sp.air_wave_speed / sp.frequency);

  ImGui::TextUnformatted("Air density");
  input |= ImGui::InputDouble("##air_density", &sp.air_density, NULL, NULL,
                              "%.3f kg/m3", ImGuiInputTextFlags_CharsScientific);

  ImGui::TextUnformatted("Wave speed (in air)");
  input |= ImGui::InputDouble("##air_wave_speed", &sp.air_wave_speed, NULL, NULL,
                              "%.3f m/s", ImGuiInputTextFlags_CharsScientific);

  ImGui::TextUnformatted("Particle radius");
  input |= ImGui::InputDouble("##particle_radius", &sp.particle_radius, NULL, NULL,
                              "%.3e m", ImGuiInputTextFlags_CharsScientific);

  ImGui::TextUnformatted("Particle density");
  input |= ImGui::InputDouble("##particle_density", &sp.particle_density, NULL, NULL,
                              "%.3f kg/m3", ImGuiInputTextFlags_CharsScientific);

  ImGui::TextUnformatted("Wave speed (in particle)");
  input |= ImGui::InputDouble("##particle_wave_speed", &sp.particle_wave_speed, NULL,
                              NULL, "%.3f m/s", ImGuiInputTextFlags_CharsScientific);

  input |= ImGui::Checkbox("Assume large particle density",
                           &sp.assume_large_particle_density);

  // Check invalid parameter if input changed
  static auto invalid_parameter = sp.checkInvalidParameter();
  if (input) {
    invalid_parameter = sp.checkInvalidParameter();
  }

  ImGui::PushTextWrapPos(250);
  if (sp.assume_large_particle_density) {
    ImGui::TextColored(
        Colors::Amber400,
        "Assume that particle density approach infinity. Some parameters are ignored.");
  }
  if (invalid_parameter.empty()) {
    ImGui::TextColored(Colors::Green400, "Simulation parameters loaded");

    ImGui::Text("K1: %e", sp.constant_k1());
    ImGui::Text("K2: %e", sp.constant_k2());
  } else {
    ImGui::TextColored(Colors::Red400, "Simulation parameters NOT loaded");
    ImGui::TextColored(Colors::Red400, "%s", invalid_parameter.c_str());
  }
  ImGui::PopTextWrapPos();

  ImGui::Separator();

  // Prevent clipboard button from activating multiple times
  static auto clipboard_button_pressed = false;

  // Button to save and load from clipboard
  auto save_to_clipboard = ImGui::Button("Copy Parameters");
  ImGui::SameLine();
  auto load_from_clipboard = ImGui::Button("Paste Parameters");

  // Implement logic to save and load from clipboard
  if (save_to_clipboard and not clipboard_button_pressed) {
    clipboard_button_pressed = true;
    ImGui::SetClipboardText(JSONConvert::from_simulation_parameter(sp).dump().c_str());
  }
  if (load_from_clipboard and not clipboard_button_pressed) {
    clipboard_button_pressed = true;
    try {
      auto json = nlohmann::json::parse(ImGui::GetClipboardText());
      sp = JSONConvert::to_simulation_parameter(json);
      invalid_parameter = sp.checkInvalidParameter();
    } catch (const std::exception& e) {
      invalid_parameter = e.what();
    }
  }
  if (not save_to_clipboard and not load_from_clipboard) {
    clipboard_button_pressed = false;
  }

  ImGui::End();
}