// Created by DONG Zhong on 2024/02/22.

#include "rezero2d/edge_builder.h"

#include <limits>

#include "rezero2d/base/logging.h"

namespace rezero {

EdgeSource::EdgeSource(const EdgeTransform& transform, const Point* vertex_data,
                       const CommandType* cmd_data, std::size_t count)
    : transform_(transform), vertex_ptr_(vertex_data), cmd_ptr_(cmd_data),
      cmd_start_(cmd_data), cmd_end_(cmd_data + count) {}

bool EdgeSource::Begin(Point& p) {
  while (true) {
    if (cmd_ptr_ == cmd_end_) {
      return false;
    }

    auto cmd = *cmd_ptr_;
    ++cmd_ptr_;
    ++vertex_ptr_;

    if (cmd != CommandType::kMove) {
      continue;
    }

    transform_.Apply(p, *(vertex_ptr_ - 1));

    return true;
  }
}

void EdgeSource::NextLineTo(Point& p) {
  transform_.Apply(p, *vertex_ptr_);
  ++cmd_ptr_;
  ++vertex_ptr_;
}

void EdgeSource::NextQuadTo(Point& p1, Point& p2) {
  transform_.Apply(p1, *vertex_ptr_);
  transform_.Apply(p2, *(vertex_ptr_ + 1));
  cmd_ptr_ += 2;
  vertex_ptr_ += 2;
}

void EdgeSource::NextCubicTo(Point& p1, Point& p2, Point& p3) {
  transform_.Apply(p1, *vertex_ptr_);
  transform_.Apply(p2, *(vertex_ptr_ + 1));
  transform_.Apply(p3, *(vertex_ptr_ + 2));

  cmd_ptr_ += 3;
  vertex_ptr_ += 3;
}

void EdgeSource::NextConicTo(Point& p1, Point& p2, double& weight) {
  transform_.Apply(p1, *vertex_ptr_);
  transform_.Apply(p2, *(vertex_ptr_ + 2));
  weight = (vertex_ptr_ + 1)->GetX();

  cmd_ptr_ += 3;
  vertex_ptr_ += 3;
}

EdgeBuilder::EdgeBuilder() : tolerance_(std::numeric_limits<double>::max()) {}

EdgeBuilder::~EdgeBuilder() = default;

void EdgeBuilder::SetTolerance(double tolerance) {
  tolerance_ = tolerance;
}

bool EdgeBuilder::AddPath(const std::shared_ptr<Path>& path) {
  if (!path) {
    return false;
  }

  const auto& points = path->points_;
  const auto& commands = path->commands_;

  REZERO_DCHECK(points.size() == commands.size());

  EdgeSource edge_source(EdgeTransform(), &points.front(), (CommandType*)&commands.front(), commands.size());

  Point begin_point;
  State state;
  while (edge_source.Begin(state.p0)) {
    while (true) {
      if (edge_source.IsLineTo()) {
        LineTo(edge_source, state);
      } else if (edge_source.IsQuadTo()) {
        QuadTo(edge_source, state);
      } else if (edge_source.IsCubicTo()) {
        CubicTo(edge_source, state);
      } else if (edge_source.IsConicTo()) {
        ConicTo(edge_source, state);
      } else if (edge_source.IsClose()) {
        LineTo(edge_source, begin_point, state);
      } else {
        break;
      }
    }
  }

  return true;
}

void EdgeBuilder::LineTo(EdgeSource& source, State& state) {
  Point points[2];
  Point& p0 = state.p0;
  Point& p1 = points[1];
  source.NextLineTo(p1);

  // TODO:

  p0 = p1;
}

void EdgeBuilder::LineTo(EdgeSource& source, const Point& p, State& state) {
  Point points[2];
  points[1] = p;

  Point& p0 = state.p0;
  Point& p1 = points[1];

  Point dummy_point;
  source.NextLineTo(dummy_point);

  // TODO:

  p0 = p1;
}

void EdgeBuilder::QuadTo(EdgeSource& source, State& state) {
  Point points[3];
  Point& p0 = state.p0;
  Point& p1 = points[1];
  Point& p2 = points[2];
  source.NextQuadTo(p1, p2);

  // TODO:

  p0 = p2;
}

void EdgeBuilder::CubicTo(EdgeSource& source, State& state) {
  Point points[4];
  Point& p0 = state.p0;
  Point& p1 = points[1];
  Point& p2 = points[2];
  Point& p3 = points[3];
  source.NextCubicTo(p1, p2, p3);

  // TODO:

  p0 = p3;
}

void EdgeBuilder::ConicTo(EdgeSource& source, State& state) {
  Point points[3];
  Point& p0 = state.p0;
  Point& p1 = points[1];
  Point& p2 = points[2];
  double weight;
  source.NextConicTo(p1, p2, weight);

  // TODO:

  p0 = p2;
}

} // namespace rezero
