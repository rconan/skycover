#include "probe.h"
#include "polygon.h"
#include <vector>
#include <math.h>
#include <cmath>
#include <iostream>
using namespace std;


#define PI 3.1415926535

// Scale a vector v by magnitude m.
Point scale(Point v, double m) {
  Point scaled(v.x * m, v.y * m);
  return scaled;
}

Probe::Probe() { }

Probe::Probe(double angle, double _width, double length) {
  needs_transfer = false;
  
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

  add_pt(center.translate(origin, u));
  add_pt(center.translate(origin, scale(u, -1)));
  add_pt(polygon.points[1].translate(origin, scale(center, -m)));
  add_pt(polygon.points[0].translate(origin, scale(center, -m)));
}

// Probe::Probe(string _name, double near_edge_angle_deg, double far_edge_angle_deg, double axis_angle) {
//   name = _name;
//   Point origin_vector(0, 1);
// 
//   add_pt(origin_vector.rotate(near_edge_angle_deg * (PI / 180)));
//   add_pt(origin_vector.rotate(far_edge_angle_deg  * (PI / 180)));
//   add_pt(scale(polygon.points[1], .4));
//   add_pt(scale(polygon.points[0], .4));
// 
//   axis = origin_vector.rotate(axis_angle * (PI / 180));
// 
//   probe_coverage();
// }

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

double abs_angle_between_vectors(Point u, Point v) {
  return acos( ((u.x * v.x) + (u.y * v.y)) /
               (vector_length(v) * vector_length(u)) );
}

// Should return the angle that u must rotate to rest parallel to v.
double angle_between_vectors(Point u, Point v) {
  Point origin(0, 1);
  int mod = 1;

  double u_angle, v_angle;
  u_angle = abs_angle_between_vectors(origin, u);
  v_angle = abs_angle_between_vectors(origin, v);

  // cout << "point: (" << v.x << ", " << v.y << endl;
  // cout << "u_angle: " << u_angle << ", v_angle: " << v_angle << endl;

  int v_quadrant = quadrant(v);
  int u_quadrant = quadrant(u);

  // cout << "u_quadrant: " << u_quadrant << ", v_quadrant: " << v_quadrant << ", " << endl;

  if (u_quadrant == 1) {
    if (v_quadrant == 1 && u_angle < v_angle) {
      mod = -1;
    } else if (v_quadrant == 4) {
      mod = -1;
    }
  } else if (u_quadrant == 2) {
    if (v_quadrant == 2 && u_angle > v_angle) {
      mod = -1;
    } else if (v_quadrant == 1) {
      mod = -1;
    }
  } else if (u_quadrant == 3) {
    if (v_quadrant == 3 && u_angle > v_angle) {
      mod = -1;
    } else if (v_quadrant == 2) {
      mod = -1;
    }
  } else if (u_quadrant == 4) {
    if (v_quadrant == 4 && u_angle < v_angle) {
      mod = -1;
    } else if (v_quadrant == 3) {
      mod = -1;
    }
  }
  
  return abs_angle_between_vectors(u, v) * mod;
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

int quadrant(Point pt) {
  if (pt.x >= 0 && pt.y >= 0) { return 1; }
  if (pt.x <= 0 && pt.y >= 0) { return 2; }
  if (pt.x <= 0 && pt.y <= 0) { return 3; }
  return 4;
}

// Angle between probe's axis and the point represented as a vector
// from the origin.
// double Probe::angle_to_point(Point pt) {
//   return angle_between_vectors(pt, axis);
// }

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
    if (pt_quadrant == 2 && axis_angle < pt_angle) {
      mod = 1;
    } else if (pt_quadrant == 1) {
      mod = 1;
    }
  } else if (axis_quadrant == 3) {
    if (pt_quadrant == 3 && axis_angle < pt_angle) {
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

  // cout << "center: " << this->center.x << ", " << this->center.y << endl;

  // cout << "vector";
  // pivot.print("red");

  Polygon translated_poly = translate_poly(this->polygon, this->center, this->rotate_about);
  Polygon rotated_poly    = rotate_poly(translated_poly, theta);
  Polygon retranslated    = translate_poly(rotated_poly, this->rotate_about, this->center);

  Point origin(0, 0);
  
  Edge e(axis, pivot);
  Point u = scale(e.normal(), width/2);

  // cout << "before: " << retranslated.points[2].x << ", " << retranslated.points[2].y << endl;
  
  retranslated.points[2] = pivot.translate(origin, u);
  retranslated.points[3] = pivot.translate(origin, scale(u, -1));

  // cout << "after: " << retranslated.points[2].x << ", " << retranslated.points[2].y << endl;

  return retranslated;
}

bool Probe::can_cover(Star s) {
  // true if angle between axis and point is < 90 deg.
  return abs(angle_to_point(s.point())) <= 1.57079632679;
}

void Probe::probe_coverage() {
  Point origin(0, 0);

  coverable_area.add_pt(axis.rotate(-90 * (PI / 180)));
  coverable_area.add_pt(axis);
  coverable_area.add_pt(axis.rotate(90  * (PI / 180)));
  coverable_area.add_pt(scale(coverable_area.points[0], 0.4));
  coverable_area.add_pt(scale(coverable_area.points[2], 0.4));
}

vector<Point> circle_intersections(Point P0, double r0, Point P1, double r1) {
  double d = distance(P0, P1);
  vector<Point> intersections;

  if (d < (r0 + r1)) {
    double a = (pow(r0, 2) - pow(r1, 2) + pow(d, 2)) / (2 * d);
    double b = d - a;

    double h = sqrt(pow(r0, 2) - pow(a, 2));

    Point P2(P0.x + a * (P1.x - P0.x) / d,
             P0.y + a * (P1.y - P0.y) / d);

    double i1x = P2.x + h * (P1.y - P0.y) / d;
    double i1y = P2.y - h * (P1.x - P0.x) / d;
    double i2x = P2.x - h * (P1.y - P0.y) / d;
    double i2y = P2.y + h * (P1.x - P0.x) / d;

    Point i1(i1x, i1y);
    Point i2(i2x, i2y);

    intersections.push_back(i1);
    intersections.push_back(i2);
  }

  return intersections;
}

double Probe::track_distance(Star s) {
  Point origin(0, 0);
  
  vector<Point> range_extremes = circle_intersections(center, radius, origin, distance(origin, s.point()));

  // cout << "range extremes size: " << range_extremes.size() << endl;
  if (range_extremes.size() < 2) {
    return 0;
  }

  Point star_exit_range;
  if (angle_to_point(range_extremes[0]) < 0) {
    star_exit_range = range_extremes[0];
  } else {
    star_exit_range = range_extremes[1];
  }

  // cout << "range_extremes[0]: ";
  // range_extremes[0].print("black");
  // cout << "angle to range extremes[0]: " << angle_to_point(range_extremes[0]) << endl;

  // cout << "vector";
  // star_exit_range.print("black");
  // cout << "vector";
  // axis.print("black");
  // cout << "vector" << angle_to_point(star_exit_range) << endl;

  Point u(star_exit_range.x - axis.x, star_exit_range.y - axis.y);
  Point v(s.point().x - axis.x, s.point().y - axis.y);

  // v.print("red");
  // u.print("black");

  return abs(angle_between_vectors(v, u));
}

int Probe::track(Star s, double dist) {
  Point newpos(s.point().rotate(dist));

  // cout << "angle to newpos: " << angle_to_point(newpos) << endl;
  // cout << "track distance: "  << track_distance(s) << endl;

  if (angle_to_point(newpos) > track_distance(s)) {

    // cout << "beginning transfer" << endl;

    // if (abs(track_distance(s)) < dist) {
    // cerr << track_distance(s) << ", " << dist << endl;
    if (backward_transfers.size() == 0) {
      return -1;
    }

    // cout << "making transfer" << endl;

    Star transfer = backward_transfers[backward_transfer_idx];
    Point transfer_pt = transfer.point().rotate(dist);
    current_star = Star(transfer_pt.x, transfer_pt.y, transfer.r, transfer.bear);
  } else {
    Point spoint = s.point().rotate(dist);
    current_star = Star(spoint.x, spoint.y, s.r, s.bear);
  }

  return 0;
}

/**
   set up two angles:
       1. star to range
       2. axis to star
   if (1 is positive) and (2 is negative)
       if (1 < track_distance)
           then add that shit
 **/


vector<Point> Probe::get_range_extremes(Star s) {
  Point origin(0, 0);
  vector<Point> intersections = circle_intersections(center, radius, origin, distance(origin, s.point()));

  return intersections;
}

bool Probe::in_range(Star s) {
  vector<Point> range_extremes = get_range_extremes(s);

  if (range_extremes.size() < 2) {
    return false;
  }

  Point star_enter_range, star_exit_range;
  if (angle_between_vectors(range_extremes[0], axis) < 0) {
    star_exit_range  = range_extremes[0];
    star_enter_range = range_extremes[1];
  } else {
    star_exit_range  = range_extremes[1];
    star_enter_range = range_extremes[0];
  }

  // cout << "vector";
  // s.point().print("red");
  // cout << "vector";
  // star_enter_range.print("black");
  // cout << "vector";
  // star_exit_range.print("black");

  // cout << angle_between_vectors(s.point(), star_enter_range) <<
  //   ", " << angle_between_vectors(s.point(), star_exit_range) << endl;

  // cout << endl;

  if ( (angle_between_vectors(s.point(), star_enter_range) < 0)
       && (angle_between_vectors(s.point(), star_exit_range) > 0) ) {
    if (abs(angle_to_point(s.point())) < (15 * (PI / 180))) {
      return true;
    }
  }

  return false;
}

double Probe::distance_till_inrange(Star s) {
  vector<Point> range_extremes = get_range_extremes(s);

  Point star_enter_range;
  if (angle_between_vectors(range_extremes[0], axis) > 0) {
    star_enter_range = range_extremes[1];
  } else {
    star_enter_range = range_extremes[0];
  }

  return angle_between_vectors(s.point(), star_enter_range);
}

bool Probe::intersects_path_of(Star s) {
  return get_range_extremes(s).size() == 2;
}

void Probe::get_backward_transfers(vector<Star> stars, double track_dist, double maglim) {
  for (Star s : stars) {
    if (intersects_path_of(s)) {
      if (! in_range(s)) {
        if ( (distance_till_inrange(s) < track_dist) && (s.r <= maglim) ) {
          backward_transfers.push_back(s);
        }
      }
    }
  }
}
