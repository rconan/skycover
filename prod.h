#include <vector>
using namespace std;


class CombinationGenerator {
    public:

        CombinationGenerator(vector<int> _list_sizes);
        ~CombinationGenerator();

        vector< vector<int> > dims;
        int lvl;
        int done;
        vector<int> next();
        void initialize();
};
