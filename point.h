#ifndef POINT_H
#define POINT_H
#include <vector>
#include <iostream>
#include "dpoint.h"
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
    operator DPoint(){return DPoint(i,j);}
};


#endif // POINT_H
