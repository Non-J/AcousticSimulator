#include <imgui.h>
#include "../DataStore.h"
#include "UserInterface.h"

void UserInterface::ToolsMenu(DataStore::GlobalDataStore& global_data_store) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Tools")) {
      ImGui::Checkbox("Transducer Configuration",
                      &global_data_store.toolbox_open.TransducerConfiguration);
      ImGui::Checkbox("ImGui Demo", &global_data_store.toolbox_open.ImGuiDemo);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}