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

Point Point::translate(Point old_orig, Point new_orig) {
  double x_diff = new_orig.x - old_orig.x;
  double y_diff = new_orig.y - old_orig.y;

  Point res(x + x_diff, this->y + y_diff);

  return Point(x + x_diff, this->y + y_diff);
}

void Point::print(string color) {
  cout << "(" << x << ", " << y << ", '" << color << "')" << endl;
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
