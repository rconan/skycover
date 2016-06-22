#include <vector>
using namespace std;


class CombinationGenerator {
    public:

        CombinationGenerator(vector< vector<int> > _lists);
        CombinationGenerator(vector<int> _list_sizes);
        ~CombinationGenerator();

        int n;
        int done;
        vector<int> list_sizes;
        vector<int> indices;
        vector< vector<int> > lists;
        vector<int> next();
};
