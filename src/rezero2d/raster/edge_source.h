// Created by DONG Zhong on 2024/02/28.

#ifndef REZERO_RASTER_EDGE_SOURCE_H_
#define REZERO_RASTER_EDGE_SOURCE_H_

#include <cstdint>

#include "rezero2d/base/macros.h"
#include "rezero2d/path.h"

namespace rezero {

enum class CommandType : std::uint8_t {
  kMove   = 0,
  kOnPath = 1,
  kQuad   = 2,
  kCubic  = 3,
  kConic  = 4,
  kWeight = 5,
  kClose  = 6,
};

// TODO:
class EdgeTransform {
 public:
  EdgeTransform() = default;
  ~EdgeTransform() = default;

  void Apply(Point& dst, const Point& src) { dst = src; }
};

class EdgeSource {
 public:
  EdgeSource(const EdgeTransform& transform, const Point* vertex_data,
             const CommandType* cmd_data, std::size_t count);

  ~EdgeSource() = default;

  bool Begin(Point& p);

  bool IsClose() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kClose; }
  bool IsLineTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kOnPath; }
  bool IsQuadTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kQuad; }
  bool IsCubicTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kCubic; }
  bool IsConicTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kConic; }

  inline void NextLineTo(Point& p);
  inline bool MaybeNextLineTo(Point& p);

  inline void NextQuadTo(Point& p1, Point& p2);
  inline bool MaybeNextQuadTo(Point& p1, Point& p2);

  inline void NextCubicTo(Point& p1, Point& p2, Point& p3);
  inline bool MaybeNextCubicTo(Point& p1, Point& p2, Point& p3);

  inline void NextConicTo(Point& p1, Point& p2, double& weight);
  inline bool MaybeNextConicTo(Point& p1, Point& p2, double& weight);

 private:
  EdgeSource() = delete;

  const Point* vertex_ptr_;
  const CommandType* cmd_ptr_;
  const CommandType* cmd_start_;
  const CommandType* cmd_end_;

  EdgeTransform transform_;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(EdgeSource);
};

void EdgeSource::NextLineTo(Point& p) {
  transform_.Apply(p, *vertex_ptr_);
  ++cmd_ptr_;
  ++vertex_ptr_;
}

bool EdgeSource::MaybeNextLineTo(Point& p) {
  if (!IsLineTo()) {
    return false;
  }
  NextLineTo(p);
  return true;
}

void EdgeSource::NextQuadTo(Point& p1, Point& p2) {
  transform_.Apply(p1, *vertex_ptr_);
  transform_.Apply(p2, *(vertex_ptr_ + 1));
  cmd_ptr_ += 2;
  vertex_ptr_ += 2;
}

bool EdgeSource::MaybeNextQuadTo(Point& p1, Point& p2) {
  if (!IsQuadTo()) {
    return false;
  }
  NextQuadTo(p1, p2);
  return true;
}

void EdgeSource::NextCubicTo(Point& p1, Point& p2, Point& p3) {
  transform_.Apply(p1, *vertex_ptr_);
  transform_.Apply(p2, *(vertex_ptr_ + 1));
  transform_.Apply(p3, *(vertex_ptr_ + 2));

  cmd_ptr_ += 3;
  vertex_ptr_ += 3;
}

bool EdgeSource::MaybeNextCubicTo(Point& p1, Point& p2, Point& p3) {
  if (!IsCubicTo()) {
    return false;
  }
  NextCubicTo(p1, p2, p3);
  return true;
}

void EdgeSource::NextConicTo(Point& p1, Point& p2, double& weight) {
  transform_.Apply(p1, *vertex_ptr_);
  transform_.Apply(p2, *(vertex_ptr_ + 2));
  weight = (vertex_ptr_ + 1)->x;

  cmd_ptr_ += 3;
  vertex_ptr_ += 3;
}

bool EdgeSource::MaybeNextConicTo(Point& p1, Point& p2, double& weight) {
  if (!IsConicTo()) {
    return false;
  }
  NextConicTo(p1, p2, weight);
  return true;
}

} // namespace rezero

#endif // REZERO_RASTER_EDGE_SOURCE_H_
