#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "prod.h"
#include "star.h"
using namespace std;

vector<int> zeros(int n) {
    int i;
    vector<int> result;

    for (i=0; i<n; i++) {
        result.push_back(0);
    }

    return result;
}

void pvector(vector<int> v) {
  int i;
  for (i=0; i<v.size(); i++) {
    printf("%d ", v[i]);
  }
  cout << endl;
}

void pdimvector(vector< vector<int> > dims) {
  for ( vector<int> dim : dims ) {
    pvector(dim);
  }
  cout << endl;
}

vector<int> vrange(int start, int end) {
  vector<int> result;
  for (int i=start; i<end; i++) {
    result.push_back(i);
  }

  return result;
}

int maxidx(vector<int> dim)   { return dim[0]; }
int curridx(vector<int> dim)  { return dim[1]; }
int startidx(vector<int> dim) { return dim[2]; }

void CombinationGenerator::initialize() {
  int lvl_remaining;

  lvl_remaining = lvl;
  for (int i=0; i<dims.size(); i++) {
    dims[i][1] = 0;
    dims[i][2] = 0;
  }

  for (int i=dims.size()-1; i>=0; i--) {
    dims[i][1] = min(maxidx(dims[i]), lvl_remaining);
    lvl_remaining -= curridx(dims[i]);

    if ( lvl_remaining == 0 ) { break; }
  }

  for (int i=0; i<dims.size(); i++) {
    dims[i][2] = curridx(dims[i]);
  }
}

vector< vector<int> > dimrange(vector< vector<int> > dims, int start, int stop) {
  vector< vector<int> > result;
  for (int i=start; i<stop; i++) {
    result.push_back(dims[i]);
  }
  return result;
}

vector<int> indices(vector< vector<int> > dims) {
  vector<int> result;
  for ( vector<int> dim : dims ) {
    result.push_back(curridx(dim));
  }
  return result;
}

int sum(vector<int> xs) {
  int res = 0;
  for ( int x : xs ) {
    res += x;
  }
  return res;
}

vector< vector<int> > diagonal_traverse(vector< vector<int> > dims, int lvl) {
  int i, j, ndims, headstop, tailstart, new2ndtolast_idx, lvl_remaining, lvl_existing;
  vector<int> current_dim, last, sndtolast;

  ndims     = dims.size();
  headstop  = ndims - 3;
  tailstart = ndims - 2;

  last      = dims[ndims-1];
  sndtolast = dims[ndims-2];
  if ( curridx(last) > 0
       && curridx(sndtolast) < maxidx(sndtolast)
       && curridx(sndtolast) < lvl ) {
    dims[ndims-1][1] -= 1;
    dims[ndims-2][1] += 1;
  } else {
    for (i=headstop; i>=0; i--) {
      current_dim = dims[i];
      if ( curridx(current_dim) < maxidx(current_dim)
           && curridx(current_dim) < lvl
           && sum(indices(dimrange(dims, 0, i+1))) < lvl ) {
        dims[i][1] += 1;

        i++;
        for (j=i; j<tailstart; j++) {
          dims[j][1] = 0;
        }

        new2ndtolast_idx = lvl - sum(indices(dimrange(dims, 0, headstop+1))) - maxidx(last);
        if (new2ndtolast_idx < 0) {
          new2ndtolast_idx = 0;
        } else if (new2ndtolast_idx > maxidx(sndtolast)) {
          lvl_remaining = new2ndtolast_idx - maxidx(sndtolast);
          new2ndtolast_idx -= lvl_remaining;
          for (int i=headstop; i>=0; i--) {
            dims[i][1] = min(maxidx(dims[i]), lvl_remaining);
            lvl_remaining -= curridx(dims[i]);

            if ( lvl_remaining == 0 ) { break; }
          }
        }
        dims[tailstart][1]   = new2ndtolast_idx;
        dims[tailstart+1][1] = lvl - sum(indices(dimrange(dims, 0, tailstart+1)));

        lvl_existing = sum(indices(dimrange(dims, 0, ndims)));
        if ( lvl_existing < lvl ) {
          lvl_remaining = lvl - lvl_existing;
          for (int i=dims.size()-1; i>=0; i--) {
            dims[i][1] = min(maxidx(dims[i]), lvl_remaining);
            lvl_remaining -= curridx(dims[i]);

            if ( lvl_remaining == 0 ) { break; }
          }
        }

        break;
      }
    }
  }

  return dims;
}

CombinationGenerator::CombinationGenerator(vector<int> list_sizes) {
  for ( int size : list_sizes ) {
    dims.push_back(vector<int> { size, 0, 0 });
  }
  lvl = 0;
  initialize();
  done = 0;
}

CombinationGenerator::~CombinationGenerator() { }

vector<int> CombinationGenerator::next() {
  vector<int> result, old_idxs, new_idxs;

  result = indices(dims);

  old_idxs = indices(dims);
  dims     = diagonal_traverse(dims, lvl);
  new_idxs = indices(dims);
  if ( equal(old_idxs.begin(), old_idxs.end(), new_idxs.begin()) ) {
    lvl += 1;

    initialize();

    new_idxs = indices(diagonal_traverse(dims, lvl));
    if ( equal(old_idxs.begin(), old_idxs.end(), new_idxs.begin()) ) {
      done = 1;
    }
  }

  return result;
}

vector<int> get_list_sizes(vector< vector<int> > lists) {
    int i;
    vector<int> result;

    for (i=0; i<lists.size(); i++) {
        result.push_back(lists[i].size());
    }

    return result;
}
