#include "point.h"
#include "star.h"
#include <vector>
#include <string>
using namespace std;

class StarGroup {

 public:
  StarGroup();
  StarGroup(vector<Star> _stars);
  ~StarGroup();

  vector<Star> stars;
  vector<int>   mags;

  void add_star(Star _star);
  Star star_at(int idx);
  void update_mags();
  string magpair();
  int valid_for_phasing(double J);
  int valid(double W, double G);
  double wfsmag();
  double gdrmag();
  void print();
};
