#include "probe.h"
#include "polygon.h"
#include <vector>
#include <math.h>
#include <cmath>
#include <iostream>
using namespace std;


#define PI 3.1415926535

double distance(Point a, Point b) {
  return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

// Scale a vector v by magnitude m.
Point scale(Point v, double m) {
  Point scaled(v.x * m, v.y * m);
  return scaled;
}

Probe::Probe() { }

Probe::Probe(double angle, double _width, double length) {
  Point origin(0, 0);
  Point origin_vector(0, 1);

  radius = length;
  width = _width;

  axis = origin_vector.rotate(angle * (PI / 180));
  center = axis;
  rotate_about = origin;
  Edge e(origin, center);
  Point u = scale(e.normal(), width/2);

  double m = length / distance(origin, center);

  rotate_about = origin;

  add_pt(center.translate(origin, u));
  add_pt(center.translate(origin, scale(u, -1)));
  add_pt(polygon.points[1].translate(origin, scale(center, -m)));
  add_pt(polygon.points[0].translate(origin, scale(center, -m)));
}

Probe::Probe(string _name, double near_edge_angle_deg, double far_edge_angle_deg, double axis_angle) {
  name = _name;
  Point origin_vector(0, 1);

  add_pt(origin_vector.rotate(near_edge_angle_deg * (PI / 180)));
  add_pt(origin_vector.rotate(far_edge_angle_deg  * (PI / 180)));
  add_pt(scale(polygon.points[1], .4));
  add_pt(scale(polygon.points[0], .4));

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

int quadrant(Point pt) {
  if (pt.x >= 0 && pt.y >= 0) { return 1; }
  if (pt.x <= 0 && pt.y >= 0) { return 2; }
  if (pt.x <= 0 && pt.y <= 0) { return 3; }
  return 4;
}

// Angle between probe's axis and the point represented as a vector
// from the origin.
double Probe::angle_to_point(Point pt) {
  Point origin(0, 1);
  int mod = -1;

  double axis_angle, pt_angle;
  axis_angle = angle_between_vectors(origin, axis);
  pt_angle   = angle_between_vectors(origin, pt);

  // cout << "axis_angle: " << axis_angle << ", pt_angle: " << pt_angle << endl;

  int pt_quadrant   = quadrant(pt);
  int axis_quadrant = quadrant(axis);

  // cout << "axis_quadrant: " << axis_quadrant << ", pt_quadrant: " << pt_quadrant << ", ";

  if (axis_quadrant == 1) {
    if (pt_quadrant == 1 && axis_angle < pt_angle) {
      mod = 1;
    } else if (pt_quadrant == 4) {
      mod = 1;
    }
  } else if (axis_quadrant == 2) {
    if (pt_quadrant == 2 && axis_angle > pt_angle) {
      mod = 1;
    } else if (pt_quadrant == 1) {
      mod = 1;
    }
  } else if (axis_quadrant == 3) {
    if (pt_quadrant == 3 && axis_angle > pt_angle) {
      mod = 1;
    } else if (pt_quadrant == 2) {
      mod = 1;
    }
  } else if (axis_quadrant == 4) {
    if (pt_quadrant == 4 && axis_angle < pt_angle) {
      mod = 1;
    } else if (pt_quadrant == 3) {
      mod = 1;
    }
  }

  Point u(pt.x - center.x, pt.y - center.y);
  
  return angle_between_vectors(u, scale(center, -1)) * mod;
}

Polygon Probe::transform(Point pivot) {
  double theta = angle_to_point(pivot);
  // cout << "axis: (" << axis.x << ", " << axis.y << "), pt: (" << pivot.x << ", " << pivot.y << ")" << endl;
  // cout << "rotating " << name << " " << theta << " radians" << endl;

  Polygon translated_poly = translate_poly(polygon, center, rotate_about);
  Polygon rotated_poly    = rotate_poly(translated_poly, theta);
  Polygon retranslated    = translate_poly(rotated_poly, rotate_about, center);

  Point origin(0, 0);

  Edge e(center, pivot);
  Point u = scale(e.normal(), width/2);

  // cout << "before: " << polygon.points[0].x << ", " << polygon.points[0].y << endl;
  
  retranslated.points[2] = pivot.translate(origin, u);
  retranslated.points[3] = pivot.translate(origin, scale(u, -1));

  // cout << "after: " << polygon.points[0].x << ", " << polygon.points[0].y << endl;
  
  return retranslated;
}

bool Probe::can_cover(Star s) {
  // cout << "distance: " << distance(s.point(), center) << ", radius: " << radius << endl;
  if (distance(s.point(), center) < radius) { return true; }
  return false;
}

void Probe::probe_coverage() {
  Point origin(0, 0);

  coverable_area.add_pt(axis.rotate(-90 * (PI / 180)));
  coverable_area.add_pt(axis);
  coverable_area.add_pt(axis.rotate(90  * (PI / 180)));
  coverable_area.add_pt(scale(coverable_area.points[0], 0.4));
  coverable_area.add_pt(scale(coverable_area.points[2], 0.4));
}
