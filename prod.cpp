#include <stdio.h>
#include <vector>
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

// vector<int> CombinationGenerator::next() {
//     int i, j;
//     vector<int> result;
//     
//     for (i=0; i<n; i++) {
//         result.push_back(lists[i][indices[i]]);
//     }
// 
//     for (i=n-1; i>-1; i--) {
//         if (indices[i] < lists[i].size() - 1) {
//             indices[i] += 1;
//             for (j=i+1; j<n; j++) {
//                 indices[j] = 0;
//             }
//             break;
//         }
//     }
// 
//     if (i == -1) { done = 1; }
// 
//     return result;
// }

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
