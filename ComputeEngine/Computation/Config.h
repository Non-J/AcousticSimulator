#pragma once

#include <nlohmann/json.hpp>
#include <numbers>
#include <string>
#include "Vec3.h"

namespace Config {

struct Transducer {
  std::string id;

  Vec3<double> position;
  Vec3<double> target;

  double radius, phase_shift, loss_factor, output_power;

  [[nodiscard]] std::string checkInvalidParameter() const {
    if (this->radius <= 0) {
      return "Radius is not positive";
    }
    if (this->loss_factor < 0 || this->loss_factor > 1) {
      return "Loss factor is not in range 0 and 1";
    }

    return std::string();
  };
};

struct SimulationParameter {
  Vec3<double> begin;
  Vec3<double> end;

  double cell_size, frequency, air_density, air_wave_speed, particle_radius,
      particle_density, particle_wave_speed;

  bool assume_large_particle_density = true;

  [[nodiscard]] std::string checkInvalidParameter() const {
    if (this->cell_size <= 0) {
      return "Cell size is not positive";
    }
    if (this->frequency <= 0) {
      return "Frequency is not positive";
    }
    if (this->air_density <= 0) {
      return "Air density is not positive";
    }
    if (this->air_wave_speed <= 0) {
      return "Air wave speed is not positive";
    }
    if (this->particle_radius <= 0) {
      return "Particle radius is not positive";
    }
    if (not assume_large_particle_density and this->particle_density <= 0) {
      return "Particle density is not positive";
    }
    if (not assume_large_particle_density and this->particle_wave_speed <= 0) {
      return "Particle wave speed is not positive";
    }

    return std::string();
  }

  [[nodiscard]] constexpr double particle_volume() const {
    return (4.0 / 3.0) * std::numbers::pi * this->particle_radius *
           this->particle_radius * this->particle_radius;
  }

  [[nodiscard]] constexpr double angular_frequency() const {
    return (4.0 / 3.0) * std::numbers::pi * this->particle_radius *
           this->particle_radius * this->particle_radius;
  }

  [[nodiscard]] constexpr double constant_k1() const {
    const auto particle_volume = this->particle_volume();

    const auto i1 = this->air_wave_speed * this->air_wave_speed * this->air_density;
    const auto i2 =
        this->particle_wave_speed * this->particle_wave_speed * this->particle_density;

    if (this->assume_large_particle_density) {
      return particle_volume / i1 / 4.0;
    }
    return particle_volume * (1.0 / i1 - 1.0 / i2) / 4.0;
  }

  [[nodiscard]] constexpr double constant_k2() const {
    const auto i1 = this->particle_volume() * 3.0 / 4.0;
    const auto w = this->angular_frequency();
    const auto i2 = w * w * this->air_density;
    const auto i3 = this->air_density + 2.0 * this->particle_density;
    const auto i4 = this->air_density - this->particle_density;

    if (this->assume_large_particle_density) {
      return i1 / i2 / -2.0;
    }
    return i1 * (i4 / i3 / i2);
  }
};

}  // namespace Config

namespace JSONConvert {

[[nodiscard]] Config::Transducer to_transducer(const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_transducer(const Config::Transducer& transducer);

[[nodiscard]] Config::SimulationParameter to_simulation_parameter(
    const nlohmann::json& json);
[[nodiscard]] nlohmann::json from_simulation_parameter(
    const Config::SimulationParameter& simulation_parameter);

}  // namespace JSONConvert