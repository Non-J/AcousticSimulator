#pragma once

#include "Vec3.h"

/**
 * Static simulation parameter couldn't change while simulation is in progress
 */
class StaticSimulationParameter {
  Vec3<double> begin;
  Vec3<double> end;

  const double cell_size, medium_wave_speed, particle_volume, acoustic_potential_k1,
      acoustic_potential_k2;

 protected:
  /**
   * Internal constructor. Use public named constructor instead!
   */
  StaticSimulationParameter(Vec3<double> Begin,
                            Vec3<double> End,
                            double CellSize,
                            double MediumWaveSpeed,
                            double ParticleVolume,
                            double AcousticPotentialK1,
                            double AcousticPotentialK2);

 public:
  /**
   * Create StaticSimulationParameter where particle density and wave speed is known
   */
  static StaticSimulationParameter withKnownParticleDensity(
      Vec3<double>&& begin,
      Vec3<double>&& end,
      double CellSize,
      double MediumWaveSpeed,
      double MediumDensity,
      double ParticleWaveSpeed,
      double ParticleDensity,
      double ParticleRadius,
      double WaveAngularFrequency);

  /**
   * Create StaticSimulationParameter where particle density and wave speed is unknown.
   * The values are approximated as if the density and wave speed approach infinity
   */
  static StaticSimulationParameter withLargeParticleDensityAssumption(
      Vec3<double>&& begin,
      Vec3<double>&& end,
      double CellSize,
      double MediumWaveSpeed,
      double MediumDensity,
      double ParticleRadius,
      double WaveAngularFrequency);

  [[nodiscard]] const Vec3<double>& getBegin() const;
  [[nodiscard]] const Vec3<double>& getEnd() const;
  [[nodiscard]] double getCellSize() const;
  [[nodiscard]] double getMediumWaveSpeed() const;
  [[nodiscard]] double getParticleVolume() const;
  [[nodiscard]] double getAcousticPotentialK1() const;
  [[nodiscard]] double getAcousticPotentialK2() const;

  void setBegin(const Vec3<double>& Begin);
  void setEnd(const Vec3<double>& AnEnd);
};