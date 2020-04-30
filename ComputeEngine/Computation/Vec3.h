#pragma once

#include <cmath>
#include <cstddef>
#include <tuple>

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
