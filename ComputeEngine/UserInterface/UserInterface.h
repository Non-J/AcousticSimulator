#ifndef ACOUSTICSIMULATOR_COMPUTEENGINE_USERINTERFACE_H
#define ACOUSTICSIMULATOR_COMPUTEENGINE_USERINTERFACE_H

#include "../DataStore.h"

namespace UserInterface {

void SetupStyle();
void ToolsMenu(DataStore::GlobalDataStore& global_data_store);
void TransducerConfigurationWidget(DataStore::GlobalDataStore& global_data_store);

}  // namespace UserInterface

#endif  // ACOUSTICSIMULATOR_COMPUTEENGINE_USERINTERFACE_H
