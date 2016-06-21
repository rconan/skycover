#include <stdio.h>
#include <vector>
#include <iostream>
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
    cout << "{";
    pvector(dim);
    cout << "} ";
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

CombinationGenerator::CombinationGenerator(vector< vector<int> > _lists) {
    n = _lists.size();
    indices = zeros(n);
    lists = _lists;
}

CombinationGenerator::CombinationGenerator(vector<int> _list_sizes) {
    n = _list_sizes.size();
    indices = zeros(n);
    list_sizes = _list_sizes;

    done = 0;
    for (int i=0; i<_list_sizes.size(); i++) {
      if (_list_sizes[i] == 0) {
        done = 1;
      }
    }
}

CombinationGenerator::~CombinationGenerator() { }

vector<int> CombinationGenerator::next() {
    int i, j;
    vector<int> result;
    
    for (i=0; i<n; i++) {
        result.push_back(indices[i]);
    }

    for (i=n-1; i>-1; i--) {
        if (indices[i] < list_sizes[i] - 1) {
            indices[i] += 1;
            for (j=i+1; j<n; j++) {
                indices[j] = 0;
            }
            break;
        }
    }

    if (i == -1) { done = 1; }

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

int maxidx(vector<int> dim)   { return dim[0]; }
int curridx(vector<int> dim)  { return dim[1]; }
int startidx(vector<int> dim) { return dim[2]; }

vector< vector<int> > initialize(vector< vector<int> > dims, int lvl) {
  for (int i=dims.size()-1; i>=0; i--) {
    dims[i][1] = min(maxidx(dims[i]), lvl);
    lvl -= curridx(dims[i]);

    if ( lvl == 0 ) { break; }
  }

  for (int i=0; i<dims.size(); i++) {
    dims[i][2] = curridx(dims[i]);
  }

  return dims;
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
  int i, j, ndims, headstop, tailstart, new2ndtolast_idx;
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
           && sum(indices(dimrange(dims, 0, i))) < lvl ) {
        dims[i][1] += 1;

        i++;
        for (j=i; j<tailstart; j++) {
          dims[j][1] = startidx(dims[j]);
        }

        new2ndtolast_idx = lvl - sum(indices(dimrange(dims, 0, headstop+1))) - maxidx(last);
        if (new2ndtolast_idx < 0) {
          new2ndtolast_idx = 0;
        }
        dims[tailstart][1]   = new2ndtolast_idx;
        dims[tailstart+1][1] = lvl - sum(indices(dimrange(dims, 0, tailstart+1)));

        break;
      }
    }
  }

  return dims;
}

// void diagonal_traverse(vector<int> list_sizes, int diag) {
//   vector<int> start_idxs = initialize(list_sizes, diag);
// 
//   for (i=start_idxs[0]; i<min(list_sizes[0], diag); i++) {
//     for (j=start_idxs[1]; j<min(list_sizes[1], diag); j++) {
//       k = diag - i - j - list_sizes[3] + 1;
// 
//       if (k < 0) {
//         k = 0;
//       }
// 
//       for (l=diag - (i + j + k); l>=0; l--) {
//         if (k >= list_sizes[2]) {
//           break;
//         }
// 
//         cout << i << j << k << l << endl;
//         k++;
//       }
//     }
//   }
// }

/**
void test(vector<int> list_sizes, int diag) {
  int i, j, k, l;
  vector<int> start_idxs = initialize(list_sizes, diag);

  for (i=start_idxs[0]; i<min(list_sizes[0], diag); i++) {
    for (j=start_idxs[1]; j<min(list_sizes[1], diag); j++) {
      k = diag - i - j - list_sizes[3] + 1;

      if (k < 0) {
        k = 0;
      }

      for (l=diag - (i + j + k); l>=0; l--) {
        if (k >= list_sizes[2]) {
          break;
        }

        cout << i << j << k << l << endl;
        k++;
      }
    }
  }
}
**/

int main() {
  vector<int> first { 4, 0, 0 };
  vector<int> secnd { 2, 0, 0 };
  vector<int> third { 2, 0, 0 };
  vector<int> forth { 3, 0, 0 };
  vector< vector<int> > dims;
  dims.push_back(first);
  dims.push_back(secnd);
  dims.push_back(third);
  dims.push_back(forth);

  dims = initialize(dims, 7);

  for (int i=0; i<15; i++) {
    pvector(indices(dims));
    dims = diagonal_traverse(dims, 7);
  }

  return 0;
}
