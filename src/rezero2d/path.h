// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_PATH_H_
#define REZERO_PATH_H_

#include <cstdint>
#include <vector>

#include "rezero2d/base/macros.h"
#include "rezero2d/geometry.h"

namespace rezero {

class EdgeBuilder;

class Path {
 public:
  Path();
  ~Path();

  void MoveTo(const Point& point);
  void MoveTo(double x, double y);

  void LineTo(const Point& point);
  void LineTo(double x, double y);

  void QuadTo(const Point& point1, const Point& point2);
  void QuadTo(double x1, double y1, double x2, double y2);

  void CubicTo(const Point& point1, const Point& point2, const Point& point3);
  void CubicTo(double x1, double y1, double x2, double y2, double x3, double y3);

  void ConicTo(const Point& point1, const Point& point2, double weight);
  void ConicTo(double x1, double y1, double x2, double y2, double weight);

  void Close();

  void Clear();

 private:
  std::vector<Point> points_;
  std::vector<std::uint8_t> commands_;

  friend class EdgeBuilder;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(Path);
};

} // namespace rezero

#endif // REZERO_PATH_H_
