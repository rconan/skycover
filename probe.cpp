#include "probe.h"
#include "polygon.h"
#include <vector>
#include <math.h>
using namespace std;


Probe::Probe() { }

Probe::Probe(Point _center, double _radius, int _axis, Point _rotate_about) {
  center = _center;
  radius = _radius;
  axis   = _axis;
  rotate_about = _rotate_about;
}

Probe::~Probe(void) { }

void Probe::add_pt(double _x, double _y) {
  Point pt = Point(_x, _y);
  polygon.points.push_back(pt);
}

Point Probe::get_pt(int idx) {
  return polygon.points[idx];
}

Polygon translate_poly(Polygon polygon, Point from, Point to) {
  Polygon translated_poly;
  vector<Point>::iterator it;
  
  for(it = polygon.points.begin(); it != polygon.points.end(); it++) {
    translated_poly.add_pt(it->translate(from, to));
  }

  return translated_poly;
}

Polygon rotate_poly(Polygon poly, double theta) {
  Polygon rotated_poly;
  vector<Point>::iterator it;

  for(it = poly.points.begin(); it != poly.points.end(); ++it) {
    rotated_poly.add_pt(it->rotate(theta));
  }

  return rotated_poly;
}

double distance(Point a, Point b) {
  return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

double Probe::solve_theta_offset(Point pt) {
  double a, b, c;
  int mod = 1;
  Point A = Point(pt.x, center.y);

  if (axis == 0) {
    a = distance(A, center);
    b = distance(center, pt);
    c = distance(A, pt);

    if (polygon.points[0].x > 0) { mod = -1; }
  } else {
    a = distance(A, pt);
    b = distance(pt, center);
    c = distance(A, center);

    if (polygon.points[0].y < 0) { mod = -1; }
  }

  double cos_C = (a*a + b*b - c*c) / (2*a*b);

  // printf("theta: %f\n", acos(cos_C) * mod);

  return acos(cos_C) * mod;
}

Polygon Probe::transform(Point pivot) {
  Point origin = Point(0, 0);
  double theta = solve_theta_offset(pivot);

  Polygon translated_poly = translate_poly(this->polygon, this->center, this->rotate_about);
  Polygon rotated_poly    = rotate_poly(translated_poly, theta);
  Polygon retranslated    = translate_poly(rotated_poly, this->rotate_about, this->center);
  
  return retranslated;
}
