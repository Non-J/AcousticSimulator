#include "SimulationParameter.h"
#include <numbers>
#include <utility>

StaticSimulationParameter StaticSimulationParameter::withKnownParticleDensity(
    Vec3<double>&& begin,
    Vec3<double>&& end,
    double CellSize,
    double MediumWaveSpeed,
    double MediumDensity,
    double ParticleWaveSpeed,
    double ParticleDensity,
    double ParticleRadius,
    double WaveAngularFrequency) {
  const auto ParticleVolume =
      (4.0 / 3.0) * std::numbers::pi * ParticleRadius * ParticleRadius * ParticleRadius;

  // Intermediate values for calculation
  const auto i_pa_m = 1.0 / (MediumDensity * MediumWaveSpeed * MediumWaveSpeed);
  const auto i_pa_p = 1.0 / (ParticleDensity * ParticleWaveSpeed * ParticleWaveSpeed);

  const auto AcousticPotentialK1 = ParticleVolume * (i_pa_m - i_pa_p) / 4.0;

  // Intermediate values for calculation
  const auto i_a = (MediumDensity - ParticleDensity) /
                   (WaveAngularFrequency * WaveAngularFrequency * MediumDensity *
                    (MediumDensity + 2.0 * ParticleDensity));

  const auto AcousticPotentialK2 = ParticleVolume * i_a * 3.0 / 4.0;

  return StaticSimulationParameter(std::move(begin), std::move(end), CellSize,
                                   MediumWaveSpeed, ParticleVolume, AcousticPotentialK1,
                                   AcousticPotentialK2);
}

StaticSimulationParameter StaticSimulationParameter::withLargeParticleDensityAssumption(
    Vec3<double>&& begin,
    Vec3<double>&& end,
    double CellSize,
    double MediumWaveSpeed,
    double MediumDensity,
    double ParticleRadius,
    double WaveAngularFrequency) {
  const auto ParticleVolume =
      (4.0 / 3.0) * std::numbers::pi * ParticleRadius * ParticleRadius * ParticleRadius;

  // Intermediate values for calculation
  const auto i_pa_m = 1.0 / (MediumDensity * MediumWaveSpeed * MediumWaveSpeed);

  const auto AcousticPotentialK1 = ParticleVolume * i_pa_m / 4.0;

  // Intermediate values for calculation
  const auto i_a =
      -1.0 / (2.0 * MediumDensity * WaveAngularFrequency * WaveAngularFrequency);

  const auto AcousticPotentialK2 = ParticleVolume * i_a * 3.0 / 4.0;

  return StaticSimulationParameter(std::move(begin), std::move(end), CellSize,
                                   MediumWaveSpeed, ParticleVolume, AcousticPotentialK1,
                                   AcousticPotentialK2);
}

const Vec3<double>& StaticSimulationParameter::getBegin() const {
  return begin;
}

const Vec3<double>& StaticSimulationParameter::getEnd() const {
  return end;
}

double StaticSimulationParameter::getCellSize() const {
  return cell_size;
}

double StaticSimulationParameter::getMediumWaveSpeed() const {
  return medium_wave_speed;
}

double StaticSimulationParameter::getParticleVolume() const {
  return particle_volume;
}

double StaticSimulationParameter::getAcousticPotentialK1() const {
  return acoustic_potential_k1;
}

double StaticSimulationParameter::getAcousticPotentialK2() const {
  return acoustic_potential_k2;
}

void StaticSimulationParameter::setBegin(const Vec3<double>& Begin) {
  begin = Begin;
}
void StaticSimulationParameter::setEnd(const Vec3<double>& AnEnd) {
  end = AnEnd;
}

StaticSimulationParameter::StaticSimulationParameter(Vec3<double> Begin,
                                                     Vec3<double> End,
                                                     double CellSize,
                                                     double MediumWaveSpeed,
                                                     double ParticleVolume,
                                                     double AcousticPotentialK1,
                                                     double AcousticPotentialK2)
    : begin(std::move(Begin)),
      end(std::move(End)),
      cell_size(CellSize),
      medium_wave_speed(MediumWaveSpeed),
      particle_volume(ParticleVolume),
      acoustic_potential_k1(AcousticPotentialK1),
      acoustic_potential_k2(AcousticPotentialK2) {}
