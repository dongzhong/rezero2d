// Created by DONG Zhong on 2024/02/22.

#ifndef REZERO_RASTER_EDGE_BUILDER_H_
#define REZERO_RASTER_EDGE_BUILDER_H_

#include <memory>

#include "rezero2d/base/macros.h"
#include "rezero2d/raster/edge_source.h"
#include "rezero2d/raster/edge_storage.h"

namespace rezero {

class EdgeBuilder {
 public:
  EdgeBuilder(EdgeStorage* edge_storage);
  EdgeBuilder(EdgeStorage* edge_storage, const Rect& clipping_box, double tolerance);
  ~EdgeBuilder();

  void SetTolerance(double tolerance);

  void Begin();
  void End();

  bool AddPath(const std::shared_ptr<Path>& path);

 private:
  struct State {
    Point p0;
    std::uint32_t flags;
    // TODO:
  };

  void LineTo(EdgeSource& source, const Point& p, State& state);
  void QuadTo(EdgeSource& source, State& state);
  void CubicTo(EdgeSource& source, State& state);
  void ConicTo(EdgeSource& source, State& state);

  void BeginAscending();
  void EndAscending();

  void BeginDescending();
  void EndDescending();

  void AccumulateLeftBorder(double border_y0, double border_y1);
  void AccumulateRightBorder(double border_y0, double border_y1);

  void AddLineSegment(const Point& p0, const Point& p1);

  void FlushBorderAccumulators();
  void ResetBorderAccumulators();

  void EmitLeftBorder();
  void EmitRightBorder();

  void AddCloseLine(std::int32_t x0_coord, std::int32_t y0_coord, std::int32_t x1_coord, std::int32_t y1_coord);

  template <typename MonoCurveType>
  void FlattenMonoCurve(MonoCurveType& mono_curve, const Point* src, EdgeDirection direction);

  template <typename MonoCurveType>
  void FlattenMonoCurveClipping(MonoCurveType& mono_curve, const Point* src, EdgeDirection direction);

  double tolerance_sq_;

  Rect clipping_box_;

  EdgeStorage* edge_storage_;

  EdgeVector current_edge_;

  Rect bounding_box_;

  double border_X0Y0_;
  double border_X0Y1_;
  double border_X1Y0_;
  double border_X1Y1_;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(EdgeBuilder);
};

} // namespace rezero

#endif // REZERO_RASTER_EDGE_BUILDER_H_
