#include <stdio.h>
#include <vector>
#include "prod.h"
#include "star.h"
using namespace std;

/**
   The CombinationGenerator is an iterator that computes the outer product
   of a list of lists. To make it generic (aka not caring about types) it
   acts on the sizes of each list in the outer product computation. Upon
   being asked for the next iteration, it will return the indices of the
   elements in each sublist present in that iteration. You can then use
   this index list to get the correct elements.
   
   The algorithm works like this. Start with an index list of all zeros.
   When asked for the next iteration, return the index list. Then, to update
   the index list: start from the end of the list and look for a position
   where the index is less than the max index into that list (aka the index
   is not equal to the length of that list minus one). Increment that index
   and set all indices up to the end of the index list to zero.
**/

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
    printf("v[%d]: %d\n", i, v[i]);
  }
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
