#include <vector>
using namespace std;


class CombinationGenerator {
    public:

        CombinationGenerator();
        CombinationGenerator(vector< vector<int> > _lists);
        CombinationGenerator(vector<int> _list_sizes);
        CombinationGenerator(vector<int> _list_sizes, int _offset, int _maxiter);
        ~CombinationGenerator();

        int n;
        int current_iter;
        int maxiter;
        int done;
        vector<int> list_sizes;
        vector<int> indices;
        vector< vector<int> > lists;
        vector<int> next();
        void offset(int i);
};
