// Created by DONG Zhong on 2024/02/15.

#include "rezero2d/linearizer.h"

#include <cmath>
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

namespace {

class ConicFunction {
 public:
  ConicFunction() = default;

  ConicFunction(const Point& p0, const Point& p1, const Point& p2, double weight)
      : p0_(p0), p1_(p1), p2_(p2), weight_(weight) {}

  std::vector<std::tuple<Point, Point, Point>> DivideAsQuads(double tolerance) {
    auto tolerance_2 = tolerance * tolerance;

    std::vector<ConicFunction> divided_conics;
    std::vector<bool> need_subdivisions;
    bool need_subdivision = false;

    divided_conics.push_back(*this);
    need_subdivisions.push_back(tolerance_2 < CalculateError2());

    do {
      std::vector<ConicFunction> divided_conics_res;
      std::vector<bool> need_subdivisions_res;

      for (std::uint32_t i = 0; i < divided_conics.size(); ++i) {
        if (need_subdivisions[i]) {
          ConicFunction out[2];
          divided_conics[i].DivideIntoTwo(out);

          divided_conics_res.push_back(out[0]);
          divided_conics_res.push_back(out[1]);

          bool need_subdivision_1 = tolerance_2 < out[0].CalculateError2();
          bool need_subdivision_2 = tolerance_2 < out[1].CalculateError2();
          need_subdivisions_res.push_back(need_subdivision_1);
          need_subdivisions_res.push_back(need_subdivision_2);
          need_subdivision = need_subdivision_1 || need_subdivision_2;
        } else {
          divided_conics_res.push_back(divided_conics[i]);
          need_subdivisions_res.push_back(false);
        }
      }

      if (need_subdivision) {
        divided_conics = divided_conics_res;
        need_subdivisions = need_subdivisions_res;
      }
    } while (need_subdivision);

    std::vector<std::tuple<Point, Point, Point>> quads;
    for (auto&& conic : divided_conics) {
      quads.emplace_back(conic.p0_, conic.p1_, conic.p2_);
    }
    return quads;
  }

 private:
  double CalculateError2() {
    auto a = weight_ - 1.0;
    auto k = a / (4.0 * (2.0 + a));
    auto lx = p0_.GetX() - p1_.GetX() - p1_.GetX() + p2_.GetX();
    auto ly = p0_.GetY() - p1_.GetY() - p1_.GetY() + p2_.GetY();
    return k * k * (lx * lx + ly * ly);
  }

  void DivideIntoTwo(ConicFunction* out) {
    auto weight_plus_1 = 1.0 + weight_;
    auto weight1 = std::sqrt(weight_plus_1 * 0.5);
    Point p11 = { (p0_.GetX() + weight_ * p1_.GetX()) / weight_plus_1,
                  (p0_.GetY() + weight_ * p1_.GetY()) / weight_plus_1 };
    Point p13 = { (weight_ * p1_.GetX() + p2_.GetX()) / weight_plus_1,
                  (weight_ * p1_.GetY() + p2_.GetY()) / weight_plus_1 };
    Point p12 = { (p11.GetX() + p13.GetX()) * 0.5,
                  (p11.GetY() + p13.GetY()) * 0.5 };
    out[0] = { p0_, p11, p12, weight1 };
    out[1] = { p12, p13, p2_, weight1 };
  }

  Point p0_;
  Point p1_;
  Point p2_;
  double weight_;
};

} // namespace

bool Linearizer::ConicTo(const Point& p0, const Point& p1, const Point& p2,
                         double weight, double tolerance, Polygon& out_polygon) {
  ConicFunction conic_function(p0, p1, p2, weight);

  auto quads_points = conic_function.DivideAsQuads(tolerance);

  bool success = true;
  for (auto&& quad_ps : quads_points) {
    success = QuadTo(std::get<0>(quad_ps), std::get<1>(quad_ps), std::get<2>(quad_ps), tolerance, out_polygon)
              && success;
  }

  return success;
}

} // namespace rezero
