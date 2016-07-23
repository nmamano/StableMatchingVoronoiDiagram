#ifndef NEIGHBORHEURISTIC_H
#define NEIGHBORHEURISTIC_H

#include "bigreedymatcher.h"
using namespace std;

class NeighborHeuristic : public BiGreedyMatcher
{
public:
    NeighborHeuristic(): BiGreedyMatcher() {}
    NeighborHeuristic(const Metric& metric): BiGreedyMatcher(metric) {}

    string acronym() const { return "NH"; }

    void solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const;

private:
    int cIdIndexWithClosestNeighbor(const vector<int> &remCIds, const vector<double> &dists) const;
    int cIdIndexWithClosestNeighbor(const vector<Point> &centers, const vector<int> &remCIds, const vector<Point> &remPoints, const vector<int> &neighbors) const;
    int findIndexClosest(const Point &q, const vector<Point> &ps) const;
};

#endif // NEIGHBORHEURISTIC_H
