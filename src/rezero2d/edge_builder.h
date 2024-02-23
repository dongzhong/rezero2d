// Created by DONG Zhong on 2024/02/22.

#ifndef REZERO_EDGE_BUILDER_H_
#define REZERO_EDGE_BUILDER_H_

#include <memory>

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

  inline bool Begin(Point& p);

  bool IsClose() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kClose; }
  bool IsLineTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kOnPath; }
  bool IsQuadTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kQuad; }
  bool IsCubicTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kCubic; }
  bool IsConicTo() const { return cmd_ptr_ != cmd_end_ && *cmd_ptr_ == CommandType::kConic; }

  inline void NextLineTo(Point& p);

  inline void NextQuadTo(Point& p1, Point& p2);

  inline void NextCubicTo(Point& p1, Point& p2, Point& p3);

  inline void NextConicTo(Point& p1, Point& p2, double& weight);

 private:
  EdgeSource() = delete;

  const Point* vertex_ptr_;
  const CommandType* cmd_ptr_;
  const CommandType* cmd_start_;
  const CommandType* cmd_end_;

  EdgeTransform transform_;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(EdgeSource);
};

class EdgeBuilder {
 public:
  EdgeBuilder();
  ~EdgeBuilder();

  void SetTolerance(double tolerance);

  bool AddPath(const std::shared_ptr<Path>& path);

 private:
  struct State {
    Point p0;
    // TODO:
  };

  void LineTo(EdgeSource& source, State& state);
  void LineTo(EdgeSource& source, const Point& p, State& state);
  void QuadTo(EdgeSource& source, State& state);
  void CubicTo(EdgeSource& source, State& state);
  void ConicTo(EdgeSource& source, State& state);

  double tolerance_;

  REZERO_DISALLOW_COPY_ASSIGN_AND_MOVE(EdgeBuilder);
};

} // namespace rezero

#endif // REZERO_EDGE_BUILDER_H_
