#ifndef MATCHINGUTILS_H
#define MATCHINGUTILS_H
#include <vector>
#include <iostream>
#include "point.h"
#include "num.h"
#include "metric.h"
using namespace std;

bool is_stable(const vector<vector<int> > &plane, const vector<Point> &centers, Metric distMetric, double PRES, bool stopEarly = true);
vector<int> centerQuotas(int n, int k);

Point centroid(const vector<Point>& points);
vector<Point> centroids(const vector<vector<int>>& plane, int k);

Point weightedCentroid(const vector<Point>& points,
        const Point& center, Metric metric, Num weightExp);
vector<Point> weightedCentroids(const vector<vector<int>>& plane,
        int k, const vector<Point>& centers, Metric metric, Num weightExp);

vector<vector<Point>> pointsByCenter(const vector<vector<int>>& plane,
        int k);

vector<Point> randomCenters(int n, int k);
bool contains(const vector<Point>& ps, const Point& p);
Point randomPoint(int n);

int numAssignedPoints(const vector<vector<int>>& plane);

double avgDistPointCenter(const vector<vector<int>>& plane, const vector<Point>& centers, Metric metric);
double avgDistCenterCentroid(const vector<vector<int>>& plane, const vector<Point>& centers, Metric metric, Num weightExp);


#endif // MATCHINGUTILS_H
