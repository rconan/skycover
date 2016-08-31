#include "point.h"
#include "stargroup.h"
#include "probe.h"
#include "polygon.h"
#include <iostream>


double dotprod(Point a, Point b) {
  double sum = 0;
  sum += a.x * b.x;
  sum += a.y * b.y;

  return sum;
}

Point project(Polygon poly, Point axis) {
  int i;
  Point projection;

  projection.x = dotprod(poly.points[0], axis);
  projection.y = projection.x;

  vector<Edge> edges = poly.edges();
  for (i=0; i<poly.npoints; i++) {
    projection.x = min(dotprod(edges[i].a, axis), projection.x);
    projection.x = min(dotprod(edges[i].b, axis), projection.x);
    
    projection.y = max(dotprod(edges[i].a, axis), projection.y);
    projection.y = max(dotprod(edges[i].b, axis), projection.y);
  }

  return projection;
}

int between(double x, Point interval) {
  double a = min(interval.x, interval.y);
  double b = max(interval.x, interval.y);

  return a <= x && x <= b;
}

int intersecting(Point u, Point v) {
  int res = 0;

  if (between(u.x, v)) {
    res = 1;
  } else if (between(u.y, v)) {
    res = 1;
  } else if (between(v.x, u)) {
    res = 1;
  } else if (between(v.y, u)) {
    res = 1;
  }

  return res;
}

vector<Point> SAT_axes(Polygon poly) {
  vector<Edge> edges = poly.edges();
  vector<Point> axes;

  int i;
  for (i=0; i<poly.npoints; i++) {
    axes.push_back(edges[i].normal());
  }

  return axes;
}

int colliding(Polygon poly1, Polygon poly2) {
  Point poly1_projection, poly2_projection;
  
  vector<Point> axes = SAT_axes(poly1);
  vector<Point> poly2_axes = SAT_axes(poly2);

  vector<Point>::iterator it;
  for (it=poly2_axes.begin(); it!=poly2_axes.end(); it++) {
      axes.push_back(*it);
  }

  int res = 1;
  vector<Point>::iterator curr_axis;
  for (curr_axis=axes.begin(); curr_axis != axes.end(); ++curr_axis) {
    poly1_projection = project(poly1, *curr_axis);
    poly2_projection = project(poly2, *curr_axis);

    if (! intersecting(poly1_projection, poly2_projection)) {
      res = 0;
      break;
    }
  }

  return res;
}

int colliding_in_parts(vector<Polygon> probe1parts, vector<Polygon> probe2parts) {
  for (int i=0; i<probe1parts.size(); i++) {
    Polygon probe1part = probe1parts[i];
    for (int j=0; j<probe2parts.size(); j++) {
      Polygon probe2part = probe2parts[j];

      if (colliding(probe1part, probe2part)) {
        return 1;
      }
    }
  }

  for (Polygon probe1part : probe1parts) {
    for (Polygon probe2part : probe2parts) {
      if (colliding(probe1part, probe2part)) {
        return 1;
      }
    }
  }
  
  return 0;
}

bool star_is_obscured(Star s, Polygon obscuration) {
  if (obscuration.point_in_poly(s.point())) {
    return true;
  }

  return false;
}

bool config_is_obscured_with_current_stars(vector<Probe> probes, int obscuration_type) {
  Polygon obscuration = get_obscuration(obscuration_type);

  for (int i=0; i<probes.size(); i++) {
    if (star_is_obscured(probes[i].current_star, obscuration)) {
      return true;
    }
  }

  return false;
}

bool config_is_obscured(StarGroup group, vector<Probe> probes, int obscuration_type, int max_obscured) {
  Polygon obscuration = get_obscuration(obscuration_type);

  int obscured_probes = 0;
  for (int i=0; i<group.stars.size(); i++) {
    if (star_is_obscured(group.stars[i], obscuration)) {
      obscured_probes++;
    }
  }

  if (obscured_probes > max_obscured) {
    return true;
  }

  return false;
}

int has_collisions_in_parts(StarGroup group, vector<Probe> probes, int obscuration_type, int max_obscured) {
  Probe probe1 = probes[0];
  Probe probe2 = probes[probes.size()-1];

  vector<Polygon> probe1parts = probe1.transform_parts(group.stars[0].point());
  vector<Polygon> probe2parts = probe2.transform_parts(group.stars[group.stars.size()-1].point());

  for (Polygon probe1part : probe1parts) {
    for (Polygon probe2part : probe2parts) {
      if (colliding(probe1part, probe2part)) {
        return 1;
      }
    }
  }

  if (obscuration_type != 0) {
    if (config_is_obscured(group, probes, obscuration_type, max_obscured)) {
      return true;
    }
  }

  for (int i=0; i<probes.size()-1; i++) {
    probe1 = probes[i];
    probe2 = probes[i+1];

    probe1parts = probe1.transform_parts(group.stars[i].point());
    probe2parts = probe2.transform_parts(group.stars[i+1].point());

    for (Polygon probe1part : probe1parts) {
      for (Polygon probe2part : probe2parts) {
        if (colliding(probe1part, probe2part)) {
          return 1;
        }
      }
    }
  }

  return 0;
}

int has_collisions_with_current_stars(vector<Probe> probes, int obscuration_type) {
  Probe probe1 = probes[0];
  Probe probe2 = probes[probes.size()-1];

  vector<Polygon> probe1parts = probe1.transform_parts(probes[0].current_star.point());
  vector<Polygon> probe2parts = probe2.transform_parts(probes[probes.size()-1].current_star.point());

  for (Polygon probe1part : probe1parts) {
    for (Polygon probe2part : probe2parts) {
      if (colliding(probe1part, probe2part)) {
        // cout << "collision between first and last" << endl;
        return 1;
      }
    }
  }

  if (obscuration_type != 0) {
    if (config_is_obscured_with_current_stars(probes, obscuration_type)) {
      return true;
    }
  }

  for (int i=0; i<probes.size()-1; i++) {
    probe1 = probes[i];
    probe2 = probes[i+1];

    probe1parts = probe1.transform_parts(probes[i].current_star.point());
    probe2parts = probe2.transform_parts(probes[i+1].current_star.point());

    for (Polygon probe1part : probe1parts) {
      for (Polygon probe2part : probe2parts) {
        if (colliding(probe1part, probe2part)) {
          return 1;
        }
      }
    }
  }

  return 0;
}

int has_collisions(StarGroup group, vector<Probe> probes) {
  vector<Polygon> polygons;
  Probe prb;
  Star  star;
  int i;

  for (i=0; i<group.stars.size(); i++) {
    prb  = probes[i];
    star = group.stars[i];
    polygons.push_back(prb.transform(star.point()));
    // polygons[i].polyprint();
  }

  if (colliding(polygons[0], polygons[polygons.size()-1])) {
    return 1;
  }
  for (i=0; i<polygons.size()-1; i++) {
    if (colliding(polygons[i], polygons[i+1])) {
      return 1;
    }
  }

  // for (i=0; i<group.stars.size(); i++) {polygons[i].polyprint();}
  return 0;
}

