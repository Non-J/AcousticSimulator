#include <imgui.h>
#include "../DataStore.h"
#include "../SourceConfig.h"
#include "../imgui_stdlib/imgui_stdlib.h"
#include "UserInterface.h"

void UserInterface::TransducerConfigurationWidget(
    DataStore::GlobalDataStore& global_data_store) {
  auto& tc_ui = global_data_store.transducer_configuration_ui;

  auto windowConfig = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Transducer Configuration",
               &global_data_store.toolbox_open.TransducerConfiguration, windowConfig);

  ImGui::Text("Paste Transducer Configuration Data: ");
  ImGui::InputTextMultiline("Data Input", &tc_ui.user_input_text, ImVec2(350.0, 150.0));
  auto textPasted = ImGui::Button("Paste");
  ImGui::Separator();

  if (textPasted) {
    tc_ui.user_input_text = std::string{ImGui::GetClipboardText()};
  }

  if (textPasted or tc_ui.last_text_size != tc_ui.user_input_text.size()) {
    tc_ui.last_text_size = tc_ui.user_input_text.size();
    tc_ui.parse_text_attempted = false;
    tc_ui.parse_text_success = false;
  }

  if (tc_ui.parse_text_success) {
    ImGui::TextColored(Colors::green400, "Transducer Configuration Loaded");
  } else {
    ImGui::TextColored(Colors::amber400, "Transducer Configuration NOT Loaded");
  }

  ImGui::Separator();
  ImGui::PushTextWrapPos(350.0);
  ImGui::TextUnformatted(tc_ui.parse_text_result.c_str());
  ImGui::PopTextWrapPos();

  ImGui::End();
}