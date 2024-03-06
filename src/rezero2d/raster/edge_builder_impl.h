// Created by DONG Zhong on 2024/03/06.

#ifndef REZERO_RASTER_EDGE_BUILDER_IMPL_H_
#define REZERO_RASTER_EDGE_BUILDER_IMPL_H_

#include "rezero2d/raster/edge_builder.h"

namespace rezero {

template <typename MonoCurveType>
void EdgeBuilder::FlattenMonoCurve(MonoCurveType& mono_curve, const Point* src, EdgeDirection direction) {
  mono_curve.Begin(src, direction);
  if (direction == EdgeDirection::kAscending) {
    BeginAscending();
  } else {
    BeginDescending();
  }
  std::int32_t x_coord = static_cast<std::int32_t>(mono_curve.First().x);
  std::int32_t y_coord = static_cast<std::int32_t>(mono_curve.First().y);
  current_edge_.Append(x_coord, y_coord);

  while (true) {
    typename MonoCurveType::Step step;
    if (!mono_curve.IsFlat(step)) {
      mono_curve.Split(step);
      mono_curve.Push(step);
      continue;
    }

    // AddLine.
    x_coord = static_cast<std::int32_t>(mono_curve.Last().x);
    y_coord = static_cast<std::int32_t>(mono_curve.Last().y);
    current_edge_.Append(x_coord, y_coord);

    if (!mono_curve.CanPop()) {
      break;
    }
    mono_curve.Pop();
  }

  if (current_edge_.points.front().y == current_edge_.points.back().y) {
    current_edge_.Reset();
  } else {
    if (direction == EdgeDirection::kAscending) {
      EndAscending();
    } else {
      EndDescending();
    }
  }
}

template <typename MonoCurveType>
void EdgeBuilder::FlattenMonoCurveClipping(MonoCurveType& mono_curve, const Point* src, EdgeDirection direction) {
  // TODO:
}

} // namespace rezero

#endif // REZERO_RASTER_EDGE_BUILDER_IMPL_H_
