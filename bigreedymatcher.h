#ifndef BIGREEDYMATCHER_H
#define BIGREEDYMATCHER_H

#include <vector>
#include <string>
#include "point.h"
#include "metric.h"
using namespace std;

class BiGreedyMatcher
{
public:

    BiGreedyMatcher(): metric(Num(2)) {}
    BiGreedyMatcher(const Metric& metric): metric(metric) {}
    void setMetric(const Metric& metric);
    virtual void solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const =0;
    virtual string acronym() const =0;

protected:
    Metric metric;

    static vector<int> remainingCenterIds(const vector<int>& quotas);
    static vector<Point> remainingPoints(const vector<vector<int>>& plane, int numRemPoints);

};

#endif // BIGREEDYMATCHER_H
