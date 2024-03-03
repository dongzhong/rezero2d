// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/geometry.h"

#include <cmath>
#include <numeric>
#include <utility>

namespace rezero {

Point::Point() = default;
Point::~Point() = default;

Point::Point(double x_value, double y_value) : x_(x_value), y_(y_value) {}

Point::Point(const Point& other) : x_(other.x_), y_(other.y_) {}

Point& Point::operator=(const Point& other) {
  x_ = other.x_;
  y_ = other.y_;
  return *this;
}

Point::Point(Point&& other) {
  x_ = other.x_;
  y_ = other.y_;
  other.Reset();
}

Point& Point::operator=(Point&& other) {
  x_ = other.x_;
  y_ = other.y_;
  other.Reset();
  return *this;
}

bool Point::operator==(const Point& other) const {
  return other.x_ == x_ && other.y_ == y_;
}

bool Point::operator!=(const Point& other) const {
  return other.x_ != x_ || other.y_ != y_;
}

void Point::Reset() {
  Reset(0.0, 0.0);
}

void Point::Reset(const Point& other) {
  Reset(other.x_, other.y_);
}

void Point::Reset(double x_value, double y_value) {
  x_ = x_value;
  y_ = y_value;
}

void Point::SetX(double x_value) {
  x_ = x_value;
}

void Point::SetY(double y_value) {
  y_ = y_value;
}

Rect::Rect() = default;

Rect::~Rect() = default;

Rect::Rect(double min_x, double min_y, double max_x, double max_y)
    : min_(Point(min_x, min_y)), max_(Point(max_x, max_y)) {}

Rect::Rect(const Point& min, const Point& max) : min_(min), max_(max) {}

Rect::Rect(const Rect& other) = default;

Rect& Rect::operator=(const Rect& other) = default;

Rect::Rect(Rect&& other) : min_(std::move(other.min_)), max_(std::move(other.max_)) {}

Rect& Rect::operator=(Rect&& other) {
  *this = std::move(other);
  return *this;
}

bool Rect::IsValid() const {
  return !(std::min(min_.x_, max_.x_) < min_.x_ || std::min(min_.y_, max_.y_) < min_.y_);
}

Rect Rect::Union(const Rect& other) {
  return Rect(std::min(min_.x_, other.min_.x_),
              std::min(min_.y_, other.min_.y_),
              std::max(max_.x_, other.max_.x_),
              std::max(max_.y_, other.max_.y_));
}

} // namespace rezero
