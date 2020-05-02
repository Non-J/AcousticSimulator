#include <fmt/format.h>
#include <imgui.h>
#include "../DataStore.h"
#include "../imgui_stdlib/imgui_stdlib.h"
#include "Colors.h"
#include "UserInterface.h"

void UserInterface::TransducerConfigurationWidget(
    DataStore::GlobalDataStore& global_data_store) {
  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Transducer Configuration",
               &global_data_store.toolbox_open.TransducerConfigurationWidget,
               window_flags);

  // User Input
  ImGui::TextUnformatted("Transducer configuration data");
  static auto user_input_text = std::string();
  auto input =
      ImGui::InputTextMultiline("##DataInput", &user_input_text, ImVec2(350.0, 150.0));
  auto pasted = ImGui::Button("Paste");

  ImGui::Separator();

  static auto parse_text_success = false;
  static auto parse_text_result = std::string();

  // User Input logic
  if (pasted or input) {
    if (pasted) {
      user_input_text = std::string{ImGui::GetClipboardText()};
    }

    // Parsing logic
    try {
      if (user_input_text.empty()) {
        throw std::exception("Input is empty");
      }

      const auto parsedResult = nlohmann::json::parse(user_input_text);

      if (not parsedResult.is_array()) {
        throw std::exception("Input root is not an array");
      }

      auto& transducers = global_data_store.simulation_data.transducers;
      transducers.clear();

      for (const auto& item : parsedResult) {
        auto parsedItem = JSONConvert::to_transducer(item);

        const auto invalidParameter = parsedItem.checkInvalidParameter();
        if (not invalidParameter.empty()) {
          throw std::exception(
              fmt::format(
                  FMT_STRING("Transducer '{:s}' has an invalid parameter: {:s}"),
                  parsedItem.id, invalidParameter)
                  .c_str());
        }

        transducers.push_back(std::move(parsedItem));
      }

      parse_text_success = true;
      parse_text_result =
          fmt::format(FMT_STRING("Transducer count: {:d}"), transducers.size());
    } catch (const std::exception& e) {
      parse_text_success = false;
      parse_text_result = e.what();
    }
  }

  // Display Result
  if (parse_text_success) {
    ImGui::TextColored(Colors::Green400, "Transducer configuration loaded");
  } else {
    ImGui::TextColored(Colors::Red400, "Transducer configuration NOT loaded");
  }
  ImGui::PushTextWrapPos(350.0);
  ImGui::TextUnformatted(parse_text_result.c_str());
  ImGui::PopTextWrapPos();

  ImGui::End();
}