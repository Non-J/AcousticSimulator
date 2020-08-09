#include "BlockStorage.h"
#include <cmath>
#include <utility>

using namespace BlockStorage;

template <typename T>
Memory<T>::Memory(Vec3<std::size_t> dimension_size)
    : dimension_size(std::move(dimension_size)) {
  data = std::vector<T>(this->size());
}

template <typename T>
T BlockStorage::Memory<T>::get_cell(CellId id) const {
  return data[id.index];
}

template <typename T>
void BlockStorage::Memory<T>::set_cell(CellId id, T value) {
  data[id.index] = value;
}

template <typename T>
std::size_t BlockStorage::Memory<T>::size() const {
  return dimension_size.product();
}

template <typename T>
char* BlockStorage::Memory<T>::unsafe_get_raw_bytes() const {
  return reinterpret_cast<char*>(const_cast<T*>(data.data()));
}

template class BlockStorage::Memory<double>;
template class BlockStorage::Memory<std::complex<double>>;

Interpolation::Interpolation(Vec3<std::size_t> dimension_size,
                             Vec3<double> begin,
                             Vec3<double> end)
    : dimension_size(std::move(dimension_size)),
      begin(std::move(begin)),
      end(std::move(end)) {}

Vec3<double> Interpolation::get_real_vec(CellId id) const {
  const auto pos = get_int_vec(id).template cast<double>().elem_division(
      dimension_size.cast<double>());
  return Vec3<double>{std::lerp(this->begin.x, this->end.x, pos.x),
                      std::lerp(this->begin.y, this->end.y, pos.y),
                      std::lerp(this->begin.z, this->end.z, pos.z)};
}

Vec3<std::size_t> Interpolation::get_int_vec(CellId id) const {
  const auto x_idx =
      (id.index / dimension_size.z / dimension_size.y) % dimension_size.x;
  const auto y_idx = (id.index / dimension_size.z) % dimension_size.y;
  const auto z_idx = id.index % dimension_size.z;
  return Vec3<std::size_t>{x_idx, y_idx, z_idx};
}

CellId Interpolation::get_id(const Vec3<std::size_t>& vec) const {
  return CellId{vec.x * this->dimension_size.z * this->dimension_size.y +
                vec.y * this->dimension_size.z + vec.z};
}

std::size_t Interpolation::get_cell_count() const {
  return dimension_size.product();
}
