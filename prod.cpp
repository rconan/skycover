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

vector<int> initialize(vector<int> list_sizes, int diag) {
  vector<int> indices = zeros(list_sizes.size());
  
  for (int i=list_sizes.size()-1; i>=0; i--) {
    indices[i] = min(list_sizes[i]-1, diag);
    diag -= indices[i];

    if (diag == 0) { break; }
  }

  return indices;
}

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

int main() {
  vector<int> first = vrange(0, 4);
  vector<int> secnd = vrange(0, 8);
  vector<int> third = vrange(0, 3);
  vector<int> forth = vrange(0, 4);
  vector< vector<int> > lists;
  lists.push_back(first);
  lists.push_back(secnd);
  lists.push_back(third);
  lists.push_back(forth);

  pvector(first);
  pvector(secnd);
  pvector(third);
  pvector(forth);

  cout << endl;

  test(get_list_sizes(lists), 9);

  return 0;
}
