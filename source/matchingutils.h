#ifndef MATCHINGUTILS_H
#define MATCHINGUTILS_H
#include <vector>
#include <iostream>
#include <queue>
#include "point.h"
#include "npoint.h"
#include "num.h"
#include "metric.h"
#include "circle.h"
using namespace std;

const double PI = 3.141592653589793238462643383279502884197169399375105820974;

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
vector<NPoint> randomCentersCentered(int n, int k, bool realCoords);
vector<NPoint> randomCentersInCircle(int n, int k, bool realCoords);
vector<NPoint> lowerBoundCenters(int n, int k, double appetite);
vector<NPoint> centersInCircle(int n, int k);
vector<NPoint> lowerBoundCentersHalf(int n, int k);
vector<NPoint> invadeAllCenters(int n, int k);
vector<NPoint> invadedByAllCenters(int n, int k);
vector<NPoint> packedCenters(int k, double appetite);
bool contains(const vector<NPoint> &ps, const NPoint &q);
bool areEqual(const vector<NPoint>& v1, const vector<NPoint>& v2);

NPoint randomPoint(int n, bool realCoords);

double avgDistPointCenter(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric);
vector<double> maxDistsPointCenter(const vector<vector<int> > &plane, const vector<NPoint> &centers, Metric metric);
double avgDistCenterCentroid(const vector<vector<int>>& plane, const vector<NPoint> &centers, Metric metric, Num weightExp, bool realCentroids);

vector<int> indexRanks(const vector<double>& criteria);
vector<int> sortedRanks(const vector<double>& criteria);
bool insideOneOf(const NPoint &p, const vector<Circle>& circles, const Metric &metric);

vector<Point> intPoints(const vector<NPoint>& points);
vector<DPoint> doublePoints(const vector<Point>& points);
vector<DPoint> doublePoints(const vector<NPoint>& points);
vector<NPoint> numPoints(const vector<Point>& points);
vector<NPoint> numPoints(const vector<DPoint>& points);

unsigned int planeHash(const vector<vector<int>> &plane);

int numRemSites(const vector<int>& quotas);


#endif // MATCHINGUTILS_H
