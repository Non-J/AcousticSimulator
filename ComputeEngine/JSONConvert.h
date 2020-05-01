#pragma once

#include <nlohmann/json.hpp>
#include "Computation/Simulator.h"
#include "Computation/Vec3.h"

namespace JSONConvert {

/* For some reason, template function like these two don't work
 * when in .cpp file (they don't get linked). Should probably look
 * into this at some point, but for now, do not move these functions.
 */
template <typename T>
[[nodiscard]] Vec3<T> to_vec3(const nlohmann::json& json) {
  return Vec3<T>{json.at(0).get<T>(), json.at(1).get<T>(), json.at(2).get<T>()};
};
template <typename T>
[[nodiscard]] nlohmann::json from_vec3(const Vec3<T>& vec3) {
  auto result = nlohmann::json();
  result[0] = vec3.x;
  result[1] = vec3.y;
  result[2] = vec3.z;
  return result;
};

[[nodiscard]] Computation::Transducer to_transducer(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_transducer(const Computation::Transducer& transducer);

[[nodiscard]] Computation::SimulationParameter to_simulation_parameter(
    const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_simulation_parameter(
    const Computation::SimulationParameter& simulation_parameter);

}  // namespace JSONConvert