// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/geometry.h"

#include <algorithm>

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

Size::Size() = default;
Size::~Size() = default;

Size::Size(double w_value, double h_value) : w_(w_value), h_(h_value) {}

Size::Size(const Size& other) : w_(other.w_), h_(other.h_) {}

Size& Size::operator=(const Size& other) {
  w_ = other.w_;
  h_ = other.h_;
  return *this;
}

Size::Size(Size&& other) {
  w_ = other.w_;
  h_ = other.h_;
  other.Reset();
}

Size& Size::operator=(Size&& other) {
  w_ = other.w_;
  h_ = other.h_;
  other.Reset();
  return *this;
}

bool Size::operator==(const Size& other) const {
  return other.w_ == w_ && other.h_ == h_;
}

bool Size::operator!=(const Size& other) const {
  return other.w_ != w_ || other.h_ != h_;
}

void Size::Reset() {
  Reset(0.0, 0.0);
}

void Size::Reset(const Size& other) {
  Reset(other.w_, other.h_);
}

void Size::Reset(double w_value, double h_value) {
  w_ = w_value;
  h_ = h_value;
}

void Size::SetWidth(double w_value) {
  w_ = w_value;
}

void Size::SetHeight(double h_value) {
  h_ = h_value;
}

Rect::Rect() = default;
Rect::~Rect() = default;

Rect::Rect(double x_value, double y_value, double w_value, double h_value)
    : x_(x_value), y_(y_value), w_(w_value), h_(h_value) {}

Rect::Rect(const Rect& other)
    : x_(other.x_), y_(other.y_), w_(other.w_), h_(other.h_) {}

Rect& Rect::operator=(const Rect& other) {
  x_ = other.x_;
  y_ = other.y_;
  w_ = other.w_;
  h_ = other.h_;
  return *this;
}

Rect::Rect(Rect&& other) {
  x_ = other.x_;
  y_ = other.y_;
  w_ = other.w_;
  h_ = other.h_;
  other.Reset();
}

Rect& Rect::operator=(Rect&& other) {
  x_ = other.x_;
  y_ = other.y_;
  w_ = other.w_;
  h_ = other.h_;
  return *this;
}

bool Rect::operator==(const Rect& other) const {
  return other.x_ == x_ && other.y_ == y_ && other.w_ == w_ && other.h_ == h_;
}

bool Rect::operator!=(const Rect& other) const {
  return other.x_ != x_ || other.y_ != y_ || other.w_ != w_ || other.h_ != h_;
}

void Rect::Reset() {
  Reset(0.0, 0.0, 0.0, 0.0);
}

void Rect::Reset(const Rect& other) {
  Reset(other.x_, other.y_, other.w_, other.h_);
}

void Rect::Reset(double x_value, double y_value, double w_value, double h_value) {
  x_ = x_value;
  y_ = y_value;
  w_ = w_value;
  h_ = h_value;
}

void Rect::SetX(double x_value) {
  x_ = x_value;
}

void Rect::SetY(double y_value) {
  y_ = y_value;
}

void Rect::SetWidth(double w_value) {
  w_ = w_value;
}

void Rect::SetHeight(double h_value) {
  h_ = h_value;
}

Contour::Contour() = default;

Contour::~Contour() = default;

Contour::Contour(const Contour& other) {
  points_ = other.points_;
}

Contour& Contour::operator=(const Contour& other) {
  points_ = other.points_;
  return *this;
}

Contour::Contour(Contour&& other) {
  points_.assign(other.points_.begin(), other.points_.end());
  other.points_.clear();
}

Contour& Contour::operator=(Contour&& other) {
  points_.assign(other.points_.begin(), other.points_.end());
  other.points_.clear();
  return *this;
}

void Contour::Reset() {
  points_.clear();
}

void Contour::AppendPoint(const Point& p) {
  points_.push_back(p);
}

void Contour::AppendPoint(double x_value, double y_value) {
  points_.emplace_back(x_value, y_value);
}

void Contour::AppendPoints(const std::vector<Point>& points) {
  for (auto&& p : points) {
    points_.push_back(p);
  }
}

Polygon::Polygon() = default;

Polygon::~Polygon() = default;

Polygon::Polygon(const Polygon& other) {
  contours_ = other.contours_;
}

Polygon& Polygon::operator=(const Polygon& other) {
  contours_ = other.contours_;
  return *this;
}

Polygon::Polygon(Polygon&& other) {
  contours_.assign(other.contours_.begin(), other.contours_.end());
  other.contours_.clear();
}

Polygon& Polygon::operator=(Polygon&& other) {
  contours_.assign(other.contours_.begin(), other.contours_.end());
  other.contours_.clear();
  return *this;
}

void Polygon::Reset() {
  contours_.clear();
}

void Polygon::NewContour() {
  contours_.push_back({});
}

void Polygon::AppendPoint(const Point& point) {
  if (contours_.empty()) {
    NewContour();
  }

  contours_.back().AppendPoint(point);
}

void Polygon::AppendPoints(const std::vector<Point>& points) {
  if (contours_.empty()) {
    NewContour();
  }

  contours_.back().AppendPoints(points);
}

void Polygon::Validate() {
  auto iter = std::remove_if(contours_.begin(), contours_.end(),
                             [](const Contour& contour) { return !contour.IsValid(); });
  contours_.erase(iter, contours_.end());
}

} // namespace rezero
