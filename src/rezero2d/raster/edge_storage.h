// Created by DONG Zhong on 2024/02/28.

#ifndef REZERO_RASTER_EDGE_STORAGE_H_
#define REZERO_RASTER_EDGE_STORAGE_H_

#include <cstdint>
#include <vector>

#include "rezero2d/geometry.h"

namespace rezero {

struct EdgePoint {
  EdgePoint(std::int32_t x, std::int32_t y) : x(x), y(y) {}

  std::int32_t x;
  std::int32_t y;
};

enum class EdgeDirection : std::int8_t {
  kAscending = 1,
  kDescending = -1,
};

struct EdgeVector {
  void Append(std::int32_t x, std::int32_t y);

  void Reset();

  bool IsValid() const { return points.size() >= 2; }

  std::vector<EdgePoint> points;
  EdgeDirection direction;
};

struct EdgeList {
  void Append(const EdgeVector& edge_vector);

  std::vector<EdgeVector> edges;
};

struct EdgeStorage {
  EdgeStorage(std::uint32_t band_count, std::uint32_t band_height);
  ~EdgeStorage();

  std::uint32_t CalculateBandId(std::uint32_t y_cood);

  std::uint32_t band_count;
  std::uint32_t band_height;
  EdgeList* bands;

  Rect bounding_box_;
};

} // namespace rezero

#endif // REZERO_RASTER_EDGE_STORAGE_H_
