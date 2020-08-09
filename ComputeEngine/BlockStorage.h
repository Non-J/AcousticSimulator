#pragma once

#include <complex>
#include <cstddef>
#include <vector>
#include "Vec3.h"

namespace BlockStorage {

/**
 * Type used for referencing a cell
 */
struct CellId {
  std::size_t index;
};

/**
 * A block of 3 dimension contiguous memory
 * @tparam T type of elements in memory
 */
template <typename T>
class Memory {
  Vec3<std::size_t> dimension_size;
  std::vector<T> data;

 public:
  /**
   * Construct empty Memory from dimension_size
   * @param dimension_size Element count of each dimension axis
   */
  Memory(Vec3<std::size_t> dimension_size);

  [[nodiscard]] T get_cell(CellId id) const;
  void set_cell(CellId id, T value);

  [[nodiscard]] std::size_t size() const;

  /**
   * UNSAFE. Used to export the raw data
   * @return raw pointer to memory
   */
  [[nodiscard]] char* unsafe_get_raw_bytes() const;
};

extern template class Memory<double>;
extern template class BlockStorage::Memory<std::complex<double>>;

/**
 * Map CellId to 3 dimension real vector space
 */
class Interpolation {
  Vec3<std::size_t> dimension_size;
  Vec3<double> begin;
  Vec3<double> end;

 public:
  /**
   * Construct Interpolation from dimension_size, begin, end. No check is performed.
   * @param dimension_size Element count of integer vector space in each dimension axis
   * @param begin Lowerbound of real vector space
   * @param end Upperbound of real vector space
   */
  Interpolation(Vec3<std::size_t> dimension_size, Vec3<double> begin, Vec3<double> end);

  [[nodiscard]] Vec3<double> get_real_vec(CellId id) const;

  [[nodiscard]] Vec3<std::size_t> get_int_vec(CellId id) const;

  [[nodiscard]] CellId get_id(const Vec3<std::size_t>& vec) const;

  [[nodiscard]] std::size_t get_cell_count() const;
};

}  // namespace BlockStorage