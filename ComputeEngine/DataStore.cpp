#include "DataStore.h"

namespace DataStore::JSONConvert {

Simulator::Vec3 to_vec3(const nlohmann::json& json) {
  return Simulator::Vec3{json.at(0).get<double>(), json.at(1).get<double>(),
                         json.at(2).get<double>()};
}
nlohmann::json from_vec3(const Simulator::Vec3& vec3) {
  auto result = nlohmann::json();
  result[0] = vec3.x;
  result[1] = vec3.y;
  result[2] = vec3.z;
  return result;
}

Simulator::Transducer to_transducer(const nlohmann::json& json) {
  auto result = Simulator::Transducer();
  result.id = json.at("id").get<std::string>();
  result.position = to_vec3(json.at("position"));
  result.target = to_vec3(json.at("target"));
  result.radius = json.at("radius").get<double>();
  result.phase_shift = json.at("phase_shift").get<double>();
  result.loss_factor = json.at("loss_factor").get<double>();
  result.output_power = json.at("output_power").get<double>();
  return result;
}
nlohmann::json from_transducer(const Simulator::Transducer& transducer) {
  auto result = nlohmann::json();
  result["id"] = transducer.id;
  result["position"] = from_vec3(transducer.position);
  result["target"] = from_vec3(transducer.target);
  result["radius"] = transducer.radius;
  result["phase_shift"] = transducer.phase_shift;
  result["loss_factor"] = transducer.loss_factor;
  result["output_power"] = transducer.output_power;
  return result;
}
Simulator::SimulationParameter to_simulation_parameter(const nlohmann::json& json) {
  auto result = Simulator::SimulationParameter();
  result.begin = to_vec3(json.at("begin"));
  result.end = to_vec3(json.at("end"));
  result.cell_size = json.at("cell_size").get<double>();
  result.frequency = json.at("frequency").get<double>();
  result.particle_radius = json.at("particle_radius").get<double>();
  result.air_density = json.at("air_density").get<double>();
  result.wave_speed = json.at("wave_speed").get<double>();
  return result;
}
nlohmann::json from_simulation_parameter(
    const Simulator::SimulationParameter& simulation_parameter) {
  auto result = nlohmann::json();
  result["begin"] = from_vec3(simulation_parameter.begin);
  result["end"] = from_vec3(simulation_parameter.end);
  result["cell_size"] = simulation_parameter.cell_size;
  result["frequency"] = simulation_parameter.frequency;
  result["particle_radius"] = simulation_parameter.particle_radius;
  result["air_density"] = simulation_parameter.air_density;
  result["wave_speed"] = simulation_parameter.wave_speed;
  return result;
}

}  // namespace DataStore::JSONConvert