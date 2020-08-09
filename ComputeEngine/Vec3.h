#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>

/**
 * Basic implementation of Vector3D
 * @tparam T numerical type
 */
template <typename T>
struct Vec3 {
  T x, y, z;

  /*
   * Construct empty Vec3
   */
  Vec3() = default;
  /*
   * Construct Vec3 from x, y, z as parameters
   */
  Vec3(T x, T y, T z);
  /*
   * Construct Vec3 from value where each component is value
   */
  static Vec3<T> fromValue(T x);

  /*
   * Change type of elements stored in Vec3
   */
  template <typename V>
  [[nodiscard]] Vec3<V> cast() const {
    return Vec3<V>{V(x), V(y), V(z)};
  };

  /*
   * Product of each component
   */
  [[nodiscard]] T product() const;

  /*
   * Element-wise absolute
   */
  [[nodiscard]] Vec3<T> elem_abs() const;

  /*
   * Squared euclidean norm (distance) of vector
   */
  [[nodiscard]] T euclidean_norm_squared() const;

  /*
   * Euclidean norm (distance) of vector
   */
  [[nodiscard]] T euclidean_norm() const;

  /*
   * Element-wise equality check
   */
  bool operator==(const Vec3& rhs) const;

  /*
   * Element-wise inequality check
   */
  bool operator!=(const Vec3& rhs) const;

  /*
   * Element-wise addition by constant
   */
  Vec3<T> operator+(const T& rhs) const;

  /*
   * Element-wise subtraction by constant
   */
  Vec3<T> operator-(const T& rhs) const;

  /*
   * Element-wise scaling by constant
   */
  Vec3<T> operator*(const T& rhs) const;

  /*
   * Element-wise division by constant
   */
  Vec3<T> operator/(const T& rhs) const;

  /*
   * Vector addition
   */
  Vec3<T> operator+(const Vec3<T>& rhs) const;

  /*
   * Vector subtraction
   */
  Vec3<T> operator-(const Vec3<T>& rhs) const;

  /*
   * Hadamard/Schur product (element-wise product)
   */
  [[nodiscard]] Vec3<T> elem_product(const Vec3& rhs) const;

  /*
   * Element-wise division with another vector
   */
  [[nodiscard]] Vec3<T> elem_division(const Vec3& rhs) const;

  /*
   * Dot product with another vector
   */
  [[nodiscard]] T dot_product(const Vec3& rhs) const;

  /*
   * Cross product with another vector
   */
  [[nodiscard]] Vec3<T> cross_product(const Vec3& rhs) const;

  /*
   * Euclidean distance between endpoint of this vector to another vector
   */
  [[nodiscard]] T euclidean_distance(const Vec3& rhs) const;

  /*
   * Cosine similarity with another vector
   */
  [[nodiscard]] T cosine_similarity(const Vec3& rhs) const;

  /**
   * Cosine angle formed by vector a, b; where this vector is the vertex point
   * @param a One point of the angle
   * @param b One point of the angle
   * @return Cosine angle
   */
  [[nodiscard]] T cosine_angle(const Vec3& a, const Vec3& b) const;

  /**
   * Get vector which is nearest to some integer=multiple step away from origin
   *
   * For each component, the resulting vector's magnitude is such that it is nearest to
   * integer-multiple of step away from origin.
   *
   * @param origin the initial constant for integer step 0
   * @param step the step size
   * @return vector with magnitude computed as described
   */
  [[nodiscard]] Vec3<T> snap_to_nearest_component_wise_integer_step(
      const Vec3<T>& origin,
      const Vec3<T>& step) const;
};

/*
 * Element-wise absolute for std::size_t
 * Implementation: nothing as std::size_t is unsigned already
 */
template <>
Vec3<std::size_t> Vec3<std::size_t>::elem_abs() const;

/*
 * Element-wise absolute for double
 * Implementation: uses std::fabs
 */
template <>
Vec3<double> Vec3<double>::elem_abs() const;

/*
 * Equality check for std::size_t (specialization)
 */
template <>
bool Vec3<std::size_t>::operator==(const Vec3& rhs) const;
template <>
bool Vec3<std::size_t>::operator!=(const Vec3& rhs) const;

/*
 * Equality check for double
 * Deleted as equality check for double should be done on a case-by-case basis due to
 * (lack of) precision associated with floating point
 */
template <>
bool Vec3<double>::operator==(const Vec3& rhs) const = delete;
template <>
bool Vec3<double>::operator!=(const Vec3& rhs) const = delete;

/*
 * This method is intended only to be used for Vec3<double>
 */
template <>
Vec3<std::size_t> Vec3<std::size_t>::snap_to_nearest_component_wise_integer_step(
    const Vec3<std::size_t>& origin,
    const Vec3<std::size_t>& step) const = delete;
template <>
Vec3<double> Vec3<double>::snap_to_nearest_component_wise_integer_step(
    const Vec3<double>& origin,
    const Vec3<double>& step) const;

// Vec3 should only be used for these types as it doesn't make sense to use with others
extern template struct Vec3<std::size_t>;
extern template struct Vec3<double>;
