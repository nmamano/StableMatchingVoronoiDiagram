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
    bool operator<(const Point& rhs) const {
        return magnitude()<rhs.magnitude();
    }
    friend ostream& operator<<(ostream& out, const Point& p) {
        out << "(" << p.i << "," << p.j << ")";
        return out;
    }
    double dist(const Point& q) const;
    double magnitude() const;

};

struct Link {
    int i,j;
    int c_id;
    float dis;
    Link(int i, int j, int c_id, float dis):
        i(i), j(j), c_id(c_id), dis(dis) { }
    bool operator<(const Link& rhs) const {return dis<rhs.dis;}
    Link() { }
};

bool is_stable(const vector<vector<int> > &plane, const vector<Point> &centers);
vector<int> center_quotas(int n, int k);

vector<Point> randomCenters(int n, int k);
bool contains(const vector<Point>& ps, const Point& p);
Point random_point(int n);

#endif // MATCHINGUTILS_H
