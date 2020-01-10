#ifndef CPSTRUCT_H
#define CPSTRUCT_H

#include <vector>
#include <string>
#include "point.h"
#include "dpoint.h"
#include "metric.h"
#include "matching.h"

using namespace std;

class CPStruct
{
public:
    CPStruct(Metric metric, const vector<Point>& points):
        metric(metric), real(false), points(points) {}
    CPStruct(Metric metric, const vector<DPoint>& dpoints):
        metric(metric), real(true), dpoints(dpoints) {}


    virtual string acronym() const =0;

    virtual void delInd(int ind) =0;
    virtual int closestInd(const Point& p) const =0;

    double dist(const Point& point, int ind) const {
        if (!real) return intDist(point, ind);
        else return realDist(point, ind);
    }
    int intDist(const Point& point, int ind) const {
        return metric.iSortValue(point, points[ind]);
    }
    double realDist(const Point& point, int ind) const {
        return metric.dSortValue(point, dpoints[ind]);
    }


protected:
    Metric metric;
    bool real;
    vector<Point> points;
    vector<DPoint> dpoints;

};

#endif // CPSTRUCT_H






