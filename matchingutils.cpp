#include "matchingutils.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <QtWidgets>
#include <math.h>

using namespace std;

double Point::dist(const Point &q) const {
    return sqrt((i-q.i)*(i-q.i)+(j-q.j)*(j-q.j));
}

double Point::magnitude() const {
    return dist(Point(0,0));
}

bool is_stable(const vector<vector<int> > &plane, const vector<Point> &centers) {
    bool res = true;
    int n = plane.size();
    int k = centers.size();
    vector<double> farthest_dist(k, -1);
    vector<int> num_points(k, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int c_id = plane[i][j];
            num_points[c_id]++;
            Point p(i, j);
            double dis = p.dist(centers[c_id]);
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
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int c_id = plane[i][j];
            Point p(i, j);
            double dis = p.dist(centers[c_id]);
            for (int r = 0; r < k; r++) {
                if (r != c_id) {
                    double dis2 = p.dist(centers[r]);
                    if (dis2 < dis and dis2 < farthest_dist[r]) {
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
vector<int> center_quotas(int n, int k) {
    int quota = (n*n)/k;
    vector<int> quotas(k, quota);

    //distribute the remaining sites among the first centers
    int remaining_sites = n*n - quota*k;
    int i = 0;
    while (remaining_sites > 0) {
        quotas[i]++;
        remaining_sites--;
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
            p = random_point(n);
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

Point random_point(int n)
{
    return Point(qrand()%n, qrand()%n);
}



