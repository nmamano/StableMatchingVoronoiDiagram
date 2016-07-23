#ifndef MATCHINGUTILS_H
#define MATCHINGUTILS_H
#include <vector>
#include <iostream>
#include "point.h"
#include "npoint.h"
#include "num.h"
#include "metric.h"
using namespace std;

struct Link {
    int i,j;
    int cId;
    Link(int i, int j, int cId):
        i(i), j(j), cId(cId) { }
    Link(): cId(-1) { }
    friend ostream& operator<<(ostream& out, const Link& l) {
        out << "(" << l.i << "," << l.j << "," << l.cId << ")";
        return out;
    }
};

bool is_stable(const vector<vector<int> > &plane, const vector<Point> &centers, Metric distMetric, double PRES, bool stopEarly);
vector<int> centerQuotas(int n, int k);

NPoint weightedCentroid(const vector<Point>& points,
        const NPoint& center, Metric metric, Num weightExp, bool realCentroids);
vector<NPoint> weightedCentroids(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric, Num weightExp, bool realCentroids);

vector<vector<Point>> pointsByCenter(const vector<vector<int>>& plane,
        int k);

vector<NPoint> randomCenters(int n, int k, bool realCoords);
bool contains(const vector<NPoint> &ps, const NPoint &q);
bool areEqual(const vector<NPoint>& v1, const vector<NPoint>& v2);

NPoint randomPoint(int n, bool realCoords);

int numAssignedPoints(const vector<vector<int>>& plane);

double avgDistPointCenter(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric);
double avgDistCenterCentroid(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric, Num weightExp, bool realCentroids);

vector<Point> intPoints(const vector<NPoint>& points);
vector<DPoint> doublePoints(const vector<NPoint>& points);

#endif // MATCHINGUTILS_H
