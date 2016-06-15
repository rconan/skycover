#ifndef _PROBE_H
#define _PROBE_H


#include <vector>
#include "point.h"
#include "polygon.h"
#include "star.h"
using namespace std;

class Probe {

 public:
  Probe();
  Probe(double near_edge_angle_deg, double far_edge_angle_deg, double axis_angle);
  ~Probe();

  Point center;
  Point rotate_about;
  double radius;
  Polygon polygon;
  Polygon coverable_area;
  Point axis;

  void add_pt(double _x, double _y);
  void add_pt(Point pt);
  Point get_pt(int idx);

  Polygon transform(Point pivot);
  double solve_theta_offset(Point pt);
  double angle_to_point(Point pt);

  bool can_cover(Star s);
  void probe_coverage();
};

double distance(Point a, Point b);

#endif
