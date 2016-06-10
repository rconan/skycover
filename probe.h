#ifndef _PROBE_H
#define _PROBE_H


#include <vector>
#include "point.h"
#include "polygon.h"
using namespace std;

class Probe {

 public:
  Probe();
  Probe(Point _center, double _radius, int _axis, Point _rotate_about);
  ~Probe();

  Point center;
  Point rotate_about;
  double radius;
  Polygon polygon;
  int axis;

  void add_pt(double _x, double _y);
  Point get_pt(int idx);

  Polygon transform(Point pivot);
  double solve_theta_offset(Point pt);
};

#endif
