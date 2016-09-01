#include "probe.h"
#include "polygon.h"
#include <vector>
#include <math.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>
using namespace std;


#define PI 3.1415926535
#define SWEEPANGLE (30 * (PI / 180))
#define MINRANGE         (0.1   * 3600)
#define MAXRANGE         (0.167 * 3600)

std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

Point scale(Point v, double m) {
  Point scaled(v.x * m, v.y * m);
  return scaled;
}

Probe::Probe() { }

Polygon get_gclef_obscuration() {
  double width = 496;

  Point origin(0, 0);
  Point LevelOne(0, 1400);
  Point LevelTwo(0, -264);

  Edge  ObscurationEdge(origin, LevelOne);
  Point ObscurationWidthVector(scale(ObscurationEdge.normal(), width/2));

  Polygon Obscuration;
  Obscuration.add_pt(LevelOne.translate(origin, ObscurationWidthVector));
  Obscuration.add_pt(LevelTwo.translate(origin, ObscurationWidthVector));
  Obscuration.add_pt(LevelTwo.translate(origin, scale(ObscurationWidthVector, -1)));
  Obscuration.add_pt(LevelOne.translate(origin, scale(ObscurationWidthVector, -1)));

  for (int i=0; i<Obscuration.points.size(); i++) {
    Obscuration.points[i] = Obscuration.points[i].rotate(22.5 * (PI / 180));
  }

  return Obscuration;
}

Polygon get_m3_obscuration() {
  double levelonewidth = 601.93;
  double leveltwowidth = 80;

  Point origin(0, 0);
  Point LevelOne(0, 1200);
  Point LevelTwo(0, 96.75);

  Edge ObscurationEdge(origin, LevelOne);
  Point ObscurationWidthVectorA(scale(ObscurationEdge.normal(), levelonewidth/2));
  Point ObscurationWidthVectorB(scale(ObscurationEdge.normal(), leveltwowidth/2));

  Polygon Obscuration;
  Obscuration.add_pt(LevelOne.translate(origin, ObscurationWidthVectorA));
  Obscuration.add_pt(LevelTwo.translate(origin, ObscurationWidthVectorB));
  Obscuration.add_pt(LevelTwo.translate(origin, scale(ObscurationWidthVectorB, -1)));
  Obscuration.add_pt(LevelOne.translate(origin, scale(ObscurationWidthVectorA, -1)));

  for (int i=0; i<Obscuration.points.size(); i++) {
    Obscuration.points[i] = Obscuration.points[i].rotate(-28 * (PI / 180));
  }

  return Obscuration;
}

Polygon get_obscuration(int obscuration_type) {
  Polygon res;

  if (obscuration_type == 1) {
    res = get_m3_obscuration();
  } else {
    res = get_gclef_obscuration();
  }

  return res;
}

Polygon load_poly(string filename) {
  std::ifstream infile(filename);
  vector<string> points;
  string line;
  Polygon poly;

  for ( ; getline(infile, line); ) {
    vector<string> points = split(line, '\t');
    Point current_pt(stod(points[0]), stod(points[1]));

    poly.add_pt(current_pt);
  }

  return poly;
}

Point calculate_shaft_front(Polygon slider_shaft) {
  return Point(0, slider_shaft.min_abs_y());
}

Point calculate_baffle_ctr(Polygon baffle_tube) {
  return Point(0, baffle_tube.min_abs_y());
}

Probe::Probe(double _angle, double _padding, string slider_body_file,
             string slider_shaft_file, string baffle_tube_file) {
  angle   = _angle;
  padding = _padding;

  Base_SliderBody  = load_poly(slider_body_file).rotate(angle * (PI / 180));
  Base_SliderShaft = load_poly(slider_shaft_file).rotate(angle * (PI / 180));
  Base_BaffleTube  = load_poly(baffle_tube_file).rotate(angle * (PI / 180));

  SliderBody  = Base_SliderBody;
  SliderShaft = Base_SliderShaft;
  BaffleTube  = Base_BaffleTube;

  parts.push_back(SliderBody);
  parts.push_back(BaffleTube);
  parts.push_back(SliderShaft);

  // SliderShaftFront = calculate_shaft_front(SliderShaft).rotate(angle * (PI / 180));
  // BaffleTubeCtr    = calculate_baffle_ctr(BaffleTube).rotate(angle * (PI / 180));

  SliderShaftFront = Point(0, 115.6).rotate(angle * (PI / 180));
  BaffleTubeCtr = Point(0, 409).rotate(angle * (PI / 180));

  Point origin_vector(0, 1000);
  
  axis = scale(origin_vector.rotate(angle * (PI / 180)), 1.300);
  center = axis;

  radius = 1300;

  needs_transfer = false;

  Point origin(0, 0);
  rotate_about = origin;
}

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
  int mod = -1;

  double u_angle, v_angle;
  u_angle = abs_angle_between_vectors(origin, u);
  v_angle = abs_angle_between_vectors(origin, v);

  int u_quadrant = quadrant(u);
  int v_quadrant = quadrant(v);

  if (u_quadrant == 1) {
    if (v_quadrant == 1 && u_angle < v_angle) {
      mod = 1;
    } else if (v_quadrant == 4) {
      mod = 1;
    }
  } else if (u_quadrant == 2) {
    if (v_quadrant == 2 && v_angle < u_angle) {
      mod = 1;
    }
  } else if (u_quadrant == 3) {
    if (v_quadrant == 3 && v_angle < u_angle) {
      mod = 1;
    }
  } else if (u_quadrant == 4) {
    if (v_quadrant == 4 && u_angle < v_angle) {
      mod = 1;
    } else if (v_quadrant == 3) {
      mod = 1;
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
double Probe::angle_to_point(Point pt) {
  Point origin(0, 1);
  int mod = -1;

  double axis_angle, pt_angle;
  axis_angle = abs_angle_between_vectors(origin, axis);
  pt_angle   = abs_angle_between_vectors(origin, pt);

  int pt_quadrant   = quadrant(pt);
  int axis_quadrant = quadrant(axis);

  if (axis_quadrant == 1) {
    if (pt_quadrant == 1 && axis_angle < pt_angle) {
      mod = 1;
    } else if (pt_quadrant == 4) {
      mod = 1;
    }
  } else if (axis_quadrant == 2) {
    if (pt_quadrant == 2 && pt_angle < axis_angle) {
      mod = 1;
    }
  } else if (axis_quadrant == 3) {
    if (pt_quadrant == 3 && pt_angle < axis_angle) {
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

  return abs_angle_between_vectors(u, scale(center, -1)) * mod;
}

/**
mm -> degrees -> arcminutes
(mm / 3600) * 60
**/

double baffle_separation(Point p) {
  Point origin;
  Point p_location_arcminutes((p.x / 3600) * 60, (p.y / 3600) * 60);
  double r = distance(p_location_arcminutes, origin);
  
  return 115.6 + 78.5 * pow(r/10, 2);
}

vector<Point> get_points(vector<Polygon> polygons) {
  vector<Point> res;
  for (Polygon poly : polygons) {
    for (Point pt : poly.points) {
      res.push_back(pt);
    }
  }

  return res;
}

Polygon Probe::move_slider(Polygon slider, double theta, Point pivot, double baffle_sep) {
  Point w = SliderShaftFront.translate(center, rotate_about).rotate(theta).translate(rotate_about, center);
  Point v = Point(pivot.x - w.x, pivot.y - w.y);

  int mod = 1;
  if (distance(w, center) < distance(pivot, center)) {
    mod = -1;
  }

  Point u = scale(v, (v.length() + (mod * baffle_sep)) / v.length());

  Polygon newslider;
  Point origin(0, 0);

  for (int i=0; i<slider.points.size(); i++) {
    newslider.add_pt(slider.points[i].translate(origin, u));
  }

  return newslider;
}

void Probe::position_baffle_tube(Point pt) {
  Point origin(0, 0);
  Point c = scale(center, -1);
  Point p = pt.translate(origin, c);
  double theta = angle_between_vectors(p, c);
  Point intersection = p.rotate(theta);

  double distance_from_center = distance(origin, c) - distance(origin, intersection);

  Point unit_axis = axis.normalize();
  for (int i=0; i<BaffleTube.points.size(); i++) {
    BaffleTube.points[i] = BaffleTube.points[i].translate(origin, scale(unit_axis, distance_from_center));
  }

  parts[1] = BaffleTube;
}

void Probe::reset_parts() {
  SliderBody = Base_SliderBody;
  SliderShaft = Base_SliderShaft;
  BaffleTube = Base_BaffleTube;

  parts.clear();

  parts.push_back(SliderBody);
  parts.push_back(BaffleTube);
  parts.push_back(SliderShaft);
}

vector<Polygon> Probe::transform_parts(Point pivot) {
  vector<Polygon> transformed_parts;
  Point origin(0, 0);
  
  double dist_star_from_center   = distance(pivot, origin);
  double dist_slider_from_center = dist_star_from_center + baffle_separation(pivot);

  position_baffle_tube(pivot);

  double theta = angle_to_point(pivot);

  for (Polygon part : parts) {
    Polygon translated_poly = translate_poly(part, center, rotate_about);
    Polygon rotated_poly    = rotate_poly(translated_poly, theta);
    Polygon retranslated    = translate_poly(rotated_poly, rotate_about, center);

    transformed_parts.push_back(retranslated);
  }


  transformed_parts[0] = move_slider(transformed_parts[0], theta, pivot, baffle_separation(pivot));
  transformed_parts[2] = move_slider(transformed_parts[2], theta, pivot, baffle_separation(pivot));

  reset_parts();

  return transformed_parts;
}

Polygon Probe::transform(Point pivot) {
  Point origin(0, 0);
  
  double theta = angle_to_point(pivot);

  Polygon translated_poly = translate_poly(polygon, center, rotate_about);
  Polygon rotated_poly    = rotate_poly(translated_poly, theta);
  Polygon retranslated    = translate_poly(rotated_poly, rotate_about, center);

  return retranslated;
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

  if (range_extremes.size() < 2) {
    return 0;
  }

  Point star_exit_range;
  if (angle_to_point(range_extremes[0]) < 0) {
    star_exit_range = range_extremes[0];
  } else {
    star_exit_range = range_extremes[1];
  }

  Point u(star_exit_range.x - axis.x, star_exit_range.y - axis.y);
  Point v(s.point().x - axis.x, s.point().y - axis.y);

  return abs_angle_between_vectors(v, u);
}

int partition(vector<Star>& stars, int p, int q, Point center, Star current_star)
{
  Point probe_vector = Point(current_star.x - center.x, current_star.y - center.y);
  Star x = stars[p];
  Point x_vector = Point(x.x - center.x, x.y - center.y);
  int i = p;
  int j;

  for(j=p+1; j<q; j++) {
    Point s_vector = Point(stars[j].x - center.x, stars[j].y - center.y);
    
    if (abs_angle_between_vectors(probe_vector, s_vector) >= abs_angle_between_vectors(probe_vector, x_vector)) {
      i = i+1;
      swap(stars[i], stars[j]);
    }

  }
  
  swap(stars[i], stars[p]);

  return i;
}

// Implementation of quicksort on Star objects. Sorts by magnitude.
void sort_by_transfer_distance(vector<Star>& stars, int p, int q, Point center, Star current_star)
{
    int r;
    if(p < q)
    {
      r = partition(stars, p, q, center, current_star);
      sort_by_transfer_distance(stars, p, r, center, current_star);  
      sort_by_transfer_distance(stars, r+1, q, center, current_star);
    }
}



int Probe::track(double dist) {
  if (! in_range(base_star.rotate(dist))) {

    sort_by_transfer_distance(backward_transfers, 0, backward_transfers.size(), center, current_star);

    bool found = false;
    for (int i=0; i<backward_transfers.size(); i++) {
      Star s = backward_transfers[i];

      if (in_range(s.rotate(dist))) {
        found = true;
        base_star = s;
        current_star  = s.rotate(dist);

        break;
      }
    }

    if (found == false) {
      return -1;
    }

  } else {
    current_star = base_star.rotate(dist);
  }

  distance_tracked = dist;

  return 0;
}

vector<Point> Probe::get_range_extremes(Star s) {
  Point origin(0, 0);
  vector<Point> intersections = circle_intersections(center, radius, origin, distance(origin, s.point()));

  return intersections;
}

bool safe_distance_from_center(Star star) {
  Point star_pt(star.x, star.y), origin(0, 0);
  
  double dist = distance(star_pt, origin);

  if (MINRANGE < dist && dist < MAXRANGE) {
    return true;
  } else {
    return false;
  }
}

bool Probe::in_range(Star s) {
  vector<Point> range_extremes = get_range_extremes(s);

  if (range_extremes.size() < 2) {
    return false;
  }

  if (distance(center, s.point()) > radius) {
    return false;
  }

  if (! safe_distance_from_center(s)) {
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

  if (abs(angle_to_point(s.point())) < SWEEPANGLE) {
    return true;
  }

  return false;
}

double Probe::distance_till_inrange(Star s) {
  vector<Point> range_extremes = get_range_extremes(s);

  Point star_enter_range;
  if (angle_between_vectors(range_extremes[0], axis) > 0) {
    star_enter_range = range_extremes[0];
  } else {
    star_enter_range = range_extremes[1];
  }

  return abs_angle_between_vectors(s.point(), star_enter_range);
}

bool Probe::intersects_path_of(Star s) {
  return get_range_extremes(s).size() == 2;
}

void Probe::get_backward_transfers(vector<Star> stars, double track_dist, double maglim) {
  for (Star s : stars) {
    if (s.r <= maglim) {
      backward_transfers.push_back(s);
    }
  }
}

vector<Star>::iterator find_star(vector<Star> starlist, Star s) {
  vector<Star>::iterator curr;
  for (curr=starlist.begin(); curr!=starlist.end(); curr++) {
    if (s.x == curr->x && s.y == curr->y && s.r == curr->r) {
      return curr;
    }
  }

  return curr;
}

bool member(Star s, vector<Star> list) {
  for (Star el : list) {
    if (s.equals(el)) {
      return true;
    }
  }

  return false;
}

int Probe::backtrack(double dist) {
  sort_by_transfer_distance(backward_transfers, 0, backward_transfers.size(), center, current_star);

  bool found = false;
  for (int i=0; i<backward_transfers.size(); i++) {
    Star s = backward_transfers[i];

    if (member(s, used_transfers)) {
      continue;
    }

    if (in_range(s.rotate(dist))) {
      found = true;
      base_star = s;
      current_star  = base_star.rotate(dist);

      used_transfers.push_back(s);

      break;
    }
  }

  if (found == false) {
    return -1;
  } else {
    return 0;
  }
}
