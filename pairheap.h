#ifndef PAIRHEAP_H
#define PAIRHEAP_H
#include "bigreedymatcher.h"
#include "matchingutils.h"
#include "metric.h"

using namespace std;

class PairHeap : public BiGreedyMatcher
{
public:
    PairHeap(bool lazy, bool presort):
        BiGreedyMatcher(), lazy(lazy), presort(presort) {}
    PairHeap(const Metric& metric, bool lazy, bool presort):
        BiGreedyMatcher(metric), lazy(lazy), presort(presort) {}
    string acronym() const;

    void solveInt(const vector<Point> &centers, Matching &M) override;
    void solveReal(const vector<DPoint> &centers, Matching &M) override;

private:
    bool lazy;
    bool presort;

    vector<Point> intCenters;
    vector<DPoint> realCenters;
    bool real;

    void solve(Matching& M) const;

    vector<int> centersSortedByDist(Point site, const vector<int> &cIds) const;
    vector<IdIntDist> centersSortedByIntDist(Point site, const vector<int>& cIds) const;
    vector<IdRealDist> centersSortedByRealDist(Point site, const vector<int> &cIds) const;
    int nearestCenter(const Point &p, const vector<int> &cIds) const;
    double neighborDist(int sId, const vector<SiteStruct> &sites) const;
    void updateNeighbor(int sId, vector<SiteStruct> &sites,
            const vector<int> &quotas, const vector<int> &cIds) const;
    min_heap<IdRealDist> rebuildHeap(vector<SiteStruct> &sites,
            const vector<int> &quotas, const vector<int> &sIds, const vector<int> &cIds) const;
    void reinsertPair(int sId, min_heap<IdRealDist> &Q, vector<SiteStruct> &sites,
            const vector<int> &quotas, const vector<int> &cIds) const;
    min_heap<IdRealDist> initHeap(const vector<SiteStruct>& sites) const;
    vector<SiteStruct> initSites(const vector<int> &cIds, const Matching &M) const;
    double realDist(const Point &site, int cId) const;
    int intDist(const Point &site, int cId) const;
    void solveLazy(Matching &M, DelSet &remCIds, vector<SiteStruct> &sites, min_heap<IdRealDist> &Q) const;
    void solveEager(Matching &M, DelSet &remCIds, vector<SiteStruct> &sites, min_heap<IdRealDist> &Q) const;
};

#endif // PAIRHEAP_H
