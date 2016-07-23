#include "matchingutils.h"
#include <vector>
#include <math.h>
#include <algorithm>
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


vector<Point> randomCenters(int n, int k)
{
    vector<Point> centers(0);
    for (int i = 0; i < k; i++) {
        Point p;
        do {
            p = randomPoint(n);
        } while (contains(centers, p));
        centers.push_back(p);
    }
    return centers;
}

bool contains(const vector<Point>& ps, const Point& p)
{
    for (const Point& q : ps) if (q == p) return true;
    return false;
}

Point randomPoint(int n)
{
    return Point(qrand()%n, qrand()%n);
}

vector<Point> centroids(const vector<vector<int>>& plane,
                        int k) {
    vector<Point> centroids(k);
    vector<vector<Point>> points = pointsByCenter(plane, k);
    for (int i = 0; i < k; i++) {
        centroids[i] = centroid(points[i]);
    }
    return centroids;
}

Point centroid(const vector<Point>& points) {
    double iSum = 0, jSum = 0;
    for (const Point& p : points) {
        iSum += p.i;
        jSum += p.j;
    }
    int count = points.size();
    return Point(round(iSum/count), round(jSum/count));
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

vector<Point> weightedCentroids(
        const vector<vector<int>>& plane,
        int k, const vector<Point>& centers, Metric metric, Num weightExp) {
    vector<Point> centroids(k);
    vector<vector<Point>> points = pointsByCenter(plane, k);
    for (int i = 0; i < k; i++) {
        centroids[i] = weightedCentroid(points[i], centers[i], metric, weightExp);
    }
    return centroids;
}

Point weightedCentroid(const vector<Point>& points,
        const Point& center, Metric metric, Num weightExp) {
    if (weightExp.isInf()) {
        throw runtime_error("not implemented yet");
    }
    double iSum = 0, jSum = 0, totalWeight = 0;
    for (const Point& p : points) {
        double dis = metric.ddist(p, center);
        if (dis == 0 && weightExp.val < 0) continue;
        double pWeight = pow(dis, weightExp.val);
        iSum += p.i*pWeight;
        jSum += p.j*pWeight;
        totalWeight += pWeight;
    }
    return Point(round(iSum/totalWeight), round(jSum/totalWeight));
}



double avgDistPointCenter(const vector<vector<int> > &plane,
        const vector<Point> &centers, Metric metric) {
    int n = plane.size();
    double dis_sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int c_id = plane[i][j];
            if (c_id == -1) cout<<"Error: unassigned point"<<endl;
            Point c = centers[c_id];
            dis_sum += metric.ddist(Point(i, j), c);
        }
    }
    return dis_sum/(n*n);
}

double avgDistCenterCentroid(const vector<vector<int> > &plane,
        const vector<Point> &centers, Metric metric, Num weightExp)
{
    int k = centers.size();
    vector<Point> ctrds = weightedCentroids(plane, k, centers, metric, weightExp);
    double dis_sum = 0;
    for (int i = 0; i < k; i++) {
        dis_sum += metric.ddist(centers[i], ctrds[i]);
    }
    return dis_sum/k;
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
