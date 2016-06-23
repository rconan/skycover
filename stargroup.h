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

  void add_star(Star *_star);
  Star star_at(int idx);
  void update_mags();
  string magpair();
  int valid(int W, int G);
  int wfsmag();
  int gdrmag();
  void print();
};
