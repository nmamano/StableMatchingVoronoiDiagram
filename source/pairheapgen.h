#ifndef PAIRHEAPGEN_H
#define PAIRHEAPGEN_H
#include "bigreedymatcher.h"
#include "matchingutils.h"
#include "metric.h"
#include "cpstruct.h"

using namespace std;

class PairHeapGen : public BiGreedyMatcher
{
public:
    PairHeapGen(const Metric& metric, bool lazy, string CPName):
        BiGreedyMatcher(metric), lazy(lazy), CPName(CPName) {}
    PairHeapGen(bool lazy, string CPName):
        BiGreedyMatcher(), lazy(lazy), CPName(CPName) {}

    string acronym() const;

    void solveInt(const vector<Point> &centers, Matching &M) override;
    void solveReal(const vector<DPoint> &centers, Matching &M) override;

private:
    bool lazy;
    CPStruct* CP;
    string CPName;

    void solve(Matching &M) const;
    vector<SiteStructGen> initSites(const Matching &M) const;
    min_heap<RealDistIdPair> initHeap(const vector<SiteStructGen> &sites) const;
    void solveLazy(Matching &M, vector<SiteStructGen> &sites, min_heap<RealDistIdPair> &Q) const;
    void solveEager(Matching &M, vector<SiteStructGen> &sites, min_heap<RealDistIdPair> &Q) const;
    min_heap<RealDistIdPair> rebuildHeap(vector<SiteStructGen> &sites, const vector<int> &quotas, const vector<int> &sIds) const;
};

#endif // PAIRHEAPGEN_H
