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
  Probe(double _angle, double _padding, string slider_body_file,
               string slider_shaft_file, string baffle_tube_file);
  ~Probe();

  Point center;
  Point rotate_about;
  double radius;
  Point axis;
  Star current_star;
  Star base_star;
  vector<Star> backward_transfers;
  int backward_transfer_idx;
  bool needs_transfer;
  vector<Polygon> parts;
  Polygon SliderBody;
  Polygon SliderShaft;
  Polygon BaffleTube;
  Polygon Base_SliderBody;
  Polygon Base_SliderShaft;
  Polygon Base_BaffleTube;
  double angle;
  Point BaffleTubeCtr;
  Point SliderShaftFront;
  double distance_tracked;
  double padding;
  Star default_star;
  vector<Star> used_transfers;

  void add_pt(double _x, double _y);
  void add_pt(Point pt);
  Point get_pt(int idx);

  vector<Polygon> transform_parts(Point pivot);
  double angle_to_point(Point pt);
  double track_distance(Star s);
  int track(double dist);
  void get_backward_transfers(vector<Star> stars, double track_dist, double maglim);
  vector<Point> get_range_extremes(Star s);
  bool in_range(Star s);
  double distance_till_inrange(Star s);
  bool intersects_path_of(Star s);
  Polygon move_slider(Polygon slider, double theta, Point pivot, double baffle_sep);
  void position_baffle_tube(Point pt);
  void reset_parts();
  int backtrack(double dist);
};

double distance(Point a, Point b);
double angle_between_vectors(Point a, Point b);
int quadrant(Point p);
vector<Point> circle_intersections(Point P0, double r0, Point P1, double r1);
Point scale(Point u, double m);
Polygon graham_scan(vector<Polygon> polygons);
bool safe_distance_from_center(Star s);
Polygon get_gclef_obscuration();
Polygon get_m3_obscuration();
Polygon get_obscuration(int obscuration_type);
vector<string> split(const string &text, char sep);
Polygon load_poly(string filename);

#endif
