#include "AtomicLogger.h"

#include <fmt/format.h>

namespace AtomicLogger {

AtomicLogger::AtomicLogger() {
  this->construction_time = std::chrono::steady_clock::now();
  this->last_log_time = this->construction_time;
}

void AtomicLogger::push(std::string_view message) {
  const auto scoped_lock = std::scoped_lock(this->log_lock);
  this->log_string.append(message.begin(), message.end());
}

void AtomicLogger::log(std::string_view message) {
  const auto scoped_lock = std::scoped_lock(this->log_lock);

  const auto prefix =
      fmt::format(FMT_STRING("[{:d}/{:d} s] "),
                  std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::steady_clock::now() - this->construction_time)
                      .count(),
                  std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::steady_clock::now() - this->last_log_time)
                      .count());

  this->log_string.append(prefix);
  this->log_string.append(message);
  this->log_string.push_back('\n');

  this->last_log_time = std::chrono::steady_clock::now();
}

void AtomicLogger::clear() {
  const auto scoped_lock = std::scoped_lock(this->log_lock);
  this->log_string.clear();
}

std::pair<std::unique_lock<std::mutex>, std::string_view> AtomicLogger::read() {
  return std::pair(std::unique_lock(this->log_lock),
                   std::string_view(this->log_string));
}

}  // namespace AtomicLogger
