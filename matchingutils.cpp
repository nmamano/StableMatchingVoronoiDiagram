#include "matchingutils.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <QtWidgets>
#include <math.h>

using namespace std;

double Point::distL2(const Point &q) const {
    return sqrt((i-q.i)*(i-q.i)+(j-q.j)*(j-q.j));
}

bool is_stable(const vector<vector<int> > &plane, const vector<Point> &centers, int metric) {
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
            double dis = Point::dist(Point(i, j), c, metric);
            if (dis > farthest_dist[c_id]) {
                farthest_dist[c_id] = dis;
            }
        }
    }
    int quota = n*n/k;
    for (int i = 0; i < k; i++) {
        if (num_points[i] != quota and num_points[i] != quota+1) {
            res = false;
            cout << "Error: center has " << num_points[i] << " sites but the quota is " << quota << endl;
            return res;
        }
    }
    for (int i = 0; i < n; i+= 10) { //pseudo random sampling..
        for (int j = 0; j < n; j+= 10) {
            int c_id = plane[i][j];
            Point p(i, j);
            double dis = Point::dist(p, centers[c_id], metric);
            for (int r = 0; r < k; r++) {
                if (r != c_id) {
                    double dis2 = Point::dist(p, centers[r], metric);
                    const double PRES = 0.0000;
                    if (dis2 < dis-PRES and dis2 < farthest_dist[r]-PRES) {
                        res = false;
                        cout << "Error: matching not stable" << endl << p << " prefers center " << r << " " << centers[r] <<
                                " at distance " << dis2 << " than center " << c_id << " " << centers[c_id] << " at " <<
                                "distance " << dis << endl << "center " << r << " " << centers[r] << " prefers " << p << " to " <<
                                "its farthest point which is at distance " << farthest_dist[r] << endl;
                        return res;
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

Point centroid(const vector<Point>& points) {
    double iSum = 0, jSum = 0;
    for (const Point& p : points) {
        iSum += p.i;
        jSum += p.j;
    }
    int count = points.size();
    return Point(round(iSum/count), round(jSum/count));
}

double avgDistPointCenter(const vector<vector<int> > &plane, const vector<Point> &centers, int metric)
{
    int n = plane.size();
    double dis_sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int c_id = plane[i][j];
            if (c_id == -1) cout<<"Error: unassigned point"<<endl;
            Point c = centers[c_id];
            dis_sum += Point::dist(Point(i, j), c, metric);
        }
    }
    return dis_sum/(n*n);
}

double avgDistCenterCentroid(const vector<vector<int> > &plane, const vector<Point> &centers, int metric)
{
    int k = centers.size();
    vector<Point> ctrds = centroids(plane, k);
    double dis_sum = 0;
    for (int i = 0; i < k; i++) {
        dis_sum += Point::dist(centers[i], ctrds[i], metric);
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
