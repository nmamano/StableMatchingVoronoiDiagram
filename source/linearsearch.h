#ifndef LINEARSEARCH_H
#define LINEARSEARCH_H
#include "cpstruct.h"
#include "matchingutils.h"

class LinearSearch : public CPStruct
{
public:
    LinearSearch(Metric metric, const vector<Point> &points,
            const vector<int>& inds):
        CPStruct(metric, points), inds(inds) {}

    LinearSearch(Metric metric, const vector<DPoint> &centers,
            const vector<int>& inds):
        CPStruct(metric, centers), inds(inds) {}

    string acronym() const {return "LS";}

    void delInd(int ind);
    int closestInd(const Point &p) const;

private:
    DelSet inds;
};

#endif // LINEARSEARCH_H
