// Created by DONG Zhong on 2024/02/06.

#ifndef REZERO_GEOMETRY_H_
#define REZERO_GEOMETRY_H_

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

class Rect {
 public:
  Rect();
  ~Rect();

  Rect(const Rect& other);
  Rect& operator=(const Rect& other);

  Rect(Rect&& other);
  Rect& operator=(Rect&& other);

 private:
  Point min_;
  Point max_;
};

} // namespace rezero

#endif // REZERO_GEOMETRY_H_
