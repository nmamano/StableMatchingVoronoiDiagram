#ifndef VORONOITREE_H
#define VORONOITREE_H
#include "matching.h"
#include "metric.h"
#include "matchingutils.h"
#include <vector>

using namespace std;

class VoronoiTree
{
public:
    VoronoiTree(Metric metric = Num(2)): metric(metric) {}

    void setMetric(Metric newMetric) {metric = newMetric;}
    Metric getMetric() {return metric;}

    vector<vector<int> > solve(const vector<NPoint> &centers, int n);

    vector<vector<int> > getDiagram(const vector<NPoint> &centers, int n);
private:
    Metric metric;
    int F;

    vector<int> getQuotas(int k) const;
};

#endif // VORONOITREE_H
