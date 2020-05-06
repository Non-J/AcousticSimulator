#pragma once

#include <chrono>
#include <mutex>
#include <string>

namespace AtomicLogger {

class AtomicLogger {
  std::chrono::steady_clock::time_point construction_time;
  std::chrono::steady_clock::time_point last_log_time;
  std::mutex log_lock;
  std::string log_string;

 public:
  AtomicLogger();

  void push(std::string_view message);
  void log(std::string_view message);
  void clear();

  [[nodiscard]] std::pair<std::unique_lock<std::mutex>, std::string_view> read();
};

}  // namespace AtomicLogger