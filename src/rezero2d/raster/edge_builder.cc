// Created by DONG Zhong on 2024/02/22.

#include "rezero2d/raster/edge_builder.h"

#include <limits>

#include "rezero2d/base/logging.h"

namespace rezero {

EdgeBuilder::EdgeBuilder(EdgeStorage* edge_storage) : EdgeBuilder(edge_storage, Rect{}, 0.0) {}

EdgeBuilder::EdgeBuilder(EdgeStorage* edge_storage, const Rect& clipping_box, double tolerance)
    : edge_storage_(edge_storage), clipping_box_(clipping_box), tolerance_(tolerance),
      bounding_box_(Rect(std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max(),
                         std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::min())) {
  REZERO_DCHECK(clipping_box.IsValid());
}

EdgeBuilder::~EdgeBuilder() = default;

void EdgeBuilder::SetTolerance(double tolerance) {
  tolerance_ = tolerance;
}

void EdgeBuilder::Begin() {
  border_X0Y0_ = border_X1Y0_ = clipping_box_.GetMin().GetX();
  border_X0Y1_ = border_X1Y1_ = clipping_box_.GetMin().GetY();
}

void EdgeBuilder::End() {
  FlushBorderAccumulators();
  ResetBorderAccumulators();

  auto bbox = edge_storage_->bounding_box_.Union(bounding_box_);
  edge_storage_->bounding_box_ = bbox;
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
    begin_point = state.p0;
    state.flags = clipping_box_.CalculateOutFlags(state.p0);

    while (true) {
      if (edge_source.IsLineTo()) {
        Point p;
        edge_source.NextLineTo(p);
        LineTo(edge_source, p, state);
      } else if (edge_source.IsQuadTo()) {
        QuadTo(edge_source, state);
      } else if (edge_source.IsCubicTo()) {
        CubicTo(edge_source, state);
      } else if (edge_source.IsConicTo()) {
        ConicTo(edge_source, state);
      } else if (edge_source.IsClose()) {
        Point dummy_point;
        edge_source.NextLineTo(dummy_point);
        LineTo(edge_source, begin_point, state);
      } else {
        break;
      }
    }
  }

  return true;
}

void EdgeBuilder::LineTo(EdgeSource& source, const Point& p, State& state) {
  Point points[2];
  points[1] = p;

  Point& p0 = state.p0;
  Point& p1 = points[1];

  std::uint32_t& p0_flags = state.flags;
  std::uint32_t p1_flags;

  Point clipped_start;
  Point diff_01;

  std::int32_t x0_coord;
  std::int32_t y0_coord;
  std::int32_t x1_coord;
  std::int32_t y1_coord;

  do {
    if (!p0_flags) {
      p1_flags = clipping_box_.CalculateOutFlags(p1);
      if (!p1_flags) {
        x0_coord = static_cast<std::int32_t>(p0.GetX());
        y0_coord = static_cast<std::int32_t>(p0.GetY());
        x1_coord = static_cast<std::int32_t>(p1.GetX());
        y1_coord = static_cast<std::int32_t>(p1.GetY());

        while (true) {
          if (y0_coord < y1_coord) {
            // Descending.
DescendingBegin:
            BeginDescending();
            current_edge_.Append(x0_coord, y0_coord);
            current_edge_.Append(x1_coord, y1_coord);
            bounding_box_.SetMinY(std::min(static_cast<std::int32_t>(bounding_box_.GetMin().GetY()), y0_coord));

            while (true) {
DescendingLoop:
              p0 = p1;
              p0_flags = p1_flags;

              if (!source.MaybeNextLineTo(p1)) {
                EndDescending();
                bounding_box_.SetMaxY(std::max(static_cast<std::int32_t>(bounding_box_.GetMax().GetY()), y0_coord));
                p0 = p1;
                return;
              }

              p1_flags = clipping_box_.CalculateOutFlags(p1);
              if (p1_flags) {
                EndDescending();
                goto BeforeClipEndPoint;
              }

              x0_coord = x1_coord;
              y0_coord = y1_coord;
              x1_coord = static_cast<std::int32_t>(p1.GetX());
              y1_coord = static_cast<std::int32_t>(p1.GetY());

              if (y0_coord > y1_coord) {
                EndDescending();
                goto AscendingBegin;
              }

              current_edge_.Append(x1_coord, y1_coord);
            }
            // Not reached.
          } else if (y0_coord > y1_coord) {
            // Ascending.
AscendingBegin:
            BeginAscending();
            current_edge_.Append(x0_coord, y0_coord);
            current_edge_.Append(x1_coord, y1_coord);
            bounding_box_.SetMaxY(std::max(static_cast<std::int32_t>(bounding_box_.GetMax().GetY()), y0_coord));

            while (true) {
AscendingLoop:
              p0 = p1;
              p0_flags = p1_flags;

              if (!source.MaybeNextLineTo(p1)) {
                EndAscending();
                bounding_box_.SetMinY(std::min(static_cast<std::int32_t>(bounding_box_.GetMin().GetY()), y0_coord));
                p0 = p1;
                return;
              }

              p1_flags = clipping_box_.CalculateOutFlags(p1);
              if (p1_flags) {
                EndAscending();
                goto BeforeClipEndPoint;
              }

              x0_coord = x1_coord;
              y0_coord = y1_coord;
              x1_coord = static_cast<std::int32_t>(p1.GetX());
              y1_coord = static_cast<std::int32_t>(p1.GetY());
              if (y0_coord < y1_coord) {
                EndAscending();
                goto DescendingBegin;
              }

              current_edge_.Append(x1_coord, y1_coord);
            }
            // Not reached.
          } else {
            // Horizontal.
            p0 = p1;
            p0_flags = p1_flags;

            if (!source.MaybeNextLineTo(p1)) {
              return;
            }

            p1_flags = clipping_box_.CalculateOutFlags(p1);
            if (p1_flags) {
              break;
            }

            x0_coord = x1_coord;
            y0_coord = y1_coord;
            x1_coord = static_cast<std::int32_t>(p1.GetX());
            y1_coord = static_cast<std::int32_t>(p1.GetY());
          }
        }
      }

BeforeClipEndPoint:
      clipped_start = p0;
      diff_01 = Point(p1.GetX() - p0.GetX(), p1.GetY() - p0.GetY());
    } else {
      double border_y0;
      double border_y1;

RestartClipLoop:
      // Skip all lines that are out of clipping_box_ or at its border.
      if (p0_flags & std::uint32_t(Rect::OutSideFlags::kY0)) {
        while (true) {
          if (clipping_box_.GetMin().GetY() < p1.GetY()) {
            break;
          }

          p0 = p1;
          if (!source.MaybeNextLineTo(p1)) {
            p0_flags = clipping_box_.CalculateOutFlags(p0);
            return;
          }
        }

        p0_flags = clipping_box_.CalculateOutFlags(p0);
        p1_flags = clipping_box_.CalculateOutFlags(p1);

        border_y0 = clipping_box_.GetMin().GetY();

        auto common_flags = p0_flags & p1_flags;
        if (common_flags) {
          border_y1 = std::min(clipping_box_.GetMax().GetY(), p1.GetY());
          if (common_flags & std::uint32_t(Rect::OutSideFlags::kX0)) {
            AccumulateLeftBorder(border_y0, border_y1);
          } else {
            AccumulateRightBorder(border_y0, border_y1);
          }

          p0 = p1;
          p0_flags = p1_flags;
          continue;
        }
      } else if (p0_flags & std::uint32_t(Rect::OutSideFlags::kY1)) {
        while (true) {
          if (clipping_box_.GetMax().GetY() > p1.GetY()) {
            break;
          }

          p0 = p1;
          if (!source.MaybeNextLineTo(p1)) {
            p0_flags = clipping_box_.CalculateOutFlags(p0);
            return;
          }
        }

        p0_flags = clipping_box_.CalculateOutFlags(p0);
        p1_flags = clipping_box_.CalculateOutFlags(p1);

        border_y0 = clipping_box_.GetMax().GetY();

        auto common_flags = p0_flags & p1_flags;
        if (common_flags) {
          border_y1 = std::max(clipping_box_.GetMin().GetY(), p1.GetY());
          if (common_flags & std::uint32_t(Rect::OutSideFlags::kX0)) {
            AccumulateLeftBorder(border_y0, border_y1);
          } else {
            AccumulateRightBorder(border_y0, border_y1);
          }

          p0 = p1;
          p0_flags = p1_flags;
          continue;
        }
      } else if (p0_flags & std::uint32_t(Rect::OutSideFlags::kX0)) {
        border_y0 = std::clamp(p0.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());

        while (true) {
          if (clipping_box_.GetMin().GetX() < p1.GetX()) {
            break;
          }

          p0 = p1;

          if (!source.MaybeNextLineTo(p1)) {
            p0_flags = clipping_box_.CalculateOutFlags(p0);
            border_y1 = std::clamp(p0.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());
            if (border_y0 != border_y1) {
              AccumulateLeftBorder(border_y0, border_y1);
            }
            return;
          }
        }

        border_y1 = std::clamp(p0.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());
        if (border_y0 != border_y1) {
          AccumulateLeftBorder(border_y0, border_y1);
        }

        p0_flags = clipping_box_.CalculateOutFlags(p0);
        p1_flags = clipping_box_.CalculateOutFlags(p1);

        if (p0_flags & p1_flags) {
          goto RestartClipLoop;
        }

        border_y0 = border_y1;
      } else { // p0_flags & std::uint32_t(Rect::OutSideFlags::kX1)
        border_y0 = std::clamp(p0.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());

        while (true) {
          if (clipping_box_.GetMax().GetX() > p1.GetX()) {
            break;
          }

          p0 = p1;

          if (!source.MaybeNextLineTo(p1)) {
            p0_flags = clipping_box_.CalculateOutFlags(p0);
            border_y1 = std::clamp(p0.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());
            if (p0_flags != p1_flags) {
              AccumulateRightBorder(border_y0, border_y1);
            }
            return;
          }

          border_y1 = std::clamp(p0.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());

          if (border_y0 != border_y1) {
            AccumulateRightBorder(border_y0, border_y1);
          }

          p0_flags = clipping_box_.CalculateOutFlags(p0);
          p1_flags = clipping_box_.CalculateOutFlags(p1);

          if (p0_flags & p1_flags) {
            goto RestartClipLoop;
          }

          border_y0 = border_y1;
        }
      }

      diff_01 = Point(p1.GetX() - p0.GetX(), p1.GetY() - p0.GetY());
      clipped_start = clipping_box_.GetMax();

      switch (static_cast<Rect::OutSideFlags>(p0_flags)) {
        case Rect::OutSideFlags::kNone:
          clipped_start = p0;
          break;

        case Rect::OutSideFlags::kX0Y0:
          clipped_start.SetX(clipping_box_.GetMin().GetX());
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y0:
          clipped_start.SetY((clipped_start.GetX() - p0.GetX()) * diff_01.GetY() / diff_01.GetX() + p0.GetY());
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);

          if (clipped_start.GetY() >= clipping_box_.GetMin().GetY()) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY0:
          clipped_start.SetY(clipping_box_.GetMin().GetY());
          clipped_start.SetX(p0.GetX() + (clipping_box_.GetMin().GetY() - p0.GetY()) * diff_01.GetX() / diff_01.GetY());
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);
          break;

        case Rect::OutSideFlags::kX0Y1:
          clipped_start.SetX(clipping_box_.GetMin().GetX());
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y1:
          clipped_start.SetY((clipped_start.GetX() - p0.GetX()) * diff_01.GetY() / diff_01.GetX() + p0.GetY());
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);

          if (clipped_start.GetY() <= clipping_box_.GetMax().GetY()) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY1:
          clipped_start.SetY(clipping_box_.GetMax().GetY());
          clipped_start.SetX(p0.GetX() + (clipping_box_.GetMax().GetY() - p0.GetY()) * diff_01.GetX() / diff_01.GetY());
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);
          break;

        case Rect::OutSideFlags::kX0:
          clipped_start.SetX(clipping_box_.GetMin().GetX());
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1:
          clipped_start.SetY(p0.GetY() + (clipped_start.GetX() - p0.GetX()) * diff_01.GetY() / diff_01.GetX());
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);
          break;
        default:
          break;
      }

      if (p0_flags) {
        border_y1 = std::clamp(p1.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());
        if (clipped_start.GetX() <= clipping_box_.GetMin().GetX()) {
          AccumulateLeftBorder(border_y0, border_y1);
        } else if (clipped_start.GetX() >= clipping_box_.GetMax().GetX()) {
          AccumulateRightBorder(border_y0, border_y1);
        }

        p0 = p1;
        p0_flags = p1_flags;
        continue;
      }

      border_y1 = std::clamp(clipped_start.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());
      if (border_y0 != border_y1) {
        if (clipped_start.GetX() <= clipping_box_.GetMin().GetX()) {
          AccumulateLeftBorder(border_y0, border_y1);
        } else if (clipped_start.GetX() >= clipping_box_.GetMax().GetX()) {
          AccumulateRightBorder(border_y0, border_y1);
        }
      }

      if (!p1_flags) {
        p0 = p1;
        p0_flags = 0;

        x0_coord = static_cast<std::int32_t>(clipped_start.GetX());
        y0_coord = static_cast<std::int32_t>(clipped_start.GetY());
        x1_coord = static_cast<std::int32_t>(p1.GetX());
        y1_coord = static_cast<std::int32_t>(p1.GetY());

        if (y0_coord == y1_coord) {
          continue;
        }

        if (y0_coord < y1_coord) {
          goto DescendingBegin;
        } else {
          goto AscendingBegin;
        }
      }
    }

    {
      // Calculate clipped_end...
      Point clipped_end(clipping_box_.GetMax().GetX(), clipping_box_.GetMax().GetY());

      switch (static_cast<Rect::OutSideFlags>(p1_flags)) {
        case Rect::OutSideFlags::kX0Y0:
          clipped_end.SetX(clipping_box_.GetMin().GetX());
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y0:
          clipped_end.SetY(p0.GetY() + (clipped_end.GetX() - p0.GetX()) * diff_01.GetY() / diff_01.GetX());
          if (clipped_end.GetY() >= clipping_box_.GetMin().GetY()) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY0:
          clipped_end.SetY(clipping_box_.GetMin().GetY());
          clipped_end.SetX(p0.GetX() + (clipped_end.GetY() - p0.GetY()) * diff_01.GetX() / diff_01.GetY());
          break;

        case Rect::OutSideFlags::kX0Y1:
          clipped_end.SetX(clipping_box_.GetMin().GetX());
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y1:
          clipped_end.SetY(p0.GetY() + (clipped_end.GetX() - p0.GetX()) * diff_01.GetY() / diff_01.GetX());
          if (clipped_end.GetY() <= clipping_box_.GetMax().GetY()) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY1:
          clipped_end.SetY(clipping_box_.GetMax().GetY());
          clipped_end.SetX(p0.GetX() + (clipped_end.GetY() - p0.GetY()) * diff_01.GetX() / diff_01.GetY());
          break;

        case Rect::OutSideFlags::kX0:
          clipped_end.SetX(clipping_box_.GetMin().GetX());
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1:
          clipped_end.SetY(p0.GetY() + (clipped_end.GetX() - p0.GetX()) * diff_01.GetY() / diff_01.GetX());
          break;
        default:
          break;
      }

      AddLineSegment(clipped_start, clipped_end);

      double clipped_p1y = std::clamp(p1.GetY(), clipping_box_.GetMin().GetY(), clipping_box_.GetMax().GetY());
      if (clipped_end.GetY() != clipped_p1y) {
        if (clipped_end.GetX() == clipping_box_.GetMin().GetX()) {
          AccumulateLeftBorder(clipped_end.GetY(), clipped_p1y);
        } else {
          AccumulateRightBorder(clipped_end.GetY(), clipped_p1y);
        }
      }
    }

    p0 = p1;
    p0_flags = p1_flags;
  } while (source.MaybeNextLineTo(p1));
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

void EdgeBuilder::BeginAscending() {
  current_edge_.Reset();
  current_edge_.direction = EdgeDirection::kAscending;
}

void EdgeBuilder::EndAscending() {
  if (current_edge_.IsValid()) {
    REZERO_DCHECK(current_edge_.direction == EdgeDirection::kAscending);

    auto band_id = edge_storage_->CalculateBandId(current_edge_.points.back().y);
    edge_storage_->bands[band_id].Append(current_edge_);
  }

  current_edge_.Reset();
}

void EdgeBuilder::BeginDescending() {
  current_edge_.Reset();
  current_edge_.direction = EdgeDirection::kDescending;
}

void EdgeBuilder::EndDescending() {
  if (current_edge_.IsValid()) {
    REZERO_DCHECK(current_edge_.direction == EdgeDirection::kDescending);

    auto band_id = edge_storage_->CalculateBandId(current_edge_.points.front().y);
    edge_storage_->bands[band_id].Append(current_edge_);
  }

  current_edge_.Reset();
}

void EdgeBuilder::AccumulateLeftBorder(double border_y0, double border_y1) {
  if (border_X0Y1_ == border_y0) {
    border_X0Y1_ = border_y1;
    return;
  }

  EmitLeftBorder();

  border_X0Y0_ = border_y0;
  border_X0Y1_ = border_y1;
}

void EdgeBuilder::AccumulateRightBorder(double border_y0, double border_y1) {
  if (border_X1Y1_ == border_y0) {
    border_X1Y1_ = border_y1;
    return;
  }

  EmitRightBorder();

  border_X1Y0_ = border_y0;
  border_X1Y1_ = border_y1;
}

void EdgeBuilder::AddLineSegment(const Point& p0, const Point& p1) {
  std::int32_t x0_coord = static_cast<std::int32_t>(p0.GetX());
  std::int32_t y0_coord = static_cast<std::int32_t>(p0.GetY());
  std::int32_t x1_coord = static_cast<std::int32_t>(p1.GetX());
  std::int32_t y1_coord = static_cast<std::int32_t>(p1.GetY());

  if (x0_coord == x1_coord) {
    return;
  }

  AddCloseLine(x0_coord, y0_coord, x1_coord, y1_coord);
}

void EdgeBuilder::FlushBorderAccumulators() {
  EmitLeftBorder();
  EmitRightBorder();
}

void EdgeBuilder::ResetBorderAccumulators() {
  border_X0Y0_ = border_X0Y1_;
  border_X1Y0_ = border_X1Y1_;
}

void EdgeBuilder::EmitLeftBorder() {
  std::int32_t y0 = static_cast<std::int32_t>(border_X0Y0_);
  std::int32_t y1 = static_cast<std::int32_t>(border_X0Y1_);

  if (y0 == y1) {
    return;
  }

  auto min_y = std::min(y0, y1);
  auto max_y = std::max(y0, y1);

  bounding_box_.SetMinY(std::min(min_y, static_cast<std::int32_t>(bounding_box_.GetMin().GetY())));
  bounding_box_.SetMaxY(std::max(max_y, static_cast<std::int32_t>(bounding_box_.GetMax().GetY())));

  AddCloseLine(static_cast<std::int32_t>(clipping_box_.GetMin().GetX()), y0,
               static_cast<std::int32_t>(clipping_box_.GetMin().GetX()), y1);
}

void EdgeBuilder::EmitRightBorder() {
  std::int32_t y0 = static_cast<std::int32_t>(border_X1Y0_);
  std::int32_t y1 = static_cast<std::int32_t>(border_X1Y1_);

  if (y0 == y1) {
    return;
  }

  auto min_y = std::min(y0, y1);
  auto max_y = std::max(y0, y1);

  bounding_box_.SetMinY(std::min(min_y, static_cast<std::int32_t>(bounding_box_.GetMin().GetY())));
  bounding_box_.SetMaxY(std::max(max_y, static_cast<std::int32_t>(bounding_box_.GetMax().GetY())));

  AddCloseLine(static_cast<std::int32_t>(clipping_box_.GetMax().GetX()), y0,
               static_cast<std::int32_t>(clipping_box_.GetMax().GetX()), y1);
}

void EdgeBuilder::AddCloseLine(std::int32_t x0_coord, std::int32_t y0_coord,
                               std::int32_t x1_coord, std::int32_t y1_coord) {
  EdgeDirection direction;

  if (y0_coord < y1_coord) {
    bounding_box_.SetMinY(std::min(y0_coord, static_cast<std::int32_t>(bounding_box_.GetMin().GetY())));
    bounding_box_.SetMaxY(std::max(y1_coord, static_cast<std::int32_t>(bounding_box_.GetMax().GetY())));
    direction = EdgeDirection::kDescending;
  } else {
    bounding_box_.SetMinY(std::min(y1_coord, static_cast<std::int32_t>(bounding_box_.GetMin().GetY())));
    bounding_box_.SetMaxY(std::max(y0_coord, static_cast<std::int32_t>(bounding_box_.GetMax().GetY())));
    direction = EdgeDirection::kAscending;
  }

  EdgeVector edge;
  edge.direction = direction;
  edge.Append(x0_coord, y0_coord);
  edge.Append(x1_coord, y1_coord);

  auto band_id = edge_storage_->CalculateBandId(direction == EdgeDirection::kAscending ? y1_coord : y0_coord);
  edge_storage_->bands[band_id].Append(edge);
}

} // namespace rezero