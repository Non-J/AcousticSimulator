#ifndef ACOUSTICSIMULATOR_SIMULATOR_H
#define ACOUSTICSIMULATOR_SIMULATOR_H

#include <atomic>
#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

namespace Computation {

// region Math section

template <typename T>
struct Vec3 {
  T x, y, z;

  bool operator==(const Vec3& rhs) const {
    return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
  }
  bool operator!=(const Vec3& rhs) const { return !(rhs == *this); }
};

template <typename T>
constexpr std::tuple<T, T, T> map_id_to_index(T id, T x_size, T y_size, T z_size) {
  const auto x_idx = (id / z_size / y_size) % x_size;
  const auto y_idx = (id / z_size) % y_size;
  const auto z_idx = id % z_size;
  return std::tie(x_idx, y_idx, z_idx);
}

template <typename T>
class CellBlock {
  // 3-dimensional contiguous memory
  std::size_t x_size, y_size, z_size;
  T* data;

 public:
  CellBlock(std::size_t x_size, std::size_t y_size, std::size_t z_size)
      : x_size(x_size), y_size(y_size), z_size(z_size) {
    data = new T[x_size * y_size * z_size];
  }

  ~CellBlock() { delete[] data; }

  T get_cell_by_id(std::size_t id) const;
  void set_cell_by_id(std::size_t id, T value);

  [[nodiscard]] std::size_t size() const { return x_size * y_size * z_size; }

  [[nodiscard]] char* get_raw_bytes() const { return reinterpret_cast<char*>(data); }
};

template <typename T>
class RealSpaceInterpolation {
  // Map integer id to some vector in 3-dimensional space
  std::size_t x_size, y_size, z_size;
  Vec3<T> begin;
  Vec3<T> end;

 public:
  RealSpaceInterpolation(size_t x_size,
                         size_t y_size,
                         size_t z_size,
                         Vec3<T> begin,
                         Vec3<T> end)
      : x_size(x_size), y_size(y_size), z_size(z_size), begin(begin), end(end) {}

  Vec3<T> get_vec3_by_id(std::size_t id) const;
};

// endregion

// region Computer section

struct Transducer {
  std::string id;

  Vec3<double> position;
  Vec3<double> target;

  double radius, phase_shift, loss_factor, output_power;

  [[nodiscard]] std::string checkInvalidParameter() const;
};

struct SimulationParameter {
  Vec3<double> begin;
  Vec3<double> end;

  double cell_size, frequency, particle_radius, air_density, wave_speed;

  [[nodiscard]] std::string checkInvalidParameter() const;
};

void simulationProcess(std::atomic<bool>* process_lock_simulation_running,
                       std::string export_name,
                       std::vector<Computation::Transducer> transducers,
                       Computation::SimulationParameter simulation_parameter);

// endregion

}  // namespace Computation

#endif  // ACOUSTICSIMULATOR_SIMULATOR_H
