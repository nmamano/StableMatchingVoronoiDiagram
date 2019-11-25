#include "triangle.h"
#include "metric.h"

Triangle::Triangle(NPoint v1, NPoint v2, NPoint v3): v1(v1), v2(v2), v3(v3)
{

}

Circle Triangle::circumcircle()
{
    NPoint A = v1, B = v2, C = v3;
    double yDelta_a = B.dj() - A.dj();
    double xDelta_a = B.di() - A.di();
    double yDelta_b = C.dj() - B.dj();
    double xDelta_b = C.di() - B.di();
    double centeri = 0;
    double centerj = 0;

    double aSlope = xDelta_a == 0 ? -1 : yDelta_a/xDelta_a;
    double bSlope = xDelta_b == 0 ? -1 : yDelta_b/xDelta_b;

    NPoint AB_Mid((A.di()+B.di())/2.0, (A.dj()+B.dj())/2.0);
    NPoint BC_Mid((B.di()+C.di())/2.0, (B.dj()+C.dj())/2.0);

    if(yDelta_a == 0) {        //aSlope == 0
        centeri = AB_Mid.di();
        if (xDelta_b == 0) {       //bSlope == INFINITY
            centerj = BC_Mid.dj();
        }
        else {
            centerj = BC_Mid.dj() + (BC_Mid.di()-centeri)/bSlope;
        }
    }
    else if (yDelta_b == 0) {               //bSlope == 0
        centeri = BC_Mid.di();
        if (xDelta_a == 0) {           //aSlope == INFINITY
            centerj = AB_Mid.dj();
        }
        else {
            centerj = AB_Mid.dj() + (AB_Mid.di()-centeri)/aSlope;
        }
    }
    else if (xDelta_a == 0) {       //aSlope == INFINITY
        centerj = AB_Mid.dj();
        centeri = bSlope*(BC_Mid.dj()-centerj) + BC_Mid.di();
    }
    else if (xDelta_b == 0) {       //bSlope == INFINITY
        centerj = BC_Mid.dj();
        centeri = aSlope*(AB_Mid.dj()-centerj) + AB_Mid.di();
    }
    else {
        centeri = (aSlope*bSlope*(AB_Mid.dj()-BC_Mid.dj()) - aSlope*BC_Mid.di() + bSlope*AB_Mid.di())/(bSlope-aSlope);
        centerj = AB_Mid.dj() - (centeri - AB_Mid.di())/aSlope;
    }
    NPoint center(centeri, centerj);
    Metric L2(2);
    double radius = L2.ddist(A, center);
    return Circle(center, radius);
}

bool Triangle::hasVertex(NPoint q)
{
    return q == v1 || q == v2 || q == v3;
}

bool Triangle::hasEdge(Edge e)
{
    return edge1() == e || edge2() == e || edge3() == e;
}

Edge Triangle::edge1()
{
    return Edge(v1, v2);
}

Edge Triangle::edge2()
{
    return Edge(v2, v3);
}

Edge Triangle::edge3()
{
    return Edge(v3, v1);
}
