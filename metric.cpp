#include "metric.h"
#include <cmath>
using namespace std;


bool Metric::hasIntDists() const {
    return isInf() || isInt();
}

double Metric::ddist(const Point &p, const Point &q) const {
    if (isInf()) return distLInf(p, q);
    if (val == 2) return distL2(p, q);
    if (val == 1) return distL1(p, q);
    return distLGen(p, q);
}

double Metric::distLGen(const Point &p, const Point &q) const {
    double i = pow(abs(p.i-q.i), val);
    double j = pow(abs(p.j-q.j), val);
    return pow((i+j), 1/val);
}

double Metric::dSortValue(const Point &p, const Point &q) const {
    if (isInf()) return distLInf(p, q);
    if (val == 2) return distL2Squared(p, q);
    if (val == 1) return distL1(p, q);
    return dSortValueGen(p, q);
}

double Metric::dSortValueGen(const Point &p, const Point& q) const {
    double i = pow(abs(p.i-q.i), val);
    double j = pow(abs(p.j-q.j), val);
    return i+j;
}

int Metric::iSortValue(const Point &p, const Point &q) const {
    if (isInf()) return distLInf(p, q);
    if (val == 2) return distL2Squared(p, q);
    if (val == 1) return distL1(p, q);
    return iSortValueGen(p, q);
}

double Metric::iSortValueGen(const Point &p, const Point& q) const {
    int i = ipow(abs(p.i-q.i), toInt());
    int j = ipow(abs(p.j-q.j), toInt());
    return i+j;
}

int Metric::ipow(int base, int exp) {
    int result = 1;
    while (exp) {
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}

int Metric::distL1(const Point &p, const Point &q) {
    return abs(p.i-q.i)+abs(p.j-q.j);
}

int Metric::distL2Squared(const Point &p, const Point &q) {
    return (p.i-q.i)*(p.i-q.i)+(p.j-q.j)*(p.j-q.j);
}

double Metric::distL2(const Point &p, const Point &q) {
    return sqrt(distL2Squared(p, q));
}

int Metric::distLInf(const Point &p, const Point &q) {
    return max(abs(p.i-q.i), abs(p.j-q.j));
}

bool Metric::operator==(const Metric &rhs) const {
    return val == rhs.val;
}