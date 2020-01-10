#ifndef EDGE_H
#define EDGE_H
#include "npoint.h"
#include "metric.h"

class Edge
{
public:
    NPoint start, end;
    Edge() {}
    Edge(NPoint start, NPoint end): start(start), end(end) {

    }
    bool operator==(const Edge& rhs) const {
        return (start == rhs.start and end == rhs.end) or
               (start == rhs.end and end == rhs.start);
    }
    double length();
    Edge asVector();
    Edge asUnitVector();
    Edge perpendicular();
    Edge opposite();
    Edge stretch(double factor);

    double dist(NPoint q);
};

#endif // EDGE_H
