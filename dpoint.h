#ifndef DPOINT_H
#define DPOINT_H

#include <iostream>
#include "npoint.h"

using namespace std;

class DPoint
{
public:
    double i, j;
    DPoint(double i, double j): i(i), j(j) {}
    DPoint(): i(-1), j(-1) {}
    bool operator==(const DPoint& rhs) const {
        return i == rhs.i and j == rhs.j;
    }
    friend ostream& operator<<(ostream& out, const DPoint& p) {
        out << "(" << p.i << "," << p.j << ")";
        return out;
    }
    operator NPoint() const {return NPoint(i,j);}

};

#endif // DPOINT_H
