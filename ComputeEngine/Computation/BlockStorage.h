#pragma once

#include <cmath>
#include <cstddef>
#include <tuple>
#include "Vec3.h"

namespace Computation {

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

  T get_cell(std::size_t id) const { return data[id]; };
  void set_cell(std::size_t id, T value) { data[id] = value; };

  [[nodiscard]] std::size_t size() const { return dimension_size.product(); }

  [[nodiscard]] char* get_raw_bytes() const { return reinterpret_cast<char*>(data); }
};

template <typename T>
class CellBlockInterpolation {
  // Map integer id to some vector in 3-dimensional space
  Vec3<std::size_t> dimension_size;
  Vec3<T> begin;
  Vec3<T> end;

 public:
  CellBlockInterpolation(Vec3<std::size_t> dimension_size, Vec3<T> begin, Vec3<T> end)
      : dimension_size(dimension_size), begin(begin), end(end) {}

  [[nodiscard]] Vec3<T> get_real_vec(std::size_t id) const {
    const auto pos =
        get_int_vec(id).template cast<T>().elem_division(dimension_size.cast<T>());
    return Vec3<T>{std::lerp(begin.x, end.x, pos.x), std::lerp(begin.y, end.y, pos.y),
                   std::lerp(begin.z, end.z, pos.z)};
  };
  [[nodiscard]] Vec3<std::size_t> get_int_vec(std::size_t id) const {
    const auto x_idx = (id / dimension_size.z / dimension_size.y) % dimension_size.x;
    const auto y_idx = (id / dimension_size.z) % dimension_size.y;
    const auto z_idx = id % dimension_size.z;
    return Vec3<std::size_t>{x_idx, y_idx, z_idx};
  };
  [[nodiscard]] std::size_t get_cell_count() const { return dimension_size.product(); };
};

}  // namespace Computation