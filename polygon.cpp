#include "polygon.h"
#include <vector>
#include <iostream>
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
    printf("(%f, %f) ", currpt->x, currpt->y);
  }
  printf("\n");
}


