#ifndef BIGREEDYMATCHER_H
#define BIGREEDYMATCHER_H

#include <vector>
#include <string>
#include "point.h"
#include "dpoint.h"
#include "metric.h"
#include "matching.h"
using namespace std;

class BiGreedyMatcher
{
public:

    BiGreedyMatcher(): metric(Num(2)) {}
    BiGreedyMatcher(const Metric& metric): metric(metric) {}
    void setMetric(const Metric& metr) { metric = metr; }

    virtual string acronym() const =0;

    virtual void solveInt(const vector<Point> &centers, Matching& M) =0;
    virtual void solveReal(const vector<DPoint> &centers, Matching& M) =0;

protected:
    Metric metric;

};

#endif // BIGREEDYMATCHER_H
