#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "npoint.h"
#include "circle.h"
#include "edge.h"

class Triangle
{
public:
    NPoint v1, v2, v3;
    Triangle() {}
    Triangle(NPoint v1, NPoint v2, NPoint v3);
    Circle circumcircle();
    bool hasVertex(NPoint q);
    bool hasEdge(Edge e);
    Edge edge1();
    Edge edge2();
    Edge edge3();
    vector<Edge> edges() {
        vector<Edge> res(0);
        res.push_back(edge1());
        res.push_back(edge2());
        res.push_back(edge3());
        return res;
    }

    friend ostream& operator<<(ostream& out, const Triangle& t) {
        out << "[" << t.v1 << ", " << t.v2 << ", " << t.v3 << "]";
        return out;
    }
};

#endif // TRIANGLE_H
