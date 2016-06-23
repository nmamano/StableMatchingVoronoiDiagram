#ifndef FASTMATCHER_H
#define FASTMATCHER_H

#include "matchingutils.h"
#include <vector>

using namespace std;

class FastMatcher
{
public:
    FastMatcher(int n);
    vector<vector<int>> query(const vector<Point>& centers) const;
    void updateP(int n);
    void setEarlyCutoff(int cutoff) { earlyCutoff = cutoff; }
    int tailoredCutoff(int k) const;
private:
    int n;
    int earlyCutoff;
    vector<Point> P;

    void buildP();

    void innerLoop(int pi, int pj,
            const vector<int>& c_ids, const vector<Point>& centers,
            vector<vector<int>>& plane, vector<int>& quotas) const;

    void bruteForceMatching(vector<vector<int>> &plane, const vector<Point>& centers, vector<int> &quotas, int num_rem_points, int num_rem_centers) const;
    vector<Link> sortedRemLinks(const vector<vector<int>> &plane, const vector<Point>& centers, const vector<int> &quotas, int num_rem_points, int num_rem_centers) const;
};

#endif // FASTMATCHER_H
