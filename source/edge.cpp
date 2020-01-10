#include "edge.h"


double Edge::length() {
    Metric L2(2);
    return L2.ddist(start, end);
}

Edge Edge::asVector() {
    return Edge(NPoint(0,0), NPoint(end.di()-start.di(),end.dj()-start.dj()));
}

Edge Edge::asUnitVector() {
    Edge e = asVector();
    return Edge(NPoint(0,0), NPoint(e.end.di()/length(),
    e.end.dj()/length()));
}

Edge Edge::perpendicular() {
    return Edge(start, NPoint(end.dj(), -end.di()));
}

Edge Edge::opposite() {
    return Edge(end, start);
}

Edge Edge::stretch(double factor) {
    Edge e = asVector();
    return Edge(start, NPoint(start.di()+e.end.di()*factor,
                              start.dj()+e.end.dj()*factor));
}

double Edge::dist(NPoint q) {
    double x1 = start.di();
    double y1 = start.dj();
    double x2 = end.di();
    double y2 = end.dj();
    double x0 = q.di();
    double y0 = q.dj();
    double top = (y2-y1)*x0-(x2-x1)*y0+x2*y1-y2*x1;
    double bottom = (y2-y1)*(y2-y1)+(x2-x1)*(x2-x1);
    return abs(top)/sqrt(bottom);
}
