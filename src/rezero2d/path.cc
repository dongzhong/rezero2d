// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/path.h"

namespace rezero {

Path::Path() = default;

Path::~Path() = default;

void Path::MoveTo(const Point& point) {
  commands_.push_back(CommandType::kMove);
  points_.push_back(point);
}

void Path::MoveTo(double x, double y) {
  commands_.push_back(CommandType::kMove);
  points_.emplace_back(x, y);
}

void Path::LineTo(const Point& point) {
  commands_.push_back(CommandType::kOnPath);
  points_.push_back(point);
}

void Path::LineTo(double x, double y) {
  commands_.push_back(CommandType::kOnPath);
  points_.emplace_back(x, y);
}

void Path::QuadTo(const Point& point1, const Point& point2) {
  commands_.push_back(CommandType::kQuad);
  commands_.push_back(CommandType::kOnPath);

  points_.push_back(point1);
  points_.push_back(point2);
}

void Path::QuadTo(double x1, double y1, double x2, double y2) {
  commands_.push_back(CommandType::kQuad);
  commands_.push_back(CommandType::kOnPath);

  points_.emplace_back(x1, y1);
  points_.emplace_back(x2, y2);
}

void Path::CubicTo(const Point& point1, const Point& point2, const Point& point3) {
  commands_.push_back(CommandType::kCubic);
  commands_.push_back(CommandType::kCubic);
  commands_.push_back(CommandType::kOnPath);

  points_.push_back(point1);
  points_.push_back(point2);
  points_.push_back(point3);
}

void Path::CubicTo(double x1, double y1, double x2, double y2, double x3, double y3) {
  commands_.push_back(CommandType::kCubic);
  commands_.push_back(CommandType::kCubic);
  commands_.push_back(CommandType::kOnPath);

  points_.emplace_back(x1, y1);
  points_.emplace_back(x2, y2);
  points_.emplace_back(x3, y3);
}

void Path::ConicTo(const Point& point1, const Point& point2, double weight) {
  commands_.push_back(CommandType::kConic);
  commands_.push_back(CommandType::kWeight);
  commands_.push_back(CommandType::kOnPath);

  points_.push_back(point1);
  points_.emplace_back(weight, std::numeric_limits<double>::quiet_NaN());
  points_.push_back(point2);
}

void Path::ConicTo(double x1, double y1, double x2, double y2, double weight) {
  commands_.push_back(CommandType::kConic);
  commands_.push_back(CommandType::kWeight);
  commands_.push_back(CommandType::kOnPath);

  points_.emplace_back(x1, y1);
  points_.emplace_back(weight, std::numeric_limits<double>::quiet_NaN());
  points_.emplace_back(x2, y2);
}

void Path::Close() {
  commands_.push_back(CommandType::kClose);
  points_.emplace_back(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
}

} // namespace rezero
