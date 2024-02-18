// Created by DONG Zhong on 2024/02/15.

#ifndef REZERO_LINEARIZER_H_
#define REZERO_LINEARIZER_H_

#include <memory>

#include "rezero2d/path.h"

namespace rezero {

class Linearizer {
 public:
  Linearizer();
  ~Linearizer();

  std::pair<bool, Polygon> Linearize(const std::shared_ptr<Path>& path, double tolerance);

 private:
  using PointList = std::vector<Point>;

  bool CheckAndLinearizeInternal(const std::vector<Point>::iterator& begin_iter,
                                 const std::vector<Point>::iterator& end_iter,
                                 const std::vector<Path::CommandType>::iterator& begin_cmd_iter,
                                 const std::vector<Path::CommandType>::iterator& end_cmd_iter,
                                 double tolerance,
                                 Polygon& out_polygon);

  bool QuadTo(const Point& p0, const Point& p1, const Point& p2, double tolerance, Polygon& out_polygon);

  bool CubicTo(const Point& p0, const Point& p1, const Point& p2, const Point& p3, double tolerance, Polygon& out_polygon);

  bool ConicTo(const Point& p0, const Point& p1, const Point& p2, double weight, double tolerance, Polygon& out_polygon);

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Linearizer);
};

} // namespace rezero

#endif // REZERO_LINEARIZER_H_
