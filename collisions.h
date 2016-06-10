double dotprod(Point a, Point b);
Point project(Polygon poly, Point axis);
int between(double x, Point interval);
int intersecting(Point u, Point v);
vector<Point> SAT_axes(Polygon poly);
int colliding(Polygon poly1, Polygon poly2);
int has_collisions(StarGroup group, vector<Probe> probes);
