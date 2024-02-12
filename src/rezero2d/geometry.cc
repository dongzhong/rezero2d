// Created by DONG Zhong on 2024/02/06.

#include "rezero2d/geometry.h"

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

} // namespace rezero
