#include "DataStore.h"
#include "Computation/Vec3.h"

namespace JSONConvert {

Computation::Transducer to_transducer(const nlohmann::json& json) {
  auto result = Computation::Transducer();
  result.id = json.at("id").get<std::string>();
  result.position = Vec3<double>(json.at("position"));
  result.target = Vec3<double>(json.at("target"));
  result.radius = json.at("radius").get<double>();
  result.phase_shift = json.at("phase_shift").get<double>();
  result.loss_factor = json.at("loss_factor").get<double>();
  result.output_power = json.at("output_power").get<double>();
  return result;
}
nlohmann::json from_transducer(const Computation::Transducer& transducer) {
  auto result = nlohmann::json();
  result["id"] = transducer.id;
  result["position"] = transducer.position.to_json();
  result["target"] = transducer.target.to_json();
  result["radius"] = transducer.radius;
  result["phase_shift"] = transducer.phase_shift;
  result["loss_factor"] = transducer.loss_factor;
  result["output_power"] = transducer.output_power;
  return result;
}

Computation::SimulationParameter to_simulation_parameter(const nlohmann::json& json) {
  auto result = Computation::SimulationParameter();
  result.begin = Vec3<double>(json.at("begin"));
  result.end = Vec3<double>(json.at("end"));
  result.cell_size = json.at("cell_size").get<double>();
  result.frequency = json.at("frequency").get<double>();
  result.particle_radius = json.at("particle_radius").get<double>();
  result.air_density = json.at("air_density").get<double>();
  result.wave_speed = json.at("wave_speed").get<double>();
  return result;
}
nlohmann::json from_simulation_parameter(
    const Computation::SimulationParameter& simulation_parameter) {
  auto result = nlohmann::json();
  result["begin"] = simulation_parameter.begin.to_json();
  result["end"] = simulation_parameter.end.to_json();
  result["cell_size"] = simulation_parameter.cell_size;
  result["frequency"] = simulation_parameter.frequency;
  result["particle_radius"] = simulation_parameter.particle_radius;
  result["air_density"] = simulation_parameter.air_density;
  result["wave_speed"] = simulation_parameter.wave_speed;
  return result;
}

}  // namespace JSONConvert