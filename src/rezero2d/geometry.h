// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_GEOMETRY_H_
#define REZERO_GEOMETRY_H_

#include <vector>

namespace rezero {

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

  double GetX() const { return x_; }
  void SetX(double x_value);

  double GetY() const { return y_; }
  void SetY(double y_value);

 private:
  double x_ = 0.0;
  double y_ = 0.0;
};

class Size {
 public:
  Size();
  ~Size();

  Size(double w_value, double h_value);

  Size(const Size& other);
  Size& operator=(const Size& other);

  Size(Size&& other);
  Size& operator=(Size&& other);

  bool operator==(const Size& other) const;
  bool operator!=(const Size& other) const;

  void Reset();
  void Reset(const Size& other);
  void Reset(double w_value, double h_value);

  double GetWidth() const { return w_; }
  void SetWidth(double w_value);

  double GetHeight() const { return h_; }
  void SetHeight(double h_value);

 private:
  double w_ = 0.0;
  double h_ = 0.0;
};

class Rect {
 public:
  Rect();
  ~Rect();

  Rect(double x_value, double y_value, double w_value, double h_value);

  Rect(const Rect& other);
  Rect& operator=(const Rect& other);

  Rect(Rect&& other);
  Rect& operator=(Rect&& other);

  bool operator==(const Rect& other) const;
  bool operator!=(const Rect& other) const;

  void Reset();
  void Reset(const Rect& other);
  void Reset(double x_value, double y_value, double w_value, double h_value);

  double GetX() const { return x_; }
  void SetX(double x_value);

  double GetY() const { return y_; }
  void SetY(double y_value);

  double GetWidth() const { return w_; }
  void SetWidth(double w_value);

  double GetHeight() const { return h_; }
  void SetHeight(double h_value);

 private:
  double x_ = 0.0;
  double y_ = 0.0;
  double w_ = 0.0;
  double h_ = 0.0;
};

class Contour {
 public:
  Contour();
  ~Contour();

  Contour(const Contour& other);
  Contour& operator=(const Contour& other);

  Contour(Contour&& other);
  Contour& operator=(Contour&& other);

  void Reset();

  void AppendPoint(const Point& p);
  void AppendPoint(double x_value, double y_value);

  void AppendPoints(const std::vector<Point>& points);

  bool IsValid() const { return points_.size() >= 3; }

  const std::vector<Point>& GetPoints() const { return points_; }

 private:
  std::vector<Point> points_;
};

class Polygon {
 public:
  Polygon();
  ~Polygon();

  Polygon(const Polygon& other);
  Polygon& operator=(const Polygon& other);

  Polygon(Polygon&& other);
  Polygon& operator=(Polygon&& other);

  void Reset();

  void NewContour();

  void AppendPoint(const Point& point);

  void AppendPoints(const std::vector<Point>& points);

  void Validate();

  const std::vector<Contour>& GetContours() const { return contours_; }

 private:
  std::vector<Contour> contours_;
};

} // namespace rezero

#endif // REZERO_GEOMETRY_H_
