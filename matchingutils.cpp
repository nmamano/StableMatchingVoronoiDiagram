#include "matchingutils.h"
#include "matching.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <QtWidgets>
#include <math.h>

using namespace std;

vector<NPoint> randomCenters(int n, int k, bool realCoords)
{
    vector<NPoint> centers(0);
    for (int i = 0; i < k; i++) {
        NPoint p;
        if (realCoords) p = randomPoint(n, realCoords);
        else {
            do {
                p = randomPoint(n, realCoords);
            } while (contains(centers, p));
        }
        centers.push_back(p);
    }
    return centers;
}

bool contains(const vector<NPoint>& ps, const NPoint& q)
{
    for (const NPoint& p : ps) if (p == q) return true;
    return false;
}

bool areEqual(const vector<NPoint>& v1, const vector<NPoint>& v2) {
    for (int i = 0; i < (int)v1.size(); i++) {
        if (v1[i] != v2[i]) return false;
    }
    return true;
}

NPoint randomPoint(int n, bool realCoords)
{
    if (!realCoords) return NPoint(qrand()%n, qrand()%n);
    double qrandMax = (double)RAND_MAX;
    return NPoint(qrand()/qrandMax*(n-1), qrand()/qrandMax*(n-1));
}

vector<NPoint> weightedCentroids(
        const vector<vector<int>>& plane,
        const vector<NPoint>& centers,
        Metric metric, Num weightExp, bool realCentroids) {

    int k = centers.size();
    vector<NPoint> centroids(k);
    vector<vector<Point>> points = Matching::pointsByCenter(plane, k);
    for (int i = 0; i < k; i++) {
        centroids[i] = weightedCentroid(points[i], centers[i], metric, weightExp, realCentroids);
    }
    return centroids;
}

NPoint weightedCentroid(const vector<Point>& points,
        const NPoint& center, Metric metric, Num weightExp, bool realCentroids) {

    if (weightExp.isInf()) {
        //should return the circumcentre
        throw runtime_error("not implemented yet");
    }

    double iSum = 0, jSum = 0, totalWeight = 0;
    for (const Point& p : points) {
        double dis = metric.ddist(NPoint(p), center);
        if (dis == 0 && weightExp.val < 0) continue;
        double pWeight = pow(dis, weightExp.val);
        iSum += p.i*pWeight;
        jSum += p.j*pWeight;
        totalWeight += pWeight;
    }
    NPoint centroid(iSum/totalWeight, jSum/totalWeight);
    if (realCentroids) return centroid;
    else return NPoint(centroid.i.round(), centroid.j.round());
}

double avgDistPointCenter(const vector<vector<int> > &plane,
        const vector<NPoint> &centers, Metric metric) {
    int n = plane.size();
    double disSum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int cId = plane[i][j];
            if (cId == -1) cout<<"Error avg dist: unassigned point"<<endl;
            NPoint c = centers[cId];
            disSum += metric.ddist(NPoint(i, j), c);
        }
    }
    return disSum/(n*n);
}

double avgDistCenterCentroid(const vector<vector<int> > &plane,
        const vector<NPoint> &centers, Metric metric, Num weightExp, bool realCentroids)
{
    int k = centers.size();
    vector<NPoint> ctrds = weightedCentroids(plane, centers, metric, weightExp, realCentroids);
    double disSum = 0;
    for (int i = 0; i < k; i++) {
        disSum += metric.ddist(centers[i], ctrds[i]);
    }
    return disSum/k;
}

vector<NPoint> numPoints(const vector<Point> &points) {
    vector<NPoint> res;
    for (const Point& p : points) res.push_back(p);
    return res;
}

vector<NPoint> numPoints(const vector<DPoint> &points) {
    vector<NPoint> res;
    for (const DPoint& p : points) res.push_back(p);
    return res;
}

vector<DPoint> doublePoints(const vector<Point>& points) {
    vector<DPoint> res;
    for (const Point& p : points) res.push_back(p);
    return res;
}

vector<Point> intPoints(const vector<NPoint>& points) {
    int n = points.size();
    vector<Point> res(n);
    for (int i = 0; i < n; i++) {
        NPoint p = points[i];
        res[i] = Point(p.i.asInt(), p.j.asInt());
    }
    return res;
}

vector<DPoint> doublePoints(const vector<NPoint>& points) {
    int n = points.size();
    vector<DPoint> res(n);
    for (int i = 0; i < n; i++) {
        NPoint p = points[i];
        res[i] = DPoint(p.i.asDouble(), p.j.asDouble());
    }
    return res;
}

unsigned int planeHash(const vector<vector<int>> &plane) {
    const int BIG_PRIME = 10000019;
    int n = plane.size();
    unsigned int hash = 5381;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            hash = (hash*65599 + plane[i][j]) % BIG_PRIME;
        }
    }
    return hash;
}

DelSet::DelSet(const vector<int> &v): elems(v) {
    if (elems.size() == 0) return;
    int maxElem = elems[0];
    for (int elem : elems) {
        maxElem = max(maxElem, elem);
    }
    positions = vector<int>(maxElem+1, -1);
    for (int i = 0; i < (int)elems.size(); i++) {
        positions[elems[i]] = i;
    }
}

DelSet::DelSet(int maxElem) {
    elems = vector<int> (maxElem);
    positions = vector<int> (maxElem);
    for (int i = 0; i < maxElem; i++) {
        elems[i] = positions[i] = i;
    }
}

void DelSet::erase(int elem) {
    int pos = positions[elem];
    if (pos == -1) return;
    positions[elem] = -1;
    if (pos != (int)elems.size()-1) {
        elems[pos] = elems[elems.size()-1];
        positions[elems[pos]] = pos;
    }
    elems.pop_back();
}
