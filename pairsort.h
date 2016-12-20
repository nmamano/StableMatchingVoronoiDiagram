#ifndef ALLPAIRSORTING_H
#define ALLPAIRSORTING_H

#include "bigreedymatcher.h"
#include "matchingutils.h"

using namespace std;

class PairSort : public BiGreedyMatcher
{
public:
    PairSort(): BiGreedyMatcher() {}
    PairSort(const Metric& metric): BiGreedyMatcher(metric) {}

    string acronym() const { return "PS"; }
    void solveInt(const vector<Point> &centers, Matching& M) override;
    void solveReal(const vector<DPoint> &centers, Matching& M) override;


private:
    vector<Point> intCenters;
    vector<DPoint> realCenters;
    bool real;

    void solve(Matching &M);

    vector<Link> linksSortedByLength(const vector<Point> &remPoints, const vector<int> &remCIds, int n);
    static vector<Link> linksSortedByLengthL2(int n, const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds);
    static vector<Link> linksSortedByLengthIntDists(const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds, const Metric &metric);
    static vector<Link> linksSortedByLengthRealDists(const vector<DPoint> &centers, const vector<Point> &remPoints, const vector<int> &remCIds, const Metric &metric);

    static void processLinks(vector<Link> L, Matching &M, int numRemSites);
};

#endif // ALLPAIRSORTING_H
