//#include <imgui.h>
//#include "../Vec3.h"
//#include "Colors.h"
//#include "Widgets.h"
//
//void Widgets::SimulationConfig(Config::SimulationParameter& simulation_parameters) {
//  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
//  ImGui::Begin("Simulation Control", nullptr, window_flags);
//
//  // This marks if any input field changed
//  auto input = false;
//
//  ImGui::TextUnformatted("Start simulation region");
//  input |= ImGui::InputDouble("X##begin", &simulation_parameters.begin.x, NULL, NULL,
//                              "%.3e m", ImGuiInputTextFlags_CharsScientific);
//  input |= ImGui::InputDouble("Y##begin", &simulation_parameters.begin.y, NULL, NULL,
//                              "%.3e m", ImGuiInputTextFlags_CharsScientific);
//  input |= ImGui::InputDouble("Z##begin", &simulation_parameters.begin.z, NULL, NULL,
//                              "%.3e m", ImGuiInputTextFlags_CharsScientific);
//
//  ImGui::TextUnformatted("Stop simulation region");
//  input |= ImGui::InputDouble("X##end", &simulation_parameters.end.x, NULL, NULL,
//                              "%.3e m", ImGuiInputTextFlags_CharsScientific);
//  input |= ImGui::InputDouble("Y##end", &simulation_parameters.end.y, NULL, NULL,
//                              "%.3e m", ImGuiInputTextFlags_CharsScientific);
//  input |= ImGui::InputDouble("Z##end", &simulation_parameters.end.z, NULL, NULL,
//                              "%.3e m", ImGuiInputTextFlags_CharsScientific);
//
//  ImGui::TextUnformatted("Simulation cell size");
//  input |= ImGui::InputDouble("##cell_size", &simulation_parameters.cell_size, NULL,
//                              NULL, "%.3e m", ImGuiInputTextFlags_CharsScientific);
//  ImGui::Text("Estimated cell count\n%.0f",
//              ((simulation_parameters.end - simulation_parameters.begin).elem_abs() /
//                   simulation_parameters.cell_size +
//               1.0)
//                  .product());
//
//  ImGui::TextUnformatted("Transducer frequency");
//  input |= ImGui::InputDouble("##frequency", &simulation_parameters.frequency, NULL,
//                              NULL, "%.3f Hz", ImGuiInputTextFlags_CharsScientific);
//  ImGui::Text("Wavelength\n%e m",
//              simulation_parameters.air_wave_speed / simulation_parameters.frequency);
//
//  ImGui::TextUnformatted("Air density");
//  input |= ImGui::InputDouble("##air_density", &simulation_parameters.air_density, NULL,
//                              NULL, "%.3f kg/m3", ImGuiInputTextFlags_CharsScientific);
//
//  ImGui::TextUnformatted("Wave speed (in air)");
//  input |=
//      ImGui::InputDouble("##air_wave_speed", &simulation_parameters.air_wave_speed,
//                         NULL, NULL, "%.3f m/s", ImGuiInputTextFlags_CharsScientific);
//
//  ImGui::TextUnformatted("Particle radius");
//  input |=
//      ImGui::InputDouble("##particle_radius", &simulation_parameters.particle_radius,
//                         NULL, NULL, "%.3e m", ImGuiInputTextFlags_CharsScientific);
//
//  ImGui::TextUnformatted("Particle density");
//  input |=
//      ImGui::InputDouble("##particle_density", &simulation_parameters.particle_density,
//                         NULL, NULL, "%.3f kg/m3", ImGuiInputTextFlags_CharsScientific);
//
//  ImGui::TextUnformatted("Wave speed (in particle)");
//  input |= ImGui::InputDouble("##particle_wave_speed",
//                              &simulation_parameters.particle_wave_speed, NULL, NULL,
//                              "%.3f m/s", ImGuiInputTextFlags_CharsScientific);
//
//  input |= ImGui::Checkbox("Assume large particle density",
//                           &simulation_parameters.assume_large_particle_density);
//
//  // Check invalid parameter if input changed
//  static auto invalid_parameter = simulation_parameters.checkInvalidParameter();
//  if (input) {
//    invalid_parameter = simulation_parameters.checkInvalidParameter();
//  }
//
//  ImGui::PushTextWrapPos(250);
//  if (simulation_parameters.assume_large_particle_density) {
//    ImGui::TextColored(Colors::Amber300,
//                       "Assume that particle density approach infinity. Some "
//                       "parameters are ignored.");
//  }
//  if (invalid_parameter.empty()) {
//    ImGui::TextColored(Colors::Green300, "Simulation parameters loaded");
//
//    ImGui::Text("K1: %e", simulation_parameters.constant_k1());
//    ImGui::Text("K2: %e", simulation_parameters.constant_k2());
//  } else {
//    ImGui::TextColored(Colors::Red300, "Simulation parameters NOT loaded");
//    ImGui::TextColored(Colors::Red300, "%s", invalid_parameter.c_str());
//  }
//  ImGui::PopTextWrapPos();
//
//  ImGui::Separator();
//
//  // Prevent clipboard button from activating multiple times
//  static auto clipboard_button_pressed = false;
//
//  // Button to save and load from clipboard
//  auto save_to_clipboard = ImGui::Button("Copy Parameters");
//  ImGui::SameLine();
//  auto load_from_clipboard = ImGui::Button("Paste Parameters");
//
//  // Implement logic to save and load from clipboard
//  if (save_to_clipboard and not clipboard_button_pressed) {
//    clipboard_button_pressed = true;
//    ImGui::SetClipboardText(JSONConvert::from_simulation_parameter(simulation_parameters).dump().c_str());
//  }
//  if (load_from_clipboard and not clipboard_button_pressed) {
//    clipboard_button_pressed = true;
//    try {
//      auto json = nlohmann::json::parse(ImGui::GetClipboardText());
//      simulation_parameters = JSONConvert::to_simulation_parameter(json);
//      invalid_parameter = simulation_parameters.checkInvalidParameter();
//    } catch (const std::exception& e) {
//      invalid_parameter = e.what();
//    }
//  }
//  if (not save_to_clipboard and not load_from_clipboard) {
//    clipboard_button_pressed = false;
//  }
//
//  ImGui::End();
//}