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

  // Construction and conversion
  Vec3() = default;
  Vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z){};
  Vec3(const std::tuple<T, T, T>& v) : x(get<0>(v)), y(get<1>(v)), z(get<2>(v)) {}

  [[nodiscard]] std::tuple<T, T, T> tie() const { return std::tie(x, y, z); }
  template <typename V>
  [[nodiscard]] Vec3<V> cast() const {
    return Vec3<V>{V(x), V(y), V(z)};
  }

  // Equality and relations
  bool operator==(const Vec3& rhs) const {
    return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
  }
  bool operator!=(const Vec3& rhs) const { return !(rhs == *this); }

  // Vector operations
  Vec3 operator+(const Vec3& rhs) const {
    return Vec3<T>{this->x + rhs.x, this->y + rhs.y, this->z + rhs.z};
  }
  Vec3 operator-(const Vec3& rhs) const {
    return Vec3<T>{this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
  }
  // Element-wise operations
  [[nodiscard]] Vec3 elem_product(const Vec3& rhs) const {
    return Vec3<T>{this->x * rhs.x, this->y * rhs.y, this->z * rhs.z};
  }
  [[nodiscard]] Vec3 elem_division(const Vec3& rhs) const {
    return Vec3<T>{this->x / rhs.x, this->y / rhs.y, this->z / rhs.z};
  }
  // Other vector operations
  [[nodiscard]] T dot_product(const Vec3& rhs) const {
    return this->x * rhs.x + this->y * rhs.y + this.z * rhs.z;
  }
  [[nodiscard]] Vec3 cross_product(const Vec3& rhs) const {
    return Vec3<T>{
        this->y * rhs.z - this->z * rhs.y,
        this->z * rhs.x - this->x * rhs.z,
        this->x * rhs.y - this->y * rhs.x,
    };
  }
  [[nodiscard]] T euclidean_norm_squared() const {
    return this->x * this->x + this->y * this->y + this->z * this->z;
  }
  [[nodiscard]] T euclidean_norm() const { sqrt(this->euclidean_norm()); }
  [[nodiscard]] T euclidean_distance(const Vec3& rhs) const {
    return (*this - rhs).euclidean_norm();
  }
  [[nodiscard]] T cosine_similarity(const Vec3& rhs) const {
    return this->dot_product(rhs) / (this->euclidean_norm() * rhs.euclidean_norm());
  }
  [[nodiscard]] T cosine_angle(const Vec3& a, const Vec3& b) const {
    // Return angle form by vector a, b; where the origin point is this vector
    return acos((a - *this).cosine_similarity(b - *this));
  }

  // Scalar operations
  Vec3 operator+(const T& rhs) const {
    return Vec3<T>{this->x + rhs, this->y + rhs, this->z + rhs};
  }
  Vec3 operator-(const T& rhs) const {
    return Vec3<T>{this->x - rhs, this->y - rhs, this->z - rhs};
  }
  Vec3 operator*(const T& rhs) const {
    return Vec3<T>{this->x * rhs, this->y * rhs, this->z * rhs};
  }
  Vec3 operator/(const T& rhs) const {
    return Vec3<T>{this->x / rhs, this->y / rhs, this->z / rhs};
  }

  // Other operations
  [[nodiscard]] Vec3 elem_abs() const {
    return Vec3<T>{abs(this->x), abs(this->y), abs(this->z)};
  }
  [[nodiscard]] T product() const { return x * y * z; }
};

template <typename T>
class CellBlock {
  // 3-dimensional contiguous memory
  Vec3<std::size_t> dimension_size;
  T* data;

 public:
  CellBlock(Vec3<std::size_t> dimension_size) : dimension_size(dimension_size) {
    data = new T[dimension_size.product()];
  }

  ~CellBlock() { delete[] data; }

  T get_cell_by_id(std::size_t id) const;
  void set_cell_by_id(std::size_t id, T value);

  [[nodiscard]] std::size_t size() const { return dimension_size.product(); }

  [[nodiscard]] char* get_raw_bytes() const { return reinterpret_cast<char*>(data); }
};

template <typename T>
class RealSpaceInterpolation {
  // Map integer id to some vector in 3-dimensional space
  Vec3<std::size_t> dimension_size;
  Vec3<T> begin;
  Vec3<T> end;

 public:
  RealSpaceInterpolation(Vec3<std::size_t> dimension_size, Vec3<T> begin, Vec3<T> end)
      : dimension_size(dimension_size), begin(begin), end(end) {}

  [[nodiscard]] Vec3<T> get_real_vec(std::size_t id) const;
  [[nodiscard]] Vec3<std::size_t> get_int_vec(std::size_t id) const;
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
