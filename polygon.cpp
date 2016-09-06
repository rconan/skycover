#include "polygon.h"
#include <float.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;


Edge::Edge(Point _a, Point _b) {
  a = _a;
  b = _b;
}

Edge::Edge() { }
Edge::~Edge() { }

Point Edge::normal() {
  Point N;
  N.y = a.x - b.x;
  N.x = -1 * (a.y - b.y);

  return N.normalize();
}

Polygon::Polygon() {
  npoints = 0;
}

Polygon::~Polygon() { }

vector<Edge> Polygon::edges() {
  Edge e;
  vector<Edge> E;
  vector<Point>::iterator it;
  
  Edge e1 = Edge(points[0], points[npoints-1]);
  E.push_back(e1);
  for (it=points.begin(); it != prev(points.end()); it++) {
    e = Edge(*it, *(next(it)));
    E.push_back(e);
  }

  return E;
}

void Polygon::add_pt(double x, double y) {
  Point p(x, y);
  add_pt(p);
}

void Polygon::add_pt(Point pt) {
  points.push_back(pt);
  npoints += 1;

  if (npoints == 1) {
    minx = pt.x;
    miny = pt.y;
  } else {
    if (pt.x < minx) { minx = pt.x; }
    if (pt.y < miny) { miny = pt.y; }
  }
}

void Polygon::polyprint() {
  vector<Point>::iterator currpt;

  for (currpt=points.begin(); currpt!=points.end(); currpt++) {
    fprintf(stdout, "(%f, %f) ", currpt->x, currpt->y);
  }
  fprintf(stdout, "\n");
}

vector<double> Polygon::ycoords() {
  vector<double> result;

  for ( Point p : points ) {
    result.push_back(p.y);
  }

  return result;
}

vector<double> Polygon::xcoords() {
  vector<double> result;

  for ( Point p : points ) {
    result.push_back(p.x);
  }

  return result;
}

bool Polygon::point_in_poly(Point p) {
  int   i, j = npoints-1;
  bool  oddNodes = false;
  vector<double> ys = ycoords();
  vector<double> xs = xcoords();
  double x = p.x;
  double y = p.y;

  for (i=0; i<npoints; i++) {
    if ( (ys[i] < y && ys[j] >= y) || (ys[j] < y && ys[i] >= y) ) {
      if (xs[i] + (y - ys[i]) / (ys[j] - ys[i]) * (xs[j] - xs[i]) < x) {
        oddNodes = !oddNodes;
      }
    }
    j = i;
  }

  return oddNodes;
}

double Polygon::min_abs_y() {
  double result = DBL_MAX;

  for (Point pt : points) {
    if (abs(pt.y) < result) {
      result = pt.y;
    }
  }

  return result;
}

double Polygon::min_abs_x() {
  double result = DBL_MAX;

  for (Point pt : points) {
    if (pt.x < result) {
      result = pt.x;
    }
  }

  return result;
}

Polygon Polygon::rotate(double angle) {
  Polygon rotated_poly;
  
  for (Point pt : points) {
    rotated_poly.add_pt(pt.rotate(angle));
  }

  return rotated_poly;
}
