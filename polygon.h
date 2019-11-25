#ifndef POLYGON_H
#define POLYGON_H
#include<vector>
#include "edge.h"
using namespace std;

class Polygon
{
public:
//    vector<NPoint> points; //ccw order
    vector<Edge> edges; //no order
    Polygon();
    Polygon(const vector<Edge> edges): edges(edges) {}
};

#endif // POLYGON_H
