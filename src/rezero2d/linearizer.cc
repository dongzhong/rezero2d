// Created by DONG Zhong on 2024/02/15.

#include "rezero2d/linearizer.h"

#include <map>

#include "rezero2d/base/logging.h"

namespace rezero {

Linearizer::Linearizer() = default;

Linearizer::~Linearizer() = default;

std::pair<bool, Polygon> Linearizer::Linearize(const std::shared_ptr<Path>& path, double tolerance) {
  REZERO_CHECK(path);

  Polygon polygon;

  auto command_iter = path->commands_.begin();
  auto point_iter = path->points_.begin();

  auto curve_start_point_iter = point_iter;
  auto cuver_start_cmd_iter = command_iter;

  while (command_iter != path->commands_.end() || point_iter != path->points_.end()) {
    switch (*command_iter) {
      case Path::CommandType::kMove: {
        curve_start_point_iter = point_iter;
        cuver_start_cmd_iter = command_iter;
        polygon.NewContour();
        polygon.AppendPoint(*point_iter);
        break;
      }
      case Path::CommandType::kOnPath: {
        auto success = CheckAndLinearizeInternal(curve_start_point_iter, point_iter,
                                                 cuver_start_cmd_iter, command_iter,
                                                 tolerance, polygon);
        if (!success) {
          return { false, Polygon{} };
        }
        curve_start_point_iter = point_iter;
        cuver_start_cmd_iter = command_iter;
        break;
      }
      default: break;
    }

    ++command_iter;
    ++point_iter;
  }

  polygon.Validate();

  return { true, polygon };
}

bool Linearizer::CheckAndLinearizeInternal(const std::vector<Point>::iterator& begin_iter,
                                           const std::vector<Point>::iterator& end_iter,
                                           const std::vector<Path::CommandType>::iterator& begin_cmd_iter,
                                           const std::vector<Path::CommandType>::iterator& end_cmd_iter,
                                           double tolerance,
                                           Polygon& out_polygon) {
  auto point_count = end_iter - begin_iter;

  switch (point_count) {
    case 1: { // LineTo
      out_polygon.AppendPoint(*end_iter);
      return true;
    }
    case 2: { // QuadTo
      auto& p0 = *begin_iter;
      auto& p1 = *(begin_iter + 1);
      auto& p2 = *end_iter;
      if (*(begin_cmd_iter + 1) == Path::CommandType::kQuad) {
        return QuadTo(p0, p1, p2, tolerance, out_polygon);
      } else {
        return false;
      }
    }
    case 3: { // CubicTo or ConicTo
      auto& p0 = *begin_iter;
      auto& p1 = *(begin_iter + 1);
      auto& p2 = *(begin_iter + 2);
      auto& p3 = *end_iter;
      if (*(begin_cmd_iter + 1) == Path::CommandType::kCubic) {
        return CubicTo(p0, p1, p2, p3, tolerance, out_polygon);
      } else if (*(begin_cmd_iter + 1) == Path::CommandType::kConic) {
        return ConicTo(p0, p1, p3, p2.GetX(), tolerance, out_polygon);
      } else {
        return false;
      }
    }
    default:
      return false;
  }
}

namespace {

class QuadFunction {
 public:
  QuadFunction(const Point& p0, const Point& p1, const Point& p2) {
    // result = A * t^2 + B * t + C
    C_ = p0;
    B_ = { p1.GetX() + p1.GetX() - p0.GetX() - p0.GetX(),
           p1.GetY() - p0.GetY() + p1.GetY() - p0.GetY() };
    A_ = { p0.GetX() - p1.GetX() - p1.GetX() + p2.GetX(),
           p0.GetY() - p1.GetY() - p1.GetY() + p2.GetY() };
  }

  Point Calculate(double t) {
    return { (A_.GetX() * t + B_.GetX()) * t + C_.GetX(),
             (A_.GetY() * t + B_.GetY()) * t + C_.GetY() };
  }

 private:
  Point A_;
  Point B_;
  Point C_;
};

} // namespace

bool Linearizer::QuadTo(const Point& p0, const Point& p1, const Point& p2,
                        double tolerance, Polygon& out_polygon) {
  QuadFunction quad_function(p0, p1, p2);
  double tolerance_2 = tolerance * tolerance;

  std::map<double, Point> points;
  points[0.0] = quad_function.Calculate(0.0);
  points[1.0] = quad_function.Calculate(1.0);

  auto iter = points.begin();
  auto next = iter;
  ++next;

  for (; next != points.end(); iter = next, ++next) {
    double mid_t;
    Point mid_point;
    bool need_subdivision = false;

    do {
      mid_t = (iter->first + next->first) * 0.5;
      mid_point = quad_function.Calculate(mid_t);

      auto lx = mid_point.GetX() - (iter->second.GetX() + next->second.GetX()) * 0.5;
      auto ly = mid_point.GetY() - (iter->second.GetY() + next->second.GetY()) * 0.5;
      need_subdivision = lx * lx + ly * ly > tolerance_2;

      if (need_subdivision) {
        next = points.insert(iter, { mid_t, mid_point });
      }
    } while (need_subdivision);
  }

  for (auto&& [t, p] : points) {
    out_polygon.AppendPoint(p);
  }

  return true;
}

namespace {

class CubicFunction {
 public:
  CubicFunction(const Point& p0, const Point& p1, const Point& p2, const Point& p3) {
    // result = A * t^3 + B * t^2 + C * t + D;
    A_ = { -p0.GetX() + (p1.GetX() - p2.GetX()) * 3.0 + p3.GetX(),
           -p0.GetY() + (p1.GetY() - p2.GetY()) * 3.0 + p3.GetY() };
    B_ = { (p0.GetX() - p1.GetX() - p1.GetX() + p2.GetX()) * 3.0,
           (p0.GetY() - p1.GetY() - p1.GetY() + p2.GetY()) * 3.0 };
    C_ = { (-p0.GetX() + p1.GetX()) * 3.0, (-p0.GetY() + p1.GetY()) * 3.0 };
    D_ = p0;
  }

  Point Calculate(double t) {
    return { ((A_.GetX() * t + B_.GetX()) * t + C_.GetX()) * t + D_.GetX(),
             ((A_.GetY() * t + B_.GetY()) * t + C_.GetY()) * t + D_.GetY() };
  }

 private:
  Point A_;
  Point B_;
  Point C_;
  Point D_;
};

} // namespace

bool Linearizer::CubicTo(const Point& p0, const Point& p1, const Point& p2,
                         const Point& p3, double tolerance, Polygon& out_polygon) {
  CubicFunction cubic_function(p0, p1, p2, p3);
  double tolerance_2 = tolerance * tolerance;

  std::map<double, Point> points;
  points[0.0] = cubic_function.Calculate(0.0);
  points[1.0] = cubic_function.Calculate(1.0);

  auto iter = points.begin();
  auto next = iter;
  ++next;

  for (; next != points.end(); iter = next, ++next) {
    double mid_t, quarter_t;
    Point mid_point, quarter_point;
    bool need_subdivision;
    bool inflection_checked = false;

    do {
      mid_t = (iter->first + next->first) * 0.5;
      mid_point = cubic_function.Calculate(mid_t);

      auto lx = mid_point.GetX() - (iter->second.GetX() + next->second.GetX()) * 0.5;
      auto ly = mid_point.GetY() - (iter->second.GetY() + next->second.GetY()) * 0.5;
      need_subdivision = lx * lx + ly * ly > tolerance_2;

      if (!need_subdivision && !inflection_checked) {
        // Check quater points
        quarter_t = (iter->first + mid_t) * 0.5;
        quarter_point = cubic_function.Calculate(quarter_t);

        lx = quarter_point.GetX() - (iter->second.GetX() + mid_point.GetX()) * 0.5;
        ly = quarter_point.GetY() - (iter->second.GetY() + mid_point.GetY()) * 0.5;
        need_subdivision = lx * lx + ly * ly > tolerance_2;

        if (need_subdivision) {
          inflection_checked = true;
        }
      }

      if (need_subdivision) {
        next = points.insert(iter, { mid_t, mid_point });
      }
    } while (need_subdivision);
  }

  for (auto&& [t, p] : points) {
    out_polygon.AppendPoint(p);
  }

  return true;
}

bool Linearizer::ConicTo(const Point& p0, const Point& p1, const Point& p2,
                         double weight, double tolerance, Polygon& out_polygon) {
  // TODO:

  return true;
}

} // namespace rezero
