#include "matchingutils.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <QtWidgets>
#include <math.h>

using namespace std;

bool is_stable(const vector<vector<int> > &plane,
        const vector<Point> &centers, Metric metric, double PRES, bool stopEarly) {
    bool res = true;
    int n = plane.size();
    int k = centers.size();
    vector<double> farthest_dist(k, -1);
    vector<int> num_points(k, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int c_id = plane[i][j];
            num_points[c_id]++;
            Point c = centers[c_id];
            double dis = metric.ddist(Point(i, j), c);
            if (dis > farthest_dist[c_id]) {
                farthest_dist[c_id] = dis;
            }
        }
    }
    int quota = n*n/k;
    for (int i = 0; i < k; i++) {
        if (num_points[i] != quota and num_points[i] != quota+1) {
            cout << "Error: center has " << num_points[i] << " sites but the quota is " << quota << endl;
            if (stopEarly) return false;
            res = false;
        }
    }
    int inc = 1;
    if (n*n*k >= 1000*1000*1000) inc = 50;
    else if (n*n*k >= 10*1000*1000) inc = 10;

    for (int i = 0; i < n; i+= inc) { //sampling..
        for (int j = 0; j < n; j+= inc) {
            int c_id = plane[i][j];
            Point p(i, j);
            double dis = metric.ddist(p, centers[c_id]);
            for (int r = 0; r < k; r++) {
                if (r != c_id) {
                    double dis2 = metric.ddist(p, centers[r]);
                    if (dis2 < dis-PRES and dis2 < farthest_dist[r]-PRES) {
                        cout << "Error: matching not stable" << endl << p << " prefers center " << r << " " << centers[r] <<
                                " at distance " << dis2 << " than center " << c_id << " " << centers[c_id] << " at " <<
                                "distance " << dis << endl << "center " << r << " " << centers[r] << " prefers " << p << " to " <<
                                "its farthest point which is at distance " << farthest_dist[r] << endl;
                        if (stopEarly) return false;
                        res = false;
                    }
                }
            }
        }
    }
    return res;
}

/** Returns the number of sites that correspond to each center. If the quotas
are not a whole number, some centers are allowed an extra site */
vector<int> centerQuotas(int n, int k) {
    int quota = (n*n)/k;
    vector<int> quotas(k, quota);

    //distribute the remaining sites among the first centers
    int remPoints = n*n - quota*k;
    int i = 0;
    while (remPoints > 0) {
        quotas[i]++;
        remPoints--;
        i++;
    }
    return quotas;
}


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

/** Returns the list of points assigned to each center */
vector<vector<Point>> pointsByCenter(const vector<vector<int>>& plane,
        int k) {
    int n = plane.size();
    vector<vector<Point>> res(k);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int c_id = plane[i][j];
            if (c_id == -1)
                cout << " Error: unassigned point" << endl;
            res[c_id].push_back(Point(i, j));
        }
    }
    return res;
}

vector<NPoint> weightedCentroids(
        const vector<vector<int>>& plane,
        const vector<NPoint>& centers,
        Metric metric, Num weightExp, bool realCentroids) {

    int k = centers.size();
    vector<NPoint> centroids(k);
    vector<vector<Point>> points = pointsByCenter(plane, k);
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

int numAssignedPoints(const vector<vector<int> > &plane)
{
    int n = plane.size();
    int count = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (plane[i][j] != -1) count++;
    return count;
}
