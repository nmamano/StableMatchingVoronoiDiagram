#include "matchingutils.h"
#include "matching.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <QtWidgets>
#include <math.h>
#include "circle.h"

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

vector<NPoint> randomCentersCentered(int n, int k, bool realCoords)
{
    vector<NPoint> centers = randomCenters(n, k, realCoords);
    for (int i = 0; i < k; i++) {
        centers[i] = NPoint(centers[i].di()/2+n/4,
                            centers[i].dj()/2+n/4);
    }
    return centers;
}

vector<NPoint> randomCentersInCircle(int n, int k, bool realCoords)
{
    vector<NPoint> centers(0);
    double mid = n/2.0;
    double rad = n/3.5;
    Circle C(NPoint(mid, mid), rad);
    for (int i = 0; i < k; i++) {
        NPoint p;
        do {
            if (realCoords) p = randomPoint(n, realCoords);
            else {
                do {
                    p = randomPoint(n, realCoords);
                } while (contains(centers, p));
            }
        } while (not C.contains(p));
        centers.push_back(p);
    }
    return centers;
}


vector<NPoint> packedCenters(int k, double appetite) {
    vector<NPoint> centers(0);
    double side = sqrt(appetite);
    for (int i = 2; i <= k; i++) {
        for (int j = 2; j <= k; j++) {
            centers.push_back(NPoint(0.97*side*i, 0.97*side*j));
        }
    }
    return centers;
}


vector<NPoint> lowerBoundCenters(int n, int k, double appetite) {
    vector<double> angles(k);
    double pi = 3.141592653589793238462643383279502884197169399375105820974;
    double epsilon = 0.0005;
    for (int i = 0; i < k; i++) {
        angles[i] = 2*pi*i/k-i*epsilon;
    }
    vector<NPoint> centers(2*k);
    double mid = n/2.0;
    for (int i = 0; i < k; i++) {
        centers[i] = NPoint(mid+cos(angles[i])*0.5,
                            mid+sin(angles[i])*0.5);
    }

    double circleRad = sqrt(k*appetite/pi);
    double outerRad = sqrt(2.0*k*appetite/pi);
    double annulusMid = 2*circleRad+(outerRad-circleRad);
    cerr << circleRad << " " << outerRad << " " << annulusMid << endl;
    for (int i = k; i < 2*k; i++) {
        centers[i] = NPoint(mid+cos(2*pi*i/k)*annulusMid*0.5,
                            mid+sin(2*pi*i/k)*annulusMid*0.5);
    }

    return centers;
}

vector<NPoint> lowerBoundCentersHalf(int n, int k) {
    vector<double> angles(k);
    double pi = 3.141592653589793238462643383279502884197169399375105820974;
    double epsilon = 0.005;
    for (int i = 0; i < k; i++) {
        angles[i] = 2*pi*i/k-i*epsilon;
    }
    vector<NPoint> centers(k);
    double mid = n/2.0;
    for (int i = 0; i < k; i++) {
        centers[i] = NPoint(mid+cos(angles[i])*0.5,
                            mid+sin(angles[i])*0.5);
    }
    return centers;
}

vector<NPoint> centersInCircle(int n, int k) {
    vector<double> angles(k);
    double pi = 3.141592653589793238462643383279502884197169399375105820974;
    for (int i = 0; i < k; i++) {
        angles[i] = 2*pi*i/k;
    }
    vector<NPoint> centers(k);
    double mid = n/2.0;
    for (int i = 0; i < k; i++) {
        centers[i] = NPoint(mid+cos(angles[i])*50,
                            mid+sin(angles[i])*50);
    }
    return centers;
}

vector<NPoint> invadeAllCenters(int n, int k) {
    vector<double> angles(k-1);
    for (int i = 0; i < k-1; i++) {
        angles[i] = 2*PI*i/(k-1);
    }
    vector<NPoint> centers(k);
    double mid = n/2.0;
    for (int i = 0; i < k-1; i++) {
        centers[i] = NPoint(mid+70*cos(angles[i]),
                            mid+70*sin(angles[i]));
    }
    centers[k-1] = NPoint(mid, mid);
    return centers;
}

vector<NPoint> invadedByAllCenters(int n, int k) {
    vector<double> angles(k-1);
    for (int i = 0; i < k-1; i++) {
        angles[i] = 2*PI*i/(k-1);
    }
    vector<NPoint> centers(k);
    double mid = n/2.0;
    for (int i = 0; i < k-1; i++) {
        centers[i] = NPoint(mid+70*cos(angles[i]),
                            mid+70*sin(angles[i]));
    }
    centers[k-1] = NPoint(mid, mid);
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

vector<double> maxDistsPointCenter(const vector<vector<int> > &plane,
        const vector<NPoint> &centers, Metric metric) {
    int n = plane.size();
    int k = centers.size();
    vector<double> res = vector<double> (k, -1);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int cId = plane[i][j];
            if (cId == -1) continue;
            NPoint c = centers[cId];
            res[cId] = max(res[cId], metric.ddist(NPoint(i, j), c));
        }
    }
    //for(int i = 0; i < k; i++) cerr<<res[i]<<" ";cerr<<endl;
    return res;
}

bool insideOneOf(const NPoint &p, const vector<Circle>& circles,
                    const Metric& metric) {
    int n = circles.size();
    for (int i = 0; i < n; i++) {
        if (metric.ddist(p, circles[i].center) <= circles[i].radius) return true;
    }
    return false;
}

//returns a vector where V[i] indicates the rank of i
vector<int> indexRanks(const vector<double>& criteria) {
    int n = criteria.size();
    vector<int> res(n, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j && (criteria[i] > criteria[j] ||
                    (criteria[i] == criteria[j] && i > j))) {
                res[i]++;
            }
        }
    }
    return res;
}

vector<int> sortedRanks(const vector<double>& criteria) {
    vector<int> res = indexRanks(criteria);
    int n = res.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (res[j] == i) {
                res[i] = j;
                break;
            }
        }
    }
    return res;
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

