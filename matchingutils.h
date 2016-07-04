#ifndef MATCHINGUTILS_H
#define MATCHINGUTILS_H
#include <vector>
#include <iostream>

using namespace std;

class Point
{
public:
    int i, j;
    Point(int i, int j): i(i), j(j) {}
    Point(): i(-1), j(-1) {}
    bool operator==(const Point& rhs) const {
        return i == rhs.i and j == rhs.j;
    }
    friend ostream& operator<<(ostream& out, const Point& p) {
        out << "(" << p.i << "," << p.j << ")";
        return out;
    }
    double distL2(const Point& q) const;

    static bool sortL1(const Point& p1, const Point& p2) {
        return p1.i+p1.j < p2.i+p2.j;
    }

    static bool sortL2(const Point& p1, const Point& p2) {
        return p1.i*p1.i+p1.j*p1.j < p2.i*p2.i+p2.j*p2.j;
    }

    static bool sortLInfty(const Point& p1, const Point& p2) {
        return max(p1.i, p1.j) < max(p2.i, p2.j);
    }

    static double distL1(int pi, int pj, int qi, int qj) {
        return abs(pi-qi)+abs(pj-qj);
    }

    static double distL2Squared(int pi, int pj, int qi, int qj) {
        return (pi-qi)*(pi-qi)+(pj-qj)*(pj-qj);
    }

    static double distLInfty(int pi, int pj, int qi, int qj) {
        return max(abs(pi-qi),abs(pj-qj));
    }

    static double dist(int pi, int pj, int qi, int qj, int metric) {
        if (metric == 1) return distL1(pi, pj, qi, qj);
        if (metric == 2) return distL2Squared(pi, pj, qi, qj);
        if (metric == -1) return distLInfty(pi, pj, qi, qj);
        cout << "Error: not found metric";
        return -1;
    }

    static double dist(const Point& p, const Point& q, int metric) {
        return dist(p.i, p.j, q.i, q.j, metric);
    }

};


struct Link {
    int i,j;
    int c_id;
    double dis;
    Link(int i, int j, int c_id, double dis):
        i(i), j(j), c_id(c_id), dis(dis) { }
    bool operator<(const Link& rhs) const {return dis<rhs.dis;}
    Link() { }
};

bool is_stable(const vector<vector<int> > &plane, const vector<Point> &centers, int distMetric);
vector<int> centerQuotas(int n, int k);

vector<Point> centroids(const vector<vector<int>>& plane, int k);
vector<vector<Point>> pointsByCenter(const vector<vector<int>>& plane,
        int k);
Point centroid(const vector<Point>& points);

vector<Point> randomCenters(int n, int k);
bool contains(const vector<Point>& ps, const Point& p);
Point randomPoint(int n);

int numAssignedPoints(const vector<vector<int>>& plane);

double avgDistPointCenter(const vector<vector<int>>& plane, const vector<Point>& centers, int metric);
double avgDistCenterCentroid(const vector<vector<int>>& plane, const vector<Point>& centers, int metric);


#endif // MATCHINGUTILS_H
