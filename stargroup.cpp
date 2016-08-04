#include "stargroup.h"
#include "probe.h"
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;

#define PI 3.14159265350

StarGroup::StarGroup() { }

StarGroup::~StarGroup() { }

StarGroup::StarGroup(vector<Star> _stars) {
    stars = _stars;
    update_mags();
}

void StarGroup::add_star(Star *_star) {
  stars.push_back(*_star);
}

Star StarGroup::star_at(int idx) {
  return stars[idx];
}

void StarGroup::update_mags() {
  for(vector<Star>::iterator it = stars.begin(); it != stars.end(); ++it) {
    mags.push_back(it->r);
  }
}

int int_frequency(int x, vector<int> l) {
  int freq = 0;
  for (vector<int>::iterator it = l.begin(); it != l.end(); ++it) {
    if (*it == x) {
      freq += 1;
    }
  }

  return freq;
}

int StarGroup::wfsmag() {
  vector<int>::iterator mag;
  for (mag=mags.begin(); mag != mags.end(); mag++) {
    if (int_frequency(*mag, mags) >= 3) {
      return *mag;
    }
  }

  return 0;
}

int StarGroup::gdrmag() {
  if (int_frequency(mags[0], mags) == 4) { return mags[0]; }

  vector<int>::iterator mag;
  for (mag=mags.begin(); mag != mags.end(); mag++) {
    if (int_frequency(*mag, mags) == 1) {
      return *mag;
    }
  }

  return 0;
}

string StarGroup::magpair() {
    std::ostringstream res;
    res << wfsmag() << ":" << gdrmag();

    return res.str();
}

double angle180(double ang) {
  // cout << "ang: " << ang << endl;
  while (ang > 180)  { ang -= 360; }
  while (ang < -180) { ang += 360; }
  return ang;
}

int StarGroup::valid(int W, int G, int printflg) {
  vector<double> angles;
  Point origin(0, 1);
  double angle;

  //  if (abs(angle180(stars[0].bear - stars[3].bear)) < 90) {
  //    if (printflg) {
  //      for (Star s : stars) { s.print(); }
  //      cout << "failed 1/4 constraint: angle180(stars[0].bear - stars[3].bear) = " << angle180(stars[0].bear - stars[3].bear) << endl;
  //    }
  //    return 0;
  //  }
  //  if (angle180(stars[1].bear - stars[0].bear) < 45) {
  //    if (printflg) {
  //      for (Star s : stars) { s.print(); }
  //      cout << "failed 2/1 constraint" << endl;
  //    }
  //    return 0;
  //  }
  //  if (angle180(stars[2].bear - stars[1].bear) < 45) {
  //    if (printflg) {
  //      for (Star s : stars) { s.print(); }
  //      cout << "failed 3/2 constraint" << endl;
  //    }
  //    return 0;
  //  }
  //  if (angle180(stars[3].bear - stars[2].bear) < 45) {
  //    if (printflg) {
  //      for (Star s : stars) { s.print(); }
  //      cout << "failed 4/3 constraint" << endl;
  //    }
  //    return 0;
  //  }
  //
  //  // cout << "cablemin: " << stars[2].cablemin << ", cablemax: " << stars[1].cablemax << endl;
  //  if (angle180(stars[2].cablemin - stars[1].cablemax) < 0) {
  //    for (Star s : stars) { s.print(); }
  //    cout << "failed cable constraint" << endl;
  //    return 0;
  //  }

  for (Star s : stars) {
    angles.push_back(angle_between_vectors(origin, s.point()));
  }

  if (quadrant(stars[0].point()) == 2 && quadrant(stars[3].point()) == 2) {if (angles[0] < angles[3]) { return 0; }}
  if (quadrant(stars[0].point()) == 1 && quadrant(stars[3].point()) == 1) {if (angles[0] > angles[3]) { return 0; }}
  if (quadrant(stars[0].point()) == 1 && quadrant(stars[3].point()) == 2) {return 0;}

  if (quadrant(stars[0].point()) == 2 && quadrant(stars[1].point()) == 2) {if (angles[0] > angles[1]) { return 0; }}
  if (quadrant(stars[0].point()) == 3 && quadrant(stars[1].point()) == 3) {if (angles[0] > angles[1]) { return 0; }}
  if (quadrant(stars[0].point()) == 3 && quadrant(stars[1].point()) == 2) {return 0;}

  if (quadrant(stars[1].point()) == 3 && quadrant(stars[2].point()) == 3) {if (angles[1] > angles[2]) { return 0; }}
  if (quadrant(stars[1].point()) == 4 && quadrant(stars[2].point()) == 4) {if (angles[2] > angles[1]) { return 0; }}
  if (quadrant(stars[1].point()) == 4 && quadrant(stars[2].point()) == 3) {return 0;}

  if (quadrant(stars[2].point()) == 4 && quadrant(stars[3].point()) == 4) {if (angles[3] > angles[2]) { return 0; }}
  if (quadrant(stars[2].point()) == 1 && quadrant(stars[3].point()) == 1) {if (angles[3] > angles[2]) { return 0; }}
  if (quadrant(stars[2].point()) == 1 && quadrant(stars[3].point()) == 4) {return 0;}

  for (int i=0; i<stars.size(); i++) {
    for (int j=0; j<stars.size(); j++) {
      if (i == j) { continue; }

      if (stars[i].x == stars[j].x && stars[i].y == stars[j].y) {
        return 0;
      }
    }
  }
  
  double R_1 = stars[0].r;
  double R_2 = stars[1].r;
  double R_3 = stars[2].r;
  double R_4 = stars[3].r;

  if ( ((R_1 > G) || (R_2 > W) || (R_3 > W) || (R_4 > W))
       && ((R_1 > W) || (R_2 > G) || (R_3 > W) || (R_4 > W))
       && ((R_1 > W) || (R_2 > W) || (R_3 > G) || (R_4 > W))
       && ((R_1 > W) || (R_2 > W) || (R_3 > W) || (R_4 > G)) ) {
    return 0;
  }

  return 1;
}

void StarGroup::print() {
  std::cerr << endl;
    for ( Star s : stars ) {
      s.point().print("m");
    }
    std::cerr << endl;
}
