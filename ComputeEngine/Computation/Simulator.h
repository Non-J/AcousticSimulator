#pragma once

#include <atomic>
#include <filesystem>
#include <mutex>
#include <string>
#include "../Utilities/AtomicLogger.h"
#include "Config.h"
#include "Vec3.h"

namespace Computation {

void simulationProcess(std::atomic<bool>* process_lock_simulation_running,
                       AtomicLogger::AtomicLogger* result_log,
                       std::filesystem::path export_directory,
                       std::vector<Config::Transducer> transducers,
                       Config::SimulationParameter simulation_parameter);

}  // namespace Computation
