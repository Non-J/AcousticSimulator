#include <imgui-SFML.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "Widgets/SetupStyle.h"
#include "Widgets/TransducerLoader.h"

int main(int /*argc*/, char** /*argv*/) {
  // Window Setup
  auto window = sf::RenderWindow(sf::VideoMode(1200, 675), "ComputeEngine");
  window.setFramerateLimit(30);
  ImGui::SFML::Init(window, false);
  Widgets::SetupStyle();

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

    static auto tl_test = Widgets::TransducerLoader();
    tl_test.render();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}
