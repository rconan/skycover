#include "stargroup.h"
#include <iostream>
#include <sstream>
using namespace std;

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

int StarGroup::valid() {
  for (vector<int>::iterator it = mags.begin(); it != mags.end(); ++it) {
    if (int_frequency(*it, mags) >= 3) {
      return 1;
    }
  }
  return 0;
}

void StarGroup::print() {
    for ( Star s : stars ) {
        s.print();
    }
    std::cout << endl;
}
