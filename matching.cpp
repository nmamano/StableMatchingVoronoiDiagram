#include "matching.h"
#include <iostream>

Matching::Matching(const vector<vector<int> > &plane,
        const vector<int> &quotas):
    plane(plane), quotas(quotas) {}

Matching::Matching(int n, int k, int appetite) {
    plane = vector<vector<int>> (n, vector<int> (n, -1));
    quotas = centerQuotas(n, k, appetite);
}

Matching::Matching(const Matching &M) {
    plane = M.plane;
    quotas = M.quotas;
}

vector<int> Matching::centerQuotas(int n, int k, int appetite) {
    if (appetite != 0) {
        return vector<int> (k, appetite);
    }
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

vector<int> Matching::remainingCIds() const {
    vector<int> res(0);
    res.reserve(k());
    for (int i = 0; i < k(); i++) {
        if (quotas[i] > 0) res.push_back(i);
    }
    res.shrink_to_fit();
    return res;
}

int Matching::numRemSites() const {
    int res = 0;
    for (int quota : quotas) res += quota;
    return res;
}

int Matching::numUmatchedSites() const {
    int count = 0;
    for (int i = 0; i < n(); i++) {
        for (int j = 0; j < n(); j++) {
            if (plane[i][j] == -1) {
                count++;
            }
        }
    }
    return count;
}

vector<Point> Matching::remainingSites() const {
    vector<Point> res(0);
    res.reserve(numRemSites());
    for (int i = 0; i < n(); i++) {
        for (int j = 0; j < n(); j++) {
            if (plane[i][j] == -1) {
                res.push_back(Point(i,j));
            }
        }
    }
    return res;
}

vector<vector<Point> > Matching::pointsByCenter(const vector<vector<int> > &plane, int k) {
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

bool Matching::isStable(const vector<NPoint> &centers, Metric metric, double PRES, bool stopEarly) {
    bool res = true;
    int n = plane.size();
    int k = centers.size();
    vector<double> farthestDist(k, -1);
    vector<int> regionSizes(k, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int cId = plane[i][j];
            regionSizes[cId]++;
            NPoint c = centers[cId];
            double dis = metric.ddist(Point(i, j), c);
            if (dis > farthestDist[cId]) {
                farthestDist[cId] = dis;
            }
        }
    }
    int quota = n*n/k;
    for (int i = 0; i < k; i++) {
        if (regionSizes[i] != quota and regionSizes[i] != quota+1) {
            cout<<"Error: center has "<<regionSizes[i]<<" sites but the quota is "<<quota<<endl;
            if (stopEarly) return false;
            res = false;
        }
    }
    int inc = 1;
    if (n*n*k >= 1000*1000*1000) inc = 50;
    else if (n*n*k >= 10*1000*1000) inc = 10;

    for (int i = 0; i < n; i+= inc) { //sampling..
        for (int j = 0; j < n; j+= inc) {
            int cId = plane[i][j];
            Point p(i, j);
            double dis = metric.ddist(p, centers[cId]);
            for (int r = 0; r < k; r++) {
                if (r != cId) {
                    double dis2 = metric.ddist(p, centers[r]);
                    if (dis2 < dis-PRES and dis2 < farthestDist[r]-PRES) {
                        cout<<"Error: matching not stable"<<endl<<p<<" prefers center "<<r<<" "<<centers[r]<<
                        " at distance "<<dis2<<" than center "<<cId<<" "<<centers[cId]<<" at "<<
                        "distance "<<dis<<endl<<"center "<<r<<" "<<centers[r]<<" prefers "<<p<<" to "<<
                        "its farthest point which is at distance "<<farthestDist[r]<<endl;
                        if (stopEarly) return false;
                        res = false;
                    }
                }
            }
        }
    }
    return res;
}

int Matching::numAssignedPoints(const vector<vector<int> > &plane) {
    int n = plane.size();
    int count = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (plane[i][j] != -1) count++;
    return count;
}
