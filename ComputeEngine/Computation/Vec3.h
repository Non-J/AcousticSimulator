#pragma once

#include <cstddef>
#include <nlohmann/json.hpp>
#include <tuple>

template <typename T>
struct Vec3 {
  T x, y, z;

  // region Construction and conversion

  Vec3() = default;
  Vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z){};
  Vec3(const std::tuple<T, T, T>& v) : x(get<0>(v)), y(get<1>(v)), z(get<2>(v)) {}
  Vec3(const nlohmann::json& json)
      : x(json.at(0).get<T>()), y(json.at(1).get<T>()), z(json.at(2).get<T>()){};

  // Tie to std::tuple
  [[nodiscard]] std::tuple<T, T, T> tie() const { return std::tie(x, y, z); }

  // Convert to Vec3 of another type
  template <typename V>
  [[nodiscard]] Vec3<V> cast() const {
    return Vec3<V>{V(x), V(y), V(z)};
  }
  // Convert to json
  [[nodiscard]] nlohmann::json to_json() const {
    auto result = nlohmann::json();
    result[0] = this->x;
    result[1] = this->y;
    result[2] = this->z;
    return result;
  }

  // endregion
  // region Self Operation

  // Return product of each component
  [[nodiscard]] T product() const { return x * y * z; }
  // Return Vec3 with each component being positive
  [[nodiscard]] Vec3 elem_abs() const {
    return Vec3<double>{std::abs(this->x), std::abs(this->y), std::abs(this->z)};
  };
  // Return squared euclidean norm of vector (squared vector length)
  [[nodiscard]] T euclidean_norm_squared() const {
    return this->x * this->x + this->y * this->y + this->z * this->z;
  }
  // Return euclidean norm of vector (vector length)
  [[nodiscard]] T euclidean_norm() const {
    return T(sqrt(this->euclidean_norm_squared()));
  }

  // endregion
  // region Equality and relations

  bool operator==(const Vec3& rhs) const {
    return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
  }
  bool operator!=(const Vec3& rhs) const { return !(rhs == *this); }

  // endregion
  // region Scalar operations

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

  // endregion
  // region Vector operations

  Vec3 operator+(const Vec3& rhs) const {
    return Vec3<T>{this->x + rhs.x, this->y + rhs.y, this->z + rhs.z};
  }
  Vec3 operator-(const Vec3& rhs) const {
    return Vec3<T>{this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
  }

  // Return element-wise product with another vector (Hadamard/Schur product)
  [[nodiscard]] Vec3 elem_product(const Vec3& rhs) const {
    return Vec3<T>{this->x * rhs.x, this->y * rhs.y, this->z * rhs.z};
  }
  // Return element-wise division with another vector
  [[nodiscard]] Vec3 elem_division(const Vec3& rhs) const {
    return Vec3<T>{this->x / rhs.x, this->y / rhs.y, this->z / rhs.z};
  }

  // Return dot product with another vector
  [[nodiscard]] T dot_product(const Vec3& rhs) const {
    return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
  }
  // Return cross product with another vector
  [[nodiscard]] Vec3 cross_product(const Vec3& rhs) const {
    return Vec3<T>{
        this->y * rhs.z - this->z * rhs.y,
        this->z * rhs.x - this->x * rhs.z,
        this->x * rhs.y - this->y * rhs.x,
    };
  }
  // Return euclidean distance from this vector to another vector
  [[nodiscard]] T euclidean_distance(const Vec3& rhs) const {
    return (*this - rhs).euclidean_norm();
  }
  // Return cosine similarity with another vector
  [[nodiscard]] T cosine_similarity(const Vec3& rhs) const {
    return this->dot_product(rhs) / (this->euclidean_norm() * rhs.euclidean_norm());
  }
  // Return cosine angle formed by vector a, b; where this vector is the origin point
  [[nodiscard]] T cosine_angle(const Vec3& a, const Vec3& b) const {
    return T(acos((a - *this).cosine_similarity(b - *this)));
  }

  // endregion
};

// Specialization of abs for std::size_t as std::size_t is already unsigned
template <>
Vec3<std::size_t> Vec3<std::size_t>::elem_abs() const {
  return Vec3<std::size_t>{this->x, this->y, this->z};
}

// Specialization of abs for double
template <>
Vec3<double> Vec3<double>::elem_abs() const {
  return Vec3<double>{std::fabs(this->x), std::fabs(this->y), std::fabs(this->z)};
}

// Force compiler to generate only one implementation in Vec3.cpp
extern template struct Vec3<std::size_t>;
extern template struct Vec3<double>;
