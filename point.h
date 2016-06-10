#ifndef _POINT_H
#define _POINT_H

class Point {

 public:
  Point();
  Point(double _x, double _y);
  ~Point();

  double x;
  double y;

  Point translate(Point old_orig, Point new_orig);
  Point rotate(double theta);

  // move this to Vector class eventually
  Point normalize();
};

#endif
