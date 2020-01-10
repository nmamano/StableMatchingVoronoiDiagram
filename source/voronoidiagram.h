#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H
#include "edge.h"
#include "triangle.h"
#include "qimage.h"

class VoronoiDiagram
{
public:
    vector<Edge> voronoiEdges;
    vector<Edge> delaunayEdges;
    vector<Triangle> delaunayTriangles;
    vector<Triangle> superTTriangles;
    vector<NPoint> sites;
    int numVoronoiEdges() {return voronoiEdges.size();}
    int numDelaunayEdges() {return delaunayEdges.size();}
    VoronoiDiagram(vector<NPoint> sites);
    void triangulate(const vector<NPoint> &sites);
    void delaunayToVoronoi();
private:
    Triangle superTriangle(const vector<NPoint> &sites);
};

#endif // VORONOIDIAGRAM_H
