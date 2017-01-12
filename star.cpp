#include "star.h"
#include "point.h"
#include <math.h>
#include <stdio.h>


Star::Star() { }

Star::Star(double _x, double _y, double _r, double _j, double _bear) {
  pt = Point(_x, _y);
  x  = pt.x;
  y  = pt.y;
  r  = _r;
  j  = _j;
  bear  = _bear;
}

Star::~Star() { }

int Star::in_range(Point *o, double minrange, double maxrange) {
  double distance = sqrt( pow(o->x - pt.x, 2) + pow(o->y - pt.y, 2) );
  if (minrange < distance && distance < maxrange) {
    return 1;
  } else {
    return 0;
  }
}

Point Star::point() {
    return Point(x, y);
}

void Star::print() {
    fprintf(stderr, "x: %f, y: %f, r: %f, j: %f\n", pt.x, pt.y, r, j);
}

Star Star::rotate(double dist) {
  Point p = point().rotate(dist);
  return Star(p.x, p.y, r, j, bear);
}

bool Star::equals(Star other) {
  if (x == other.x && y == other.y && r == other.r) {
    return true;
  }
  return false;
}
