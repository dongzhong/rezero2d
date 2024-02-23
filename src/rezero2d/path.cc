// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/path.h"

#include <limits>

#include "rezero2d/edge_builder.h"

namespace rezero {

namespace {

using CommandTypeUnderlying = std::underlying_type<CommandType>::type;

} // namespace

Path::Path() {
  MoveTo(0.0, 0.0);
}

Path::~Path() = default;

void Path::MoveTo(const Point& point) {
  points_.push_back(point);
  commands_.push_back((CommandTypeUnderlying)CommandType::kMove);
}

void Path::MoveTo(double x, double y) {
  MoveTo(Point(x, y));
}

void Path::LineTo(const Point& point) {
  points_.push_back(point);
  commands_.push_back((CommandTypeUnderlying)CommandType::kOnPath);
}

void Path::LineTo(double x, double y) {
  LineTo(Point(x, y));
}

void Path::QuadTo(const Point& point1, const Point& point2) {
  points_.push_back(point1);
  points_.push_back(point2);
  commands_.push_back((CommandTypeUnderlying)CommandType::kQuad);
  commands_.push_back((CommandTypeUnderlying)CommandType::kOnPath);
}

void Path::QuadTo(double x1, double y1, double x2, double y2) {
  QuadTo(Point(x1, y1), Point(x2, y2));
}

void Path::CubicTo(const Point& point1, const Point& point2, const Point& point3) {
  points_.push_back(point1);
  points_.push_back(point2);
  points_.push_back(point3);
  commands_.push_back((CommandTypeUnderlying)CommandType::kCubic);
  commands_.push_back((CommandTypeUnderlying)CommandType::kCubic);
  commands_.push_back((CommandTypeUnderlying)CommandType::kOnPath);
}

void Path::CubicTo(double x1, double y1, double x2, double y2, double x3, double y3) {
  CubicTo(Point(x1, y1), Point(x2, y2), Point(x3, y3));
}

void Path::ConicTo(const Point& point1, const Point& point2, double weight) {
  points_.push_back(point1);
  points_.emplace_back(weight, weight);
  points_.push_back(point2);
  commands_.push_back((CommandTypeUnderlying)CommandType::kConic);
  commands_.push_back((CommandTypeUnderlying)CommandType::kWeight);
  commands_.push_back((CommandTypeUnderlying)CommandType::kOnPath);
}

void Path::ConicTo(double x1, double y1, double x2, double y2, double weight) {
  ConicTo(Point(x1, y1), Point(x2, y2), weight);
}

void Path::Close() {
  points_.emplace_back(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
  commands_.push_back((CommandTypeUnderlying)CommandType::kClose);
}

void Path::Clear() {
  points_.clear();
  commands_.clear();
}

} // namespace rezero
