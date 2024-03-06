// Created by DONG Zhong on 2024/03/06.

#include "rezero2d/raster/flatten_utils.h"

namespace rezero {

FlattenMonoQuad::FlattenMonoQuad(double tolerance_sq) : tolerance_sq_(tolerance_sq) {}

FlattenMonoQuad::~FlattenMonoQuad() = default;

void FlattenMonoQuad::Begin(const Point* src, EdgeDirection direction) {
  p0_ = src[0];
  p1_ = src[1];
  p2_ = src[2];
}

bool FlattenMonoQuad::IsFlat(Step& step) {
  Point v1 = p1_ - p0_;
  Point v2 = p2_ - p0_;

  double d = v2.x * v1.y - v2.y * v1.x;
  double length_sq = v2.x * v2.x + v2.y * v2.y;

  // `step.value / length_sq` is the length`s square between `p1` and `v2`.
  step.value = d * d;
  step.limit = tolerance_sq_ * length_sq;

  return step.value <= step.limit;
}

void FlattenMonoQuad::Split(Step& step) {
  step.p01 = (p0_ + p1_) * 0.5;
  step.p12 = (p1_ + p2_) * 0.5;
  step.p012 = (step.p01 + step.p12) * 0.5;
}

void FlattenMonoQuad::Push(const Step& step) {
  stack_.push_back(step.p012);
  stack_.push_back(step.p12);
  stack_.push_back(p2_);

  p1_ = step.p01;
  p2_ = step.p012;
}

void FlattenMonoQuad::Pop() {
  p2_ = stack_.back(); stack_.pop_back();
  p1_ = stack_.back(); stack_.pop_back();
  p0_ = stack_.back(); stack_.pop_back();
}

} // namespace rezero
