#pragma once

#include <complex>
#include <string>
#include "Vec3.h"

namespace Transducer {

/**
 * Dynamic simulation parameter. Could change while simulation is in progress
 */
struct DynamicParameter {
  double wave_speed;
};

/**
 * Transducer interface
 */
class ITransducer {
 public:
  virtual ~ITransducer() = default;

  /**
   * Public name or id
   */
  std::string id;

  /**
   * Compute pressure given by the transducer at a certain point
   * @param point point at which to compute pressure
   * @param parameter dynamic simulation parameter
   * @return complex pressure amplitude
   */
  [[nodiscard]] virtual std::complex<double> get_pressure(
      const Vec3<double>& point,
      const DynamicParameter& parameter) const = 0;
};

/**
 * Static transducer is a transducer where its properties doesn't change with time
 */
class StaticTransducer : public ITransducer {
  std::string id;

  Vec3<double> position;
  Vec3<double> target;

  double radius, frequency, phase_shift, output_power;

 public:
  StaticTransducer(std::string Id,
                   Vec3<double> Position,
                   Vec3<double> Target,
                   double Radius,
                   double Frequency,
                   double PhaseShift,
                   double OutputPower);

  [[nodiscard]] std::complex<double> get_pressure(
      const Vec3<double>& point,
      const DynamicParameter& parameter) const override;

  ~StaticTransducer() override;
};

}  // namespace Transducer
