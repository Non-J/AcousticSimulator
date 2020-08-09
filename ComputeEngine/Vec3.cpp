#include "Vec3.h"
#include <cmath>
#include <type_traits>
#include <utility>

/**
 * Solve linear equation (y=mx+c) for slope (m)
 * @tparam T numerical type
 * @param y eqn. solution
 * @param x eqn. parameter
 * @param c eqn. constant
 * @return eqn. slope
 */
template <typename T>
requires std::is_arithmetic_v<T> T solve_linear_slope(T y, T x, T c) {
  return (y - c) / x;
}

template <typename T>
Vec3<T>::Vec3(const T x, const T y, const T z)
    : x(std::move(x)), y(std::move(y)), z(std::move(z)) {}

template <typename T>
Vec3<T> Vec3<T>::fromValue(T x) {
  return Vec3<T>(x, x, x);
}

template <typename T>
T Vec3<T>::product() const {
  return x * y * z;
}

template <>
Vec3<std::size_t> Vec3<std::size_t>::elem_abs() const {
  return *this;
}

template <>
Vec3<double> Vec3<double>::elem_abs() const {
  return Vec3<double>(std::fabs(this->x), std::fabs(this->y), std::fabs(this->z));
}

template <typename T>
T Vec3<T>::euclidean_norm_squared() const {
  return this->x * this->x + this->y * this->y + this->z * this->z;
}

template <typename T>
T Vec3<T>::euclidean_norm() const {
  return T(std::sqrt(this->euclidean_norm_squared()));
}

template <>
bool Vec3<std::size_t>::operator==(const Vec3& rhs) const {
  return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
}

template <>
bool Vec3<std::size_t>::operator!=(const Vec3& rhs) const {
  return !(rhs == *this);
}

template <typename T>
Vec3<T> Vec3<T>::operator+(const T& rhs) const {
  return Vec3<T>{this->x + rhs, this->y + rhs, this->z + rhs};
}

template <typename T>
Vec3<T> Vec3<T>::operator-(const T& rhs) const {
  return Vec3<T>{this->x - rhs, this->y - rhs, this->z - rhs};
}

template <typename T>
Vec3<T> Vec3<T>::operator*(const T& rhs) const {
  return Vec3<T>{this->x * rhs, this->y * rhs, this->z * rhs};
}

template <typename T>
Vec3<T> Vec3<T>::operator/(const T& rhs) const {
  return Vec3<T>{this->x / rhs, this->y / rhs, this->z / rhs};
}

template <typename T>
Vec3<T> Vec3<T>::operator+(const Vec3<T>& rhs) const {
  return Vec3<T>{this->x + rhs.x, this->y + rhs.y, this->z + rhs.z};
}

template <typename T>
Vec3<T> Vec3<T>::operator-(const Vec3<T>& rhs) const {
  return Vec3<T>{this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
}

template <typename T>
Vec3<T> Vec3<T>::elem_product(const Vec3& rhs) const {
  return Vec3<T>{this->x * rhs.x, this->y * rhs.y, this->z * rhs.z};
}

template <typename T>
Vec3<T> Vec3<T>::elem_division(const Vec3& rhs) const {
  return Vec3<T>{this->x / rhs.x, this->y / rhs.y, this->z / rhs.z};
}

template <typename T>
T Vec3<T>::dot_product(const Vec3& rhs) const {
  return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

template <typename T>
Vec3<T> Vec3<T>::cross_product(const Vec3& rhs) const {
  return Vec3<T>{
      this->y * rhs.z - this->z * rhs.y,
      this->z * rhs.x - this->x * rhs.z,
      this->x * rhs.y - this->y * rhs.x,
  };
}

template <typename T>
T Vec3<T>::euclidean_distance(const Vec3& rhs) const {
  return (*this - rhs).euclidean_norm();
}

template <typename T>
T Vec3<T>::cosine_similarity(const Vec3& rhs) const {
  return this->dot_product(rhs) / (this->euclidean_norm() * rhs.euclidean_norm());
}

template <typename T>
T Vec3<T>::cosine_angle(const Vec3& a, const Vec3& b) const {
  return T(std::acos((a - *this).cosine_similarity(b - *this)));
}

template <>
Vec3<double> Vec3<double>::snap_to_nearest_component_wise_integer_step(
    const Vec3<double>& origin,
    const Vec3<double>& step) const {
  return Vec3<double>(std::round(solve_linear_slope(this->x, step.x, origin.x)),
                      std::round(solve_linear_slope(this->y, step.y, origin.y)),
                      std::round(solve_linear_slope(this->z, step.z, origin.z)));
}

template struct Vec3<std::size_t>;
template struct Vec3<double>;
