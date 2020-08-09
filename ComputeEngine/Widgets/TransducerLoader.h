#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "../Transducer.h"

namespace Widgets {

class TransducerLoader {
  bool is_successfully_loaded;
  std::vector<std::unique_ptr<Transducer::ITransducer>> transducers;
  std::string user_input;
  std::string parsing_result;

  void parse_json();

 public:
  void render();

  [[nodiscard]] auto getTransducers() const;
};

}  // namespace Widgets