#include "point.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
using namespace std;

Point::Point() { }

Point::Point(double _x, double _y) {
  x = _x;
  y = _y;
}

Point::~Point(void) { }

/**
   If you want to translate a point along a given vector, pass
   in (0, 0) as the old origin and your vector as the new origin.
**/
Point Point::translate(Point old_orig, Point new_orig) {
  double x_diff = new_orig.x - old_orig.x;
  double y_diff = new_orig.y - old_orig.y;

  Point res(x + x_diff, y + y_diff);

  return res;
}

void Point::print(string color) {
  cerr << "vector(" << x << ", " << y << ", '" << color << "')" << endl;
}

Point Point::rotate(double theta) {
  Point newpt;
  newpt.x = this->x*cos(theta) - this->y*sin(theta);
  newpt.y = this->x*sin(theta) + this->y*cos(theta);

  return newpt;
}

Point Point::normalize() {
  double length = sqrt(x*x + y*y);
  Point p(x / length, y / length);

  return p;
}

double Point::length() {
  return sqrt(x*x + y*y);
}
