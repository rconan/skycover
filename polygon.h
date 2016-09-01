#include "point.h"
#include <vector>
#include <string>
using namespace std;


#ifndef _EDGE_H
#define _EDGE_H

class Edge {
 public:
  Edge();
  Edge(Point a, Point b);
  ~Edge();
  
  Point a, b;

  // Move this to the Vector class eventually
  Point normal();
};

#endif


#ifndef _POLYGON_H
#define _POLYGON_H

class Polygon {
 public:
  Polygon();
  ~Polygon();

  int npoints;
  void add_pt(double x, double y);
  void add_pt(Point pt);
  vector<Point> points;
  vector<Edge>  edges();

  double minx;
  double miny;

  void polyprint();
  bool point_in_poly(Point p);
  vector<double> ycoords();
  vector<double> xcoords();
  double min_abs_y();
  double min_abs_x();
  Polygon rotate(double angle);
};

#endif
