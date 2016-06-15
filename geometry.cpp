#include "point.h"
#include <math.h>


double vector_length(Point pt) {
  return sqrt( pow(pt.x, 2) + pow(pt.y, 2) );
}

// Should return the angle that u must rotate to rest parallel to v.
double angle_between_vectors(Point u, Point v) {
  return acos( ((u.x * v.x) + (u.y * v.y)) /
               (vector_length(v) * vector_length(u)) );
}

Point unit_circle_coord(double deg) {
  Point v(0, 1);
  return v.rotate(angle_between_vectors(v, v.rotate(deg)));
}
