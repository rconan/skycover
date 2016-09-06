#ifndef _POINT_H
#define _POINT_H

#include <string>

class Point {

 public:
  Point();
  Point(double _x, double _y);
  ~Point();

  double x;
  double y;

  Point translate(Point old_orig, Point new_orig);
  Point rotate(double theta);
  void print(std::string color);
  double length();
  Point normalize();

  bool operator < (Point b) {
    if (y != b.y)
      return y < b.y;
    return x < b.x;
  }
};

#endif
