#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "DataStore.h"
#include "UserInterface/UserInterface.h"

int main(int /*argc*/, char** /*argv*/) {
  auto global_data_store = DataStore::GlobalDataStore();

  // Window Setup
  auto window = sf::RenderWindow(sf::VideoMode(1200, 675), "ComputeEngine");
  window.setFramerateLimit(30);
  ImGui::SFML::Init(window);
  UserInterface::SetupStyle();

  // Main loop
  auto deltaClock = sf::Clock();
  while (window.isOpen()) {
    auto event = sf::Event();
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    ImGui::SFML::Update(window, deltaClock.restart());

    UserInterface::ToolsMenu(global_data_store);

    if (global_data_store.toolbox_open.ImGuiDemo) {
      ImGui::ShowDemoWindow(&global_data_store.toolbox_open.ImGuiDemo);
    }
    if (global_data_store.toolbox_open.TransducerConfigurationWidget) {
      UserInterface::TransducerConfigurationWidget(global_data_store);
    }
    if (global_data_store.toolbox_open.SimulationControlWidget) {
      UserInterface::SimulationControlWidget(global_data_store);
    }
    if (global_data_store.toolbox_open.RunSimulationWidget) {
      UserInterface::RunSimulationWidget(global_data_store);
    }
    if (global_data_store.toolbox_open.BenchmarkingWidget) {
      UserInterface::BenchmarkingWidget(global_data_store);
    }

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}
