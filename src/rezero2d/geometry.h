// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_GEOMETRY_H_
#define REZERO_GEOMETRY_H_

#include <cstdint>

namespace rezero {

class Rect;

class Point {
 public:
  Point();
  ~Point();

  Point(double x_value, double y_value);

  Point(const Point& other);
  Point& operator=(const Point& other);

  Point(Point&& other);
  Point& operator=(Point&& other);

  bool operator==(const Point& other) const;
  bool operator!=(const Point& other) const;

  void Reset();
  void Reset(const Point& other);
  void Reset(double x_value, double y_value);

  double x = 0.0;
  double y = 0.0;
};

static inline Point operator-(const Point& self) { return Point{-self.x, -self.y}; }

static inline Point operator+(const Point& p, double a) { return Point{p.x + a, p.y + a}; }
static inline Point operator-(const Point& p, double a) { return Point{p.x - a, p.y - a}; }
static inline Point operator*(const Point& p, double a) { return Point{p.x * a, p.y * a}; }
static inline Point operator/(const Point& p, double a) { return Point{p.x / a, p.y / a}; }

static inline Point operator+(double a, const Point& p) { return Point{a + p.x, a + p.y}; }
static inline Point operator-(double a, const Point& p) { return Point{a - p.x, a - p.y}; }
static inline Point operator*(double a, const Point& p) { return Point{a * p.x, a * p.y}; }
static inline Point operator/(double a, const Point& p) { return Point{a / p.x, a / p.y}; }

static inline Point operator+(const Point& a, const Point& b) { return Point{a.x + b.x, a.y + b.y}; }
static inline Point operator-(const Point& a, const Point& b) { return Point{a.x - b.x, a.y - b.y}; }
static inline Point operator*(const Point& a, const Point& b) { return Point{a.x * b.x, a.y * b.y}; }
static inline Point operator/(const Point& a, const Point& b) { return Point{a.x / b.x, a.y / b.y}; }

class Rect {
 public:
  enum class OutSideFlags : std::uint32_t {
    kNone = 0,
    kX0 = 1 << 0,
    kX1 = 1 << 1,
    kY0 = 1 << 2,
    kY1 = 1 << 3,

    kX = kX0 | kX1,
    kY = kY0 | kY1,

    kX0Y0 = kX0 | kY0,
    kX0Y1 = kX0 | kY1,
    kX1Y0 = kX1 | kY0,
    kX1Y1 = kX1 | kY1,

    kXY = kX | kY,
  };

  Rect();
  ~Rect();

  Rect(double min_x, double min_y, double max_x, double max_y);
  Rect(const Point& min, const Point& max);

  Rect(const Rect& other);
  Rect& operator=(const Rect& other);

  Rect(Rect&& other);
  Rect& operator=(Rect&& other);

  bool IsValid() const;

  std::uint32_t CalculateX0OutFlags(const Point& p) const { return std::uint32_t(p.x < min_x) << 0; }
  std::uint32_t CalculateX1OutFlags(const Point& p) const { return std::uint32_t(p.x > max_x) << 1; }
  std::uint32_t CalculateY0OutFlags(const Point& p) const { return std::uint32_t(p.y < min_y) << 2; }
  std::uint32_t CalculateY1OutFlags(const Point& p) const { return std::uint32_t(p.y > max_y) << 3; }

  std::uint32_t CalculateXOutFlags(const Point& p) const { return CalculateX0OutFlags(p) | CalculateX1OutFlags(p); }
  std::uint32_t CalculateYOutFlags(const Point& p) const { return CalculateY0OutFlags(p) | CalculateY1OutFlags(p); }
  std::uint32_t CalculateOutFlags(const Point& p) const { return CalculateXOutFlags(p) | CalculateYOutFlags(p); }

  Rect Union(const Rect& other);

  double min_x;
  double min_y;
  double max_x;
  double max_y;
};

} // namespace rezero

#endif // REZERO_GEOMETRY_H_
