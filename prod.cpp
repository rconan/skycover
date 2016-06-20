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
    cout << "v[" << i << "]: " << v[i] << " ";
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

CombinationGenerator::CombinationGenerator() { }

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

CombinationGenerator::CombinationGenerator(vector<int> _list_sizes, int _offset, int _maxiter) {
  maxiter = _maxiter;
  n = _list_sizes.size();
  indices = zeros(n);
  list_sizes = _list_sizes;
  current_iter = 0;

  done = 0;
  for (int i=0; i<_list_sizes.size(); i++) {
    if (_list_sizes[i] == 0) {
      done = 1;
    }
  }
  
  offset(_offset);
}

CombinationGenerator::~CombinationGenerator() { }

vector<int> slice(vector<int> data, int start, int end) {
  vector<int> result(data[start], data[end]);
  return result;
}

vector<int> compute_offset_indices(vector<int> lengths, int i) {
  int newi;
  vector<int> indices, recurse_result;

  if (lengths.size() == 0) { return indices; }
  if (i == 0) {
    return zeros(lengths.size());
  }
  
  int len = lengths[lengths.size()-1];
  indices.push_back(i % len);

  if (len > i) {
    newi = 0;
  } else {
    newi = i / len;
  }
  
  vector<int> newlengths;
  for (int j=0; j<lengths.size()-1; j++) {
    newlengths.push_back(lengths[j]);
  }

  recurse_result = compute_offset_indices(newlengths, newi);
  for ( int idx : recurse_result ) {
    indices.push_back(idx);
  }
  
  return indices;
}

void CombinationGenerator::offset(int i) {
  vector<int> offset_indices = compute_offset_indices(list_sizes, i);
  reverse(offset_indices.begin(), offset_indices.end());
  
  indices = offset_indices;
}

vector<int> CombinationGenerator::next() {
  if (current_iter >= maxiter) {
    done = 1;
    return vector<int>();
  }
  current_iter++;

  int i, j;
  vector<int> result;
    
  // n is 4 but indices.size() is 3, so we are getting data from one
  // element past the end of indices.
  for (i=0; i<n; i++) {
    result.push_back(indices[i]);
    // cout << "indices.size(): " << indices.size() << endl;
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

  // cout << "indices: ";
  // pvector(indices);
  // for (int k=0; k<indices.size(); k++) {
  //   if (indices[k] > list_sizes[k] || indices[k] < 0) {
  //     cout << "bad index in next" << endl;
  //     pvector(indices);
  //     cout << endl;
  //   }
  // }
  return result;
}

// int main() {
//   vector<int> first = vrange(1, 4);
//   vector<int> secnd = vrange(4, 7);
//   vector< vector<int> > lists;
//   lists.push_back(first);
//   lists.push_back(secnd);
// 
//   CombinationGenerator combos = CombinationGenerator(get_list_sizes(lists));
// 
//   vector<int> current = combos.next();
//   while (! combos.done) {
//       pvector(current);
//       current = combos.next();
//   }
//   pvector(current);
// 
//   return 0;
// }
