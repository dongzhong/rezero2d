// Created by DONG Zhong on 2024/03/06.

#ifndef REZERO_RASTER_FLATTEN_DATA_H_
#define REZERO_RASTER_FLATTEN_DATA_H_

#include <vector>

#include "rezero2d/geometry.h"
#include "rezero2d/raster/edge_storage.h"

namespace rezero {

class FlattenMonoQuad {
 public:
  struct Step {
    double value;
    double limit;

    Point p01;
    Point p12;
    Point p012;
  };

  FlattenMonoQuad(double tolerance_sq);
  ~FlattenMonoQuad();

  void Begin(const Point* src, EdgeDirection direction);

  bool IsFlat(Step& step);

  void Split(Step& step);

  void Push(const Step& step);

  bool CanPop() const { return !stack_.empty(); }

  void Pop();

  const Point& First() const { return p0_; }
  const Point& Last() const { return p2_; }

 private:
  double tolerance_sq_;

  Point p0_;
  Point p1_;
  Point p2_;

  std::vector<Point> stack_;
};

} // namespace rezero

#endif // REZERO_RASTER_FLATTEN_DATA_H_
