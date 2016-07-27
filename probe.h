#ifndef _PROBE_H
#define _PROBE_H


#include <vector>
#include <string>
#include "point.h"
#include "polygon.h"
#include "star.h"
using namespace std;

class Probe {

 public:
  Probe();
  Probe(string _name, double near_edge_angle_deg, double far_edge_angle_deg, double axis_angle);
  Probe(double angle, double _width, double length);
  ~Probe();

  string name;
  Point center;
  Point rotate_about;
  double radius;
  double width;
  Polygon polygon;
  Polygon coverable_area;
  Point axis;
  Star current_star;
  vector<Star> backward_transfers;
  int backward_transfer_idx;
  bool needs_transfer;

  void add_pt(double _x, double _y);
  void add_pt(Point pt);
  Point get_pt(int idx);

  Polygon transform(Point pivot);
  double solve_theta_offset(Point pt);
  double angle_to_point(Point pt);
  double track_distance(Star s);
  int track(Star s, double dist);
  void get_backward_transfers(vector<Star> stars, double track_dist, double maglim);
  vector<Point> get_range_extremes(Star s);
  bool in_range(Star s);
  double distance_till_inrange(Star s);
  bool intersects_path_of(Star s);

  bool can_cover(Star s);
  void probe_coverage();
};

double distance(Point a, Point b);
double angle_between_vectors(Point a, Point b);
int quadrant(Point p);
vector<Point> circle_intersections(Point P0, double r0, Point P1, double r1);
Point scale(Point u, double m);

#endif
