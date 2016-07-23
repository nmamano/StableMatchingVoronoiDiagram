#ifndef ALLPAIRSORTING_H
#define ALLPAIRSORTING_H

#include "bigreedymatcher.h"
#include "matchingutils.h"

using namespace std;

class AllPairSorting : public BiGreedyMatcher
{
public:
    AllPairSorting(): BiGreedyMatcher() {}
    AllPairSorting(const Metric& metric): BiGreedyMatcher(metric) {}

    string acronym() const { return "APS"; }
    void solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const;

    static vector<Link> linksSortedByLength(int n, const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds, const Metric &metric);
    static vector<Link> linksSortedByLengthL2(int n, const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds);
    static vector<Link> linksSortedByLengthGenCase(const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds, const Metric &metric);

};

#endif // ALLPAIRSORTING_H
