#ifndef GALESHAPLEY_H
#define GALESHAPLEY_H
#include <vector>

using namespace std;

class GaleShapley
{
public:
    GaleShapley();

    vector<int> solve(const vector<vector<double> > &studentDists,
                      const vector<vector<int>>& hospitalPrefs,
                      const vector<int>& quotas);
};

#endif // GALESHAPLEY_H
