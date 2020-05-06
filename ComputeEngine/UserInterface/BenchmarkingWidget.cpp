#include <fmt/format.h>
#include <imgui.h>
#include <cmath>
#include <mutex>
#include <thread>
#include "../Utilities/AtomicLogger.h"
#include "UserInterface.h"

void executeBenchmark(AtomicLogger::AtomicLogger* result, bool* running) {
  result->log("Started");

  // prevent compiler optimization
  volatile double r = 0;
  for (auto i = 0; i < 25000000; ++i) {
    r = r + std::cyl_bessel_j(1.0, 6.0);
  }
  result->log("25M sequential gsl_sf_bessel_J0");

  auto omp_result_lock = std::mutex();

  r = 0;
#pragma omp parallel for
  for (auto i = 0; i < 25000000; ++i) {
    const auto v = std::cyl_bessel_j(1.0, 6.0);
    {
      const auto scoped_lock = std::scoped_lock<std::mutex>(omp_result_lock);
      r = r + v;
    }
  }
  result->log("25M openmp parallel for gsl_sf_bessel_J0");

  result->log("Done");
  *running = false;
}

void UserInterface::BenchmarkingWidget(DataStore::GlobalDataStore& global_data_store) {
  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Benchmarking", &global_data_store.toolbox_open.BenchmarkingWidget,
               window_flags);

  static auto version_information =
      fmt::format(FMT_STRING("Version: {:s} {:s}"), __DATE__, __TIME__);
  static auto benchmark_running = false;
  static auto benchmark_logging = AtomicLogger::AtomicLogger();

  ImGui::TextUnformatted(version_information.c_str());

  if (ImGui::Button("Run", ImVec2(400, 20)) and not benchmark_running) {
    benchmark_running = true;
    auto thread = std::thread(executeBenchmark, &benchmark_logging, &benchmark_running);
    thread.detach();
  }

  ImGui::PushTextWrapPos(400);
  {
    const auto result = benchmark_logging.read();
    ImGui::TextUnformatted(result.second.data());
  }
  ImGui::PopTextWrapPos();

  ImGui::End();
}