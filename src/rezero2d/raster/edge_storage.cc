// Created by DONG Zhong on 2024/02/28.

#include "rezero2d/raster/edge_storage.h"

#include "rezero2d/base/logging.h"

namespace rezero {

void EdgeVector::Append(std::int32_t x, std::int32_t y) {
  points.emplace_back(x, y);
}

void EdgeVector::Reset() {
  points.clear();
}

void EdgeList::Append(const EdgeVector& edge_vector) {
  edges.push_back(edge_vector);
}

EdgeStorage::EdgeStorage(std::uint32_t band_count, std::uint32_t band_height)
    : band_count(band_count), band_height(band_height),
      bounding_box_(std::numeric_limits<double>::max(),
                    std::numeric_limits<double>::max(),
                    std::numeric_limits<double>::min(),
                    std::numeric_limits<double>::min()) {
  REZERO_DCHECK(band_count > 0);

  if (band_count > 0) {
    bands = new EdgeList[band_count];
  }
}

EdgeStorage::~EdgeStorage() {
  if (bands) {
    delete [] bands;
  }
}

std::uint32_t EdgeStorage::CalculateBandId(std::uint32_t y_cood) {
  return y_cood / band_height;
}

} // namespace rezero
