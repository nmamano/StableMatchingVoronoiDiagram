#ifndef MATCHING_H
#define MATCHING_H
#include <vector>
#include "point.h"
#include "metric.h"

using namespace std;

class Matching
{
public:
    vector<vector<int>> plane;
    vector<int> quotas;

    Matching(int n, int k);
    Matching(const Matching& M);

    int k() const { return quotas.size(); }
    int n() const { return plane.size(); }

    vector<int> centerQuotas() {
        return centerQuotas(n(), k());
    }
    static vector<int> centerQuotas(int n, int k);

    vector<int> remainingCIds() const;

    int numRemSites() const;
    vector<Point> remainingSites() const;

    vector<vector<Point>> pointsByCenter() {
        return pointsByCenter(plane, k());
    }
    static vector<vector<Point>> pointsByCenter(const vector<vector<int>>& plane, int k);

    bool isStable(const vector<NPoint> &centers, Metric metric, double PRES, bool stopEarly);

    int numAssignedPoints() {
        return numAssignedPoints(plane);
    }
    static int numAssignedPoints(const vector<vector<int> > &plane);

};

#endif // MATCHING_H
