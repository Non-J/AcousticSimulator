#include "TransducerLoader.h"
#include <fmt/format.h>
#include <imgui.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../imgui_stdlib/imgui_stdlib.h"
#include "Colors.h"

void Widgets::TransducerLoader::render() {
  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Transducer Loader", nullptr, window_flags);

  // User Input
  auto input =
      ImGui::InputTextMultiline("##DataInput", &user_input, ImVec2(350.0, 150.0));
  auto pasted = ImGui::Button("Paste");

  ImGui::Separator();

  // User Input logic
  if (pasted or input) {
    if (pasted) {
      user_input = std::string{ImGui::GetClipboardText()};
    }

    this->parse_json();
  }

  // Display Result
  if (is_successfully_loaded) {
    ImGui::TextColored(Colors::Green300, "Transducer configuration loaded");
  } else {
    ImGui::TextColored(Colors::Red300, "Transducer configuration NOT loaded");
  }
  ImGui::PushTextWrapPos(350.0);
  ImGui::TextUnformatted(parsing_result.c_str());
  ImGui::PopTextWrapPos();

  ImGui::End();
}

auto Widgets::TransducerLoader::getTransducers() const {
  if (this->is_successfully_loaded) {
    return std::make_optional(std::reference_wrapper(this->transducers));
  }

  return std::optional<decltype(std::reference_wrapper(this->transducers))>();
}

template <typename T>
Vec3<T> parse_vec3(const nlohmann::json& json) {
  return Vec3<T>(json.at(0).get<T>(), json.at(1).get<T>(), json.at(2).get<T>());
}

void Widgets::TransducerLoader::parse_json() {
  is_successfully_loaded = false;
  parsing_result = "Parsing...";

  if (user_input.empty()) {
    parsing_result = "User input is empty";
    return;
  }

  nlohmann::json data;

  try {
    data = nlohmann::json::parse(user_input);
  } catch (std::exception& e) {
    parsing_result = e.what();
    return;
  }

  if (not data.is_array()) {
    parsing_result = "User input is not array";
    return;
  }

  transducers.clear();

  std::size_t auto_id = 0;

  for (const auto& item : data) {
    const auto id =
        item.value("id", fmt::format(FMT_STRING("transducer #{}"), ++auto_id));

    try {
      const auto position = parse_vec3<double>(item.at("position"));
      const auto target = parse_vec3<double>(item.at("target"));
      const auto radius = item.at("radius").get<double>();
      const auto phase_shift = item.at("phase_shift").get<double>();
      const auto loss_factor = item.at("loss_factor").get<double>();
      const auto output_power = item.at("output_power").get<double>();
      const auto frequency = item.at("frequency").get<double>();

      transducers.push_back(std::make_unique<Transducer::StaticTransducer>(
          id, position, target, radius, frequency, phase_shift,
          output_power * loss_factor));
    } catch (std::exception& e) {
      parsing_result =
          fmt::format(FMT_STRING("Error while loading {}:\n{}"), id, e.what());
      return;
    }
  }

  is_successfully_loaded = true;
  parsing_result =
      fmt::format(FMT_STRING("Loaded {} transducers."), transducers.size());
}
