#ifndef NPOINT_H
#define NPOINT_H
#include <iostream>
#include "num.h"

using namespace std;

class NPoint
{
public:
    Num i, j;
    NPoint(Num i, Num j): i(i), j(j) {}
    NPoint(): i(-1), j(-1) {}
    double di() const {return i.asDouble();}
    double dj() const {return j.asDouble();}

    bool operator==(const NPoint& rhs) const {
        return i == rhs.i and j == rhs.j;
    }
    bool operator!=(const NPoint& rhs) const {
        return not (*this == rhs);
    }
    friend ostream& operator<<(ostream& out, const NPoint& p) {
        out << "(" << p.i << "," << p.j << ")";
        return out;
    }
};

#endif // NPOINT_H
