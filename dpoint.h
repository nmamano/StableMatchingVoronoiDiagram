#ifndef DPOINT_H
#define DPOINT_H
#include <vector>
#include <iostream>

using namespace std;

class DPoint
{
public:
    double i, j;
    DPoint(int i, int j): i(i), j(j) {}
    DPoint(): i(-1), j(-1) {}
    bool operator==(const DPoint& rhs) const {
        return i == rhs.i and j == rhs.j;
    }
    friend ostream& operator<<(ostream& out, const DPoint& p) {
        out << "(" << p.i << "," << p.j << ")";
        return out;
    }
};

#endif // DPOINT_H
