// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/geometry.h"

#include <cmath>
#include <numeric>
#include <utility>

namespace rezero {

Point::Point() = default;
Point::~Point() = default;

Point::Point(double x_value, double y_value) : x(x_value), y(y_value) {}

Point::Point(const Point& other) : x(other.x), y(other.y) {}

Point& Point::operator=(const Point& other) {
  x = other.x;
  y = other.y;
  return *this;
}

Point::Point(Point&& other) {
  x = other.x;
  y = other.y;
  other.Reset();
}

Point& Point::operator=(Point&& other) {
  x = other.x;
  y = other.y;
  other.Reset();
  return *this;
}

bool Point::operator==(const Point& other) const {
  return other.x == x && other.y == y;
}

bool Point::operator!=(const Point& other) const {
  return other.x != x || other.y != y;
}

void Point::Reset() {
  Reset(0.0, 0.0);
}

void Point::Reset(const Point& other) {
  Reset(other.x, other.y);
}

void Point::Reset(double x_value, double y_value) {
  x = x_value;
  y = y_value;
}

Rect::Rect() = default;

Rect::~Rect() = default;

Rect::Rect(double min_x, double min_y, double max_x, double max_y)
    : min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y) {}

Rect::Rect(const Point& min, const Point& max)
    : min_x(min.x), min_y(min.y), max_x(max.x), max_y(max.y) {}

Rect::Rect(const Rect& other) = default;

Rect& Rect::operator=(const Rect& other) = default;

Rect::Rect(Rect&& other)
    : min_x(std::move(other.min_x)), min_y(std::move(other.min_y)),
      max_x(std::move(other.max_x)), max_y(std::move(other.max_y)) {}

Rect& Rect::operator=(Rect&& other) {
  *this = std::move(other);
  return *this;
}

bool Rect::IsValid() const {
  return !(std::min(min_x, max_x) < min_x || std::min(min_y, max_y) < min_y);
}

Rect Rect::Union(const Rect& other) {
  return Rect(std::min(min_x, other.min_x),
              std::min(min_y, other.min_y),
              std::max(max_x, other.max_x),
              std::max(max_y, other.max_y));
}

namespace {

/*
 * A = p0        + p1 * (-2) + p2
 * B = p0 * (-2) + p1 * 2
 * C = p0
 *
 * V = (A * t + B) * t + C
 */
void CalculateQuadCoefficients(const Point p[3], Point& a, Point& b, Point& c) {
  auto v1 = p[1] - p[0];
  auto v2 = p[2] - p[1];

  a = v2 - v1;
  b = v1 + v1;
  c = p[0];
}

} // namespace

Point* QuadHelper::SplitQuadToSpline(const Point p[3], Point* out) {
  Point pa, pb, pc;
  CalculateQuadCoefficients(p, pa, pb, pc);

  Point extrema_ts = (p[0] - p[1]) / (p[0] - p[1] * 2.0 + p[2]);
  double extrema_t0 = std::min(extrema_ts.x, extrema_ts.y);
  double extrema_t1 = std::max(extrema_ts.x, extrema_ts.y);

  std::vector<double> ts;
  if (extrema_t0 > 0.0 && extrema_t0 < 1.0) {
    ts.push_back(extrema_t0);
  }
  if (extrema_t1 > std::max(extrema_t0, 0.0) && extrema_t1 < 1.0) {
    ts.push_back(extrema_t1);
  }

  // If has extremas, split the curve to spline.
  if (!ts.empty()) {
    ts.push_back(1.0);

    out[0] = p[0];
    Point last = p[2];

    std::size_t i = 0;
    double t_cut = 0.0;

    do {
      double t_val = ts[i];

      double dt = (t_val - t_cut) * 0.5;

      // Derivative: 2 * a * t + b.
      Point cp = (pa * (t_val * 2.0) + pb) * dt;
      Point tp = (pa * t_val + pb) * t_val + pc;

      if (++i == ts.size()) {
        tp = last;
      }

      out[1].Reset(tp - cp);
      out[2].Reset(tp);
      out += 2;

      t_cut = t_val;
    } while (i != ts.size());
  }

  return out;
}

} // namespace rezero
