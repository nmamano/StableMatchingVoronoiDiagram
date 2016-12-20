#ifndef MATCHINGUTILS_H
#define MATCHINGUTILS_H
#include <vector>
#include <iostream>
#include <queue>
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

struct SiteStruct {
    Point site;
    int neighbor;
    vector<int> presortedArray;
    int arrayIndex;
};

struct IdRealDist {
    double dist;
    int id;
    bool operator<(const IdRealDist& rhs) const {
        return dist < rhs.dist;
    }
    bool operator>(const IdRealDist& rhs) const {
        return dist > rhs.dist;
    }
};

struct IdIntDist {
    int dist;
    int id;
    bool operator<(const IdIntDist& rhs) const {
        return dist < rhs.dist;
    }
    bool operator>(const IdIntDist& rhs) const {
        return dist > rhs.dist;
    }
};

//set of small integers that allows
//deletions without needing hash tables
struct DelSet {
public:
    vector<int> elems;
    DelSet(const vector<int>& v);
    DelSet(int maxElem);
    DelSet() {}
    int size() {return elems.size();}
    void erase(int elem);

private:
    vector<int> positions;

};

template<class T> using min_heap = priority_queue<T, vector<T>, greater<T>>;


NPoint weightedCentroid(const vector<Point>& points,
        const NPoint& center, Metric metric, Num weightExp, bool realCentroids);
vector<NPoint> weightedCentroids(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric, Num weightExp, bool realCentroids);


vector<NPoint> randomCenters(int n, int k, bool realCoords);
bool contains(const vector<NPoint> &ps, const NPoint &q);
bool areEqual(const vector<NPoint>& v1, const vector<NPoint>& v2);

NPoint randomPoint(int n, bool realCoords);

double avgDistPointCenter(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric);
double avgDistCenterCentroid(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric, Num weightExp, bool realCentroids);

vector<Point> intPoints(const vector<NPoint>& points);
vector<DPoint> doublePoints(const vector<Point>& points);
vector<DPoint> doublePoints(const vector<NPoint>& points);
vector<NPoint> numPoints(const vector<Point>& points);
vector<NPoint> numPoints(const vector<DPoint>& points);

unsigned int planeHash(const vector<vector<int>> &plane);

int numRemSites(const vector<int>& quotas);


#endif // MATCHINGUTILS_H
