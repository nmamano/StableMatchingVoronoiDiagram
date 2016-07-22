#ifndef METRIC_H
#define METRIC_H
#include "num.h"
#include "point.h"
#include <string>

class Metric : public Num
{
public:

    Metric(Num l): Num(l) {}
    bool hasIntDists() const;
    double ddist(const Point& p, const Point& q) const;

    int iSortValue(const Point& p, const Point& q) const;
    double dSortValue(const Point& p, const Point& q) const;

    bool operator==(const Metric& rhs) const;

private:

    static int distL1(const Point& p, const Point& q);
    static int distL2Squared(const Point& p, const Point& q);
    static double distL2(const Point& p, const Point& q);
    static int distLInf(const Point& p, const Point& q);
    double distLGen(const Point& p, const Point& q) const;


    double sortValueGen(const Point &p, const Point &q) const;
    static int ipow(int base, int exp);
    double iSortValueGen(const Point &p, const Point &q) const;
    double dSortValueGen(const Point &p, const Point &q) const;
};

#endif // METRIC_H
