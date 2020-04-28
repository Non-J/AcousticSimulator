#include <fmt/format.h>
#include <gsl/gsl_sf_bessel.h>
#include <imgui.h>
#include <chrono>
#include <string_view>
#include <thread>
#include "UserInterface.h"

void executeBenchmark(std::string* logs, bool* running) {
  const auto begin = std::chrono::steady_clock::now();
  auto begin_lap = begin;

  const auto log = [&](std::string_view message) {
    *logs += fmt::format(FMT_STRING("[{:d}/{:d} ms] {:s}\n"),
                         std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - begin_lap)
                             .count(),
                         std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - begin)
                             .count(),
                         message);
    begin_lap = std::chrono::steady_clock::now();
  };

  log("Started");

  // prevent compiler optimization
  volatile double result = 0;
  for (auto i = 0; i < 10000000; ++i) {
    result += gsl_sf_bessel_J0(6.0);
    if (result > 1000000) {
      result = 0;
    }
  }
  log("10M sequential");

  result = 0;
#pragma omp parallel for
  for (auto i = 0; i < 50000000; ++i) {
    result += gsl_sf_bessel_J0(6.0);
    if (result > 1000000) {
      result = 0;
    }
  }
  log("50M openmp parallel for");

  log("Done");
  *running = false;
}

void UserInterface::BenchmarkingWidget(DataStore::GlobalDataStore& global_data_store) {
  auto window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Benchmarking", &global_data_store.toolbox_open.BenchmarkingWidget,
               window_flags);

  static auto version_information =
      fmt::format(FMT_STRING("Version: {:s} {:s}"), __DATE__, __TIME__);
  static auto benchmark_running = false;
  static auto benchmark_result = std::string();

  ImGui::TextUnformatted(version_information.c_str());

  if (ImGui::Button("Run", ImVec2(250, 20)) and not benchmark_running) {
    benchmark_running = true;
    std::thread thread(executeBenchmark, &benchmark_result, &benchmark_running);
    thread.detach();
  }

  ImGui::PushTextWrapPos(250);
  ImGui::TextUnformatted(benchmark_result.c_str());
  ImGui::PopTextWrapPos();

  ImGui::End();
}