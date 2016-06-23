#ifndef _STAR_H
#define _STAR_H


#include "point.h"

class Star {

 public:
  Point  pt;
  double r;
  double x;
  double y;

  Star();
  Star(double x, double y, double _r);
  ~Star();

  int in_range(Point *o, double minrange, double maxrange);
  void print();
  Point point();
};

#endif
