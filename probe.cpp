#include "probe.h"
#include "polygon.h"
#include <vector>
#include <math.h>
#include <iostream>
using namespace std;


#define PI 3.1415926535

// Scale a vector v by magnitude m.
Point scale(Point v, double m) {
  Point scaled(v.x * m, v.y * m);
  return scaled;
}

Probe::Probe() { }

Probe::Probe(double near_edge_angle_deg, double far_edge_angle_deg, double axis_angle) {
  Point origin_vector(0, 1);

  add_pt(origin_vector.rotate(near_edge_angle_deg * (PI / 180)));
  add_pt(origin_vector.rotate(far_edge_angle_deg  * (PI / 180)));
  add_pt(scale(polygon.points[1], .3));
  add_pt(scale(polygon.points[0], .3));

  axis = origin_vector.rotate(axis_angle * (PI / 180));

  probe_coverage();
}

/**
Probe::Probe(Point _center, double _radius, int _axis, Point _rotate_about) {
  center = _center;
  radius = _radius;
  axis   = _axis;
  rotate_about = _rotate_about;
}
**/

Probe::~Probe(void) { }

void Probe::add_pt(Point pt) {
  polygon.points.push_back(pt);
}

void Probe::add_pt(double _x, double _y) {
  Point pt = Point(_x, _y);
  polygon.points.push_back(pt);
}

Point Probe::get_pt(int idx) {
  return polygon.points[idx];
}

double vector_length(Point pt) {
  return sqrt( pow(pt.x, 2) + pow(pt.y, 2) );
}

// Should return the angle that u must rotate to rest parallel to v.
double angle_between_vectors(Point u, Point v) {
  return acos( ((u.x * v.x) + (u.y * v.y)) /
               (vector_length(v) * vector_length(u)) );
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

// Angle between probe's axis and the point represented as a vector
// from the origin.
double Probe::angle_to_point(Point pt) {
  return angle_between_vectors(axis, pt);
}

/**
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
**/

Polygon Probe::transform(Point pivot) {
  Point origin = Point(0, 0);
  double theta = angle_to_point(pivot);

  Polygon translated_poly = translate_poly(this->polygon, this->center, this->rotate_about);
  Polygon rotated_poly    = rotate_poly(translated_poly, theta);
  Polygon retranslated    = translate_poly(rotated_poly, this->rotate_about, this->center);
  
  return retranslated;
}

bool Probe::can_cover(Star s) {
  return coverable_area.point_in_poly(s.point());
}

void Probe::probe_coverage() {
  Point origin(0, 0);

  coverable_area.add_pt(axis.rotate(-90 * (PI / 180)));
  coverable_area.add_pt(axis);
  coverable_area.add_pt(axis.rotate(90  * (PI / 180)));
  coverable_area.add_pt(scale(coverable_area.points[0], 0.3));
  coverable_area.add_pt(scale(coverable_area.points[2], 0.3));
}
