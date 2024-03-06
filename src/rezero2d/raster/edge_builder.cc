// Created by DONG Zhong on 2024/02/22.

#include "rezero2d/raster/edge_builder_impl.h"

#include <limits>

#include "rezero2d/base/logging.h"
#include "rezero2d/raster/flatten_utils.h"

namespace rezero {

EdgeBuilder::EdgeBuilder(EdgeStorage* edge_storage) : EdgeBuilder(edge_storage, Rect{}, 0.0) {}

EdgeBuilder::EdgeBuilder(EdgeStorage* edge_storage, const Rect& clipping_box, double tolerance)
    : edge_storage_(edge_storage), clipping_box_(clipping_box), tolerance_sq_(tolerance * tolerance),
      bounding_box_(Rect(std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max(),
                         std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::min())) {
  REZERO_DCHECK(clipping_box.IsValid());
}

EdgeBuilder::~EdgeBuilder() = default;

void EdgeBuilder::SetTolerance(double tolerance) {
  tolerance_sq_ = tolerance * tolerance;
}

void EdgeBuilder::Begin() {
  border_X0Y0_ = border_X1Y0_ = clipping_box_.min_x;
  border_X0Y1_ = border_X1Y1_ = clipping_box_.min_y;
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
        x0_coord = static_cast<std::int32_t>(p0.x);
        y0_coord = static_cast<std::int32_t>(p0.y);
        x1_coord = static_cast<std::int32_t>(p1.x);
        y1_coord = static_cast<std::int32_t>(p1.y);

        while (true) {
          if (y0_coord < y1_coord) {
            // Descending.
DescendingBegin:
            BeginDescending();
            current_edge_.Append(x0_coord, y0_coord);
            current_edge_.Append(x1_coord, y1_coord);
            bounding_box_.min_y = std::min(static_cast<std::int32_t>(bounding_box_.min_y), y0_coord);

            while (true) {
DescendingLoop:
              p0 = p1;
              p0_flags = p1_flags;

              if (!source.MaybeNextLineTo(p1)) {
                EndDescending();
                bounding_box_.max_y = std::max(static_cast<std::int32_t>(bounding_box_.max_y), y0_coord);
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
              x1_coord = static_cast<std::int32_t>(p1.x);
              y1_coord = static_cast<std::int32_t>(p1.y);

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
            bounding_box_.max_y = std::max(static_cast<std::int32_t>(bounding_box_.max_y), y0_coord);

            while (true) {
AscendingLoop:
              p0 = p1;
              p0_flags = p1_flags;

              if (!source.MaybeNextLineTo(p1)) {
                EndAscending();
                bounding_box_.min_y = std::min(static_cast<std::int32_t>(bounding_box_.min_y), y0_coord);
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
              x1_coord = static_cast<std::int32_t>(p1.x);
              y1_coord = static_cast<std::int32_t>(p1.y);
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
            x1_coord = static_cast<std::int32_t>(p1.x);
            y1_coord = static_cast<std::int32_t>(p1.y);
          }
        }
      }

BeforeClipEndPoint:
      clipped_start = p0;
      diff_01 = p1 - p0;
    } else {
      double border_y0;
      double border_y1;

RestartClipLoop:
      // Skip all lines that are out of clipping_box_ or at its border.
      if (p0_flags & std::uint32_t(Rect::OutSideFlags::kY0)) {
        while (true) {
          if (clipping_box_.min_y < p1.y) {
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

        border_y0 = clipping_box_.min_y;

        auto common_flags = p0_flags & p1_flags;
        if (common_flags) {
          border_y1 = std::min(clipping_box_.max_y, p1.y);
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
          if (clipping_box_.max_y > p1.y) {
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

        border_y0 = clipping_box_.max_y;

        auto common_flags = p0_flags & p1_flags;
        if (common_flags) {
          border_y1 = std::max(clipping_box_.min_y, p1.y);
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
        border_y0 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);

        while (true) {
          if (clipping_box_.min_x < p1.x) {
            break;
          }

          p0 = p1;

          if (!source.MaybeNextLineTo(p1)) {
            p0_flags = clipping_box_.CalculateOutFlags(p0);
            border_y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);
            if (border_y0 != border_y1) {
              AccumulateLeftBorder(border_y0, border_y1);
            }
            return;
          }
        }

        border_y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);
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
        border_y0 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);

        while (true) {
          if (clipping_box_.max_x > p1.x) {
            break;
          }

          p0 = p1;

          if (!source.MaybeNextLineTo(p1)) {
            p0_flags = clipping_box_.CalculateOutFlags(p0);
            border_y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);
            if (p0_flags != p1_flags) {
              AccumulateRightBorder(border_y0, border_y1);
            }
            return;
          }

          border_y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);

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

      diff_01 = p1 - p0;
      clipped_start = {clipping_box_.max_x, clipping_box_.max_y};

      switch (static_cast<Rect::OutSideFlags>(p0_flags)) {
        case Rect::OutSideFlags::kNone:
          clipped_start = p0;
          break;

        case Rect::OutSideFlags::kX0Y0:
          clipped_start.x = clipping_box_.min_x;
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y0:
          clipped_start.y = (clipped_start.x - p0.x) * diff_01.y / diff_01.x + p0.y;
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);

          if (clipped_start.y >= clipping_box_.min_y) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY0:
          clipped_start.y = clipping_box_.min_y;
          clipped_start.x = p0.x + (clipping_box_.min_y - p0.y) * diff_01.x / diff_01.y;
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);
          break;

        case Rect::OutSideFlags::kX0Y1:
          clipped_start.x = clipping_box_.min_x;
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y1:
          clipped_start.y = (clipped_start.x - p0.x) * diff_01.y / diff_01.x + p0.y;
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);

          if (clipped_start.y <= clipping_box_.max_y) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY1:
          clipped_start.y = clipping_box_.max_y;
          clipped_start.x = p0.x + (clipping_box_.max_y - p0.y) * diff_01.x / diff_01.y;
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);
          break;

        case Rect::OutSideFlags::kX0:
          clipped_start.x = clipping_box_.min_x;
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1:
          clipped_start.y = p0.y + (clipped_start.x - p0.x) * diff_01.y / diff_01.x;
          p0_flags = clipping_box_.CalculateOutFlags(clipped_start);
          break;
        default:
          break;
      }

      if (p0_flags) {
        border_y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);
        if (clipped_start.x <= clipping_box_.min_x) {
          AccumulateLeftBorder(border_y0, border_y1);
        } else if (clipped_start.x >= clipping_box_.max_x) {
          AccumulateRightBorder(border_y0, border_y1);
        }

        p0 = p1;
        p0_flags = p1_flags;
        continue;
      }

      border_y1 = std::clamp(clipped_start.y, clipping_box_.min_y, clipping_box_.max_y);
      if (border_y0 != border_y1) {
        if (clipped_start.x <= clipping_box_.min_x) {
          AccumulateLeftBorder(border_y0, border_y1);
        } else if (clipped_start.x >= clipping_box_.max_x) {
          AccumulateRightBorder(border_y0, border_y1);
        }
      }

      if (!p1_flags) {
        p0 = p1;
        p0_flags = 0;

        x0_coord = static_cast<std::int32_t>(clipped_start.x);
        y0_coord = static_cast<std::int32_t>(clipped_start.y);
        x1_coord = static_cast<std::int32_t>(p1.x);
        y1_coord = static_cast<std::int32_t>(p1.y);

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
      Point clipped_end(clipping_box_.max_x, clipping_box_.max_y);

      switch (static_cast<Rect::OutSideFlags>(p1_flags)) {
        case Rect::OutSideFlags::kX0Y0:
          clipped_end.x = clipping_box_.min_x;
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y0:
          clipped_end.y = p0.y + (clipped_end.x - p0.x) * diff_01.y / diff_01.x;
          if (clipped_end.y >= clipping_box_.min_y) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY0:
          clipped_end.y = clipping_box_.min_y;
          clipped_end.x = p0.x + (clipped_end.y - p0.y) * diff_01.x / diff_01.y;
          break;

        case Rect::OutSideFlags::kX0Y1:
          clipped_end.x = clipping_box_.min_x;
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1Y1:
          clipped_end.y = p0.y + (clipped_end.x - p0.x) * diff_01.y / diff_01.x;
          if (clipped_end.y <= clipping_box_.max_y) {
            break;
          }
          // [[fallthrough]]
        case Rect::OutSideFlags::kY1:
          clipped_end.y = clipping_box_.max_y;
          clipped_end.x = p0.x + (clipped_end.y - p0.y) * diff_01.x / diff_01.y;
          break;

        case Rect::OutSideFlags::kX0:
          clipped_end.x = clipping_box_.min_x;
          // [[fallthrough]]
        case Rect::OutSideFlags::kX1:
          clipped_end.y = p0.y + (clipped_end.x - p0.x) * diff_01.y / diff_01.x;
          break;
        default:
          break;
      }

      AddLineSegment(clipped_start, clipped_end);

      double clipped_p1y = std::clamp(p1.y, clipping_box_.min_y, clipping_box_.max_y);
      if (clipped_end.y != clipped_p1y) {
        if (clipped_end.x == clipping_box_.min_x) {
          AccumulateLeftBorder(clipped_end.y, clipped_p1y);
        } else {
          AccumulateRightBorder(clipped_end.y, clipped_p1y);
        }
      }
    }

    p0 = p1;
    p0_flags = p1_flags;
  } while (source.MaybeNextLineTo(p1));
}

void EdgeBuilder::QuadTo(EdgeSource& source, State& state) {
  // 2 extrams and 1 terminating '1.0' value.
  constexpr std::uint32_t kMaxTCount = 2 + 1;
  Point spline[kMaxTCount * 2 + 1];

  Point& p0 = state.p0;
  Point& p1 = spline[1];
  Point& p2 = spline[2];

  std::uint32_t& p0_flags = state.flags;

  source.NextQuadTo(p1, p2);

  while (true) {
    auto p1_flags = clipping_box_.CalculateOutFlags(p1);
    auto p2_flags = clipping_box_.CalculateOutFlags(p2);

    auto flags = p0_flags & p1_flags & p2_flags;
    if (flags) {
      bool end = false;

      while (true) {
        if (flags & std::uint32_t(Rect::OutSideFlags::kY0)) {
          while (true) {
            p0 = p2;
            if (!source.MaybeNextQuadTo(p1, p2)) {
              end = true;
              break;
            }

            if (p1.y > clipping_box_.min_y || p2.y > clipping_box_.min_y) {
              break;
            }
          }
        } else if (flags & std::uint32_t(Rect::OutSideFlags::kY1)) {
          while (true) {
            p0 = p2;
            if (!source.MaybeNextQuadTo(p1, p2)) {
              end = true;
              break;
            }

            if (p1.y < clipping_box_.max_y || p2.y < clipping_box_.max_y) {
              break;
            }
          }
        } else {
          double y0 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);

          if (flags & std::uint32_t(Rect::OutSideFlags::kX0)) {
            while (true) {
              p0 = p2;

              if (!source.MaybeNextQuadTo(p1, p2)) {
                end = true;
                break;
              }

              if (p1.x > clipping_box_.min_x || p2.x > clipping_box_.max_x) {
                break;
              }
            }

            double y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);
            AccumulateLeftBorder(y0, y1);
          } else {
            while (true) {
              p0 = p2;

              if (!source.MaybeNextQuadTo(p1, p2)) {
                end = true;
                break;
              }

              if (p1.x < clipping_box_.max_x || p2.x < clipping_box_.max_x) {
                break;
              }
            }

            double y1 = std::clamp(p0.y, clipping_box_.min_y, clipping_box_.max_y);
            AccumulateRightBorder(y0, y1);
          }
        }

        p0_flags = clipping_box_.CalculateOutFlags(p0);
        if (end) {
          return;
        }

        continue;
      }
    }

    spline[0] = p0;

    Point* spline_ptr = spline;
    Point* spline_end = spline_ptr;

    spline_end = QuadHelper::SplitQuadToSpline(spline, spline_ptr);

    if (spline_end == spline_ptr) {
      spline_end = spline_ptr + 2;
    }

    FlattenMonoQuad mono_curve(tolerance_sq_);

    flags = p0_flags | p1_flags | p2_flags;
    if (flags) {
      // Need clipping.
      do {
        EdgeDirection direction = (spline_ptr[0].y > spline_end[2].y) ?
                                      EdgeDirection::kAscending : EdgeDirection::kDescending;
        FlattenMonoCurveClipping<FlattenMonoQuad>(mono_curve, spline_ptr, direction);
      } while ((spline_ptr += 2) != spline_end);

      p0 = spline_end[0];
      p0_flags = p2_flags;
    } else {
      // No clipping.
      do {
        EdgeDirection direction = (spline_ptr[0].y > spline_end[2].y) ?
                                      EdgeDirection::kAscending : EdgeDirection::kDescending;
        FlattenMonoCurve<FlattenMonoQuad>(mono_curve, spline_ptr, direction);
      } while ((spline_ptr += 2) != spline_end);

      p0 = spline_end[0];
    }

    if (!source.MaybeNextQuadTo(p1, p2)) {
      return;
    }
  }
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
  std::int32_t x0_coord = static_cast<std::int32_t>(p0.x);
  std::int32_t y0_coord = static_cast<std::int32_t>(p0.y);
  std::int32_t x1_coord = static_cast<std::int32_t>(p1.x);
  std::int32_t y1_coord = static_cast<std::int32_t>(p1.y);

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

  bounding_box_.min_y = std::min(min_y, static_cast<std::int32_t>(bounding_box_.min_y));
  bounding_box_.max_y = std::max(max_y, static_cast<std::int32_t>(bounding_box_.max_y));

  AddCloseLine(static_cast<std::int32_t>(clipping_box_.min_x), y0,
               static_cast<std::int32_t>(clipping_box_.min_x), y1);
}

void EdgeBuilder::EmitRightBorder() {
  std::int32_t y0 = static_cast<std::int32_t>(border_X1Y0_);
  std::int32_t y1 = static_cast<std::int32_t>(border_X1Y1_);

  if (y0 == y1) {
    return;
  }

  auto min_y = std::min(y0, y1);
  auto max_y = std::max(y0, y1);

  bounding_box_.min_y = std::min(min_y, static_cast<std::int32_t>(bounding_box_.min_y));
  bounding_box_.max_y = std::max(max_y, static_cast<std::int32_t>(bounding_box_.max_y));

  AddCloseLine(static_cast<std::int32_t>(clipping_box_.max_x), y0,
               static_cast<std::int32_t>(clipping_box_.max_x), y1);
}

void EdgeBuilder::AddCloseLine(std::int32_t x0_coord, std::int32_t y0_coord,
                               std::int32_t x1_coord, std::int32_t y1_coord) {
  EdgeDirection direction;

  if (y0_coord < y1_coord) {
    bounding_box_.min_y = std::min(y0_coord, static_cast<std::int32_t>(bounding_box_.min_y));
    bounding_box_.max_y = std::max(y1_coord, static_cast<std::int32_t>(bounding_box_.max_y));
    direction = EdgeDirection::kDescending;
  } else {
    bounding_box_.min_y = std::min(y1_coord, static_cast<std::int32_t>(bounding_box_.min_y));
    bounding_box_.max_y = std::max(y0_coord, static_cast<std::int32_t>(bounding_box_.max_y));
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
