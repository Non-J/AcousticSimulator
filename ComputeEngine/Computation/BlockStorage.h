#pragma once

#include <cmath>
#include <cstddef>
#include <tuple>
#include <vector>
#include "Vec3.h"

namespace Computation {

template <typename T>
class CellBlock {
  // 3-dimensional contiguous memory
  Vec3<std::size_t> dimension_size;
  std::vector<T> data;

 public:
  CellBlock(Vec3<std::size_t> dimension_size) : dimension_size(dimension_size) {
    data = std::vector<T>(dimension_size.product());
  }

  T get_cell(std::size_t id) const { return data[id]; };
  void set_cell(std::size_t id, T value) { data[id] = value; };

  [[nodiscard]] std::size_t size() const { return dimension_size.product(); }

  [[nodiscard]] char* unsafe_get_raw_bytes() {
    return reinterpret_cast<char*>(data.data());
  }
};

class CellBlockInterpolation {
  // Map integer id to some vector in 3-dimensional space
  Vec3<std::size_t> dimension_size;
  Vec3<double> begin;
  Vec3<double> end;

 public:
  CellBlockInterpolation(Vec3<std::size_t> dimension_size,
                         Vec3<double> begin,
                         Vec3<double> end)
      : dimension_size(dimension_size), begin(begin), end(end) {}

  [[nodiscard]] Vec3<double> get_real_vec(std::size_t id) const {
    const auto pos = get_int_vec(id).template cast<double>().elem_division(
        dimension_size.cast<double>());
    return Vec3<double>{std::lerp(this->begin.x, this->end.x, pos.x),
                        std::lerp(this->begin.y, this->end.y, pos.y),
                        std::lerp(this->begin.z, this->end.z, pos.z)};
  };

  [[nodiscard]] Vec3<std::size_t> get_int_vec(std::size_t id) const {
    const auto x_idx = (id / dimension_size.z / dimension_size.y) % dimension_size.x;
    const auto y_idx = (id / dimension_size.z) % dimension_size.y;
    const auto z_idx = id % dimension_size.z;
    return Vec3<std::size_t>{x_idx, y_idx, z_idx};
  };

  [[nodiscard]] std::size_t get_id(const Vec3<std::size_t>& vec) const {
    return vec.x * this->dimension_size.z * this->dimension_size.y +
           vec.y * this->dimension_size.z + vec.z;
  }

  [[nodiscard]] std::size_t get_cell_count() const { return dimension_size.product(); };
};

}  // namespace Computation