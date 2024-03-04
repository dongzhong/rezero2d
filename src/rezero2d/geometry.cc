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

} // namespace rezero
