#include "Transducer.h"
#include <numbers>
#include <utility>

constexpr auto i = std::complex<double>{0, 1};

std::complex<double> Transducer::StaticTransducer::get_pressure(
    const Vec3<double>& point,
    const Transducer::DynamicParameter& parameter) const {
  const auto angle = this->position.cosine_angle(this->target, point);
  const auto dist = this->position.euclidean_distance(point);
  const auto wave_number =
      2.0 * std::numbers::pi * this->frequency / parameter.wave_speed;

  const auto directivity = [&]() -> double {
    const auto intermediate = wave_number * this->radius * std::sin(angle);
    if (intermediate == 0.0) {
      return 1.0;
    }
    return 2.0 * std::cyl_bessel_j(1, intermediate) / intermediate;
  }();

  return std::exp(i * (wave_number * dist + this->phase_shift)) * this->output_power *
         directivity / dist;
}

Transducer::StaticTransducer::StaticTransducer(std::string Id,
                                               Vec3<double> Position,
                                               Vec3<double> Target,
                                               double Radius,
                                               double Frequency,
                                               double PhaseShift,
                                               double OutputPower)
    : id(std::move(Id)),
      position(std::move(Position)),
      target(std::move(Target)),
      radius(Radius),
      frequency(Frequency),
      phase_shift(PhaseShift),
      output_power(OutputPower) {}

Transducer::StaticTransducer::~StaticTransducer() = default;
