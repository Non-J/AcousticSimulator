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
  result.air_density = json.at("air_density").get<double>();
  result.air_wave_speed = json.at("air_wave_speed").get<double>();
  result.particle_radius = json.at("particle_radius").get<double>();
  result.particle_density = json.at("particle_density").get<double>();
  result.particle_wave_speed = json.at("particle_wave_speed").get<double>();
  return result;
}
nlohmann::json from_simulation_parameter(
    const Computation::SimulationParameter& simulation_parameter) {
  auto result = nlohmann::json();
  result["begin"] = simulation_parameter.begin.to_json();
  result["end"] = simulation_parameter.end.to_json();
  result["cell_size"] = simulation_parameter.cell_size;
  result["frequency"] = simulation_parameter.frequency;
  result["air_density"] = simulation_parameter.air_density;
  result["air_wave_speed"] = simulation_parameter.air_wave_speed;
  result["particle_radius"] = simulation_parameter.particle_radius;
  result["particle_density"] = simulation_parameter.particle_density;
  result["particle_wave_speed"] = simulation_parameter.particle_wave_speed;
  return result;
}

}  // namespace JSONConvert