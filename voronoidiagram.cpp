#include "voronoidiagram.h"
#include "npoint.h"
#include "triangle.h"
#include "edge.h"
#include <set>

using namespace std;

VoronoiDiagram::VoronoiDiagram(vector<NPoint> sites): sites(sites)
{
    triangulate(sites);
    delaunayToVoronoi();

}

Triangle VoronoiDiagram::superTriangle(const vector<NPoint>& sites) {
    double minx = sites[0].di();
    double miny = sites[0].dj();
    double maxx = sites[0].di();
    double maxy = sites[0].dj();
    for (int i = 1; i < (int)sites.size(); i++) {
        NPoint v = sites[i];
        if (v.di() < minx) minx = v.di();
        if (v.di() > maxx) maxx = v.di();
        if (v.dj() < miny) miny = v.dj();
        if (v.dj() > maxy) maxy = v.dj();
    }
    int CONST = 1000;
    NPoint superV1 = NPoint(minx-CONST, miny-CONST);
    NPoint superV2 = NPoint(minx-CONST, 2*maxy+CONST);
    NPoint superV3 = NPoint(2*maxx+CONST, miny-CONST);
    return Triangle (superV1, superV2, superV3);
}

void VoronoiDiagram::triangulate(const vector<NPoint>& sites) {
//    subroutine triangulate
//    input : vertex list
    vector<NPoint> vertexList(sites);
//    output : triangle list
    delaunayTriangles = vector<Triangle> (0);
//       initialize the triangle list
//       determine the supertriangle
    Triangle superT = superTriangle(vertexList);
//       add supertriangle vertices to the end of the vertex list
    vertexList.push_back(superT.v1);
    vertexList.push_back(superT.v2);
    vertexList.push_back(superT.v3);
//       add the supertriangle to the triangle list
    delaunayTriangles.push_back(superT);
//       for each sample point in the vertex list
    for (int i = 0; i < (int)vertexList.size(); i++) {
        NPoint vertex = vertexList[i];
//          initialize the edge buffer
        vector<Edge> edgeBuffer(0);
//          for each triangle currently in the triangle list
        vector<Triangle> newTriangleList(0);
        for (int j = 0; j < (int)delaunayTriangles.size(); j++) {
            Triangle triangle = delaunayTriangles[j];
//             calculate the triangle circumcircle center and radius
            Circle circumcircle = triangle.circumcircle();
//             if the point lies in the triangle circumcircle then
            if (circumcircle.contains(vertex)) {
//                add the three triangle edges to the edge buffer
                edgeBuffer.push_back(triangle.edge1());
                edgeBuffer.push_back(triangle.edge2());
                edgeBuffer.push_back(triangle.edge3());
//                remove the triangle from the triangle list
            } else {
                newTriangleList.push_back(triangle);
//             endif
            }
//          endfor
        }
        delaunayTriangles = newTriangleList;
//          delete all doubly specified edges from the edge buffer
        vector<Edge> newEdgeBuffer(0);
        for (int j = 0; j < (int)edgeBuffer.size(); j++) {
            bool repeated = false;
            for (int k = 0; k < (int)edgeBuffer.size(); k++) {
                if (j != k and edgeBuffer[j] == edgeBuffer[k]) {
                    repeated = true;
                }
            }
            if (not repeated) newEdgeBuffer.push_back(edgeBuffer[j]);
        }
        edgeBuffer = newEdgeBuffer;
//             this leaves the edges of the enclosing polygon only
//          add to the triangle list all triangles formed between the point
//             and the edges of the enclosing polygon
        for (int j = 0; j < (int)edgeBuffer.size(); j++) {
            Edge edge = edgeBuffer[j];
            Triangle newTriangle(vertex, edge.start, edge.end);
            delaunayTriangles.push_back(newTriangle);
        }
//       endfor
    }
//       remove any triangles from the triangle list that use the supertriangle vertices
    vector<Triangle> aux(0);
    superTTriangles = vector<Triangle>(0);
    for (int i = 0; i < (int)delaunayTriangles.size(); i++) {
        Triangle triangle = delaunayTriangles[i];
        if (!triangle.hasVertex(superT.v1) &&
            !triangle.hasVertex(superT.v2) &&
            !triangle.hasVertex(superT.v3)) {
            aux.push_back(triangle);
        } else {
            superTTriangles.push_back(triangle);
        }
    }
    delaunayTriangles = aux;
//       remove the supertriangle vertices from the vertex list
//    end
    delaunayEdges = vector<Edge>();
    for (int i = 0; i < (int)delaunayTriangles.size(); i++) {
        Triangle triangle = delaunayTriangles[i];
        delaunayEdges.push_back(triangle.edge1());
        delaunayEdges.push_back(triangle.edge2());
        delaunayEdges.push_back(triangle.edge3());
    }
    vector<Edge> newDelaunayEdges(0);
    for (int i = 0; i < (int)delaunayEdges.size(); i++) {
        bool repeated = false;
        for (int j = 0; j < i; j++) {
            if (delaunayEdges[i] == delaunayEdges[j]) {
                repeated = true;
            }
        }
        if (not repeated) newDelaunayEdges.push_back(delaunayEdges[i]);
    }
    delaunayEdges = newDelaunayEdges;
}

void VoronoiDiagram::delaunayToVoronoi() {
    //to find the dual Voronoi diagram, compute the
    //circumcentres of all the triangles, and connect any
    //two circumcentres whose triangles share an edge
    voronoiEdges = vector<Edge> (0);
    int n = delaunayTriangles.size();
    for (int i = 0; i < n; i++) {
        Triangle ti = delaunayTriangles[i];
        Circle ci = ti.circumcircle();
        NPoint pi = ci.center;
        for (int j = 0; j < i; j++) {
            Triangle tj = delaunayTriangles[j];
            if (ti.hasEdge(tj.edge1()) ||
                ti.hasEdge(tj.edge2()) ||
                ti.hasEdge(tj.edge3())) {
                Circle cj = tj.circumcircle();
                NPoint pj = cj.center;
                voronoiEdges.push_back(Edge(pi, pj));
            }
        }
        for (int j = 0; j < (int)superTTriangles.size(); j++) {
            Triangle tj = superTTriangles[j];
            if (ti.hasEdge(tj.edge1()) ||
                ti.hasEdge(tj.edge2()) ||
                ti.hasEdge(tj.edge3())) {
                Circle cj = tj.circumcircle();
                NPoint pj = cj.center;
                voronoiEdges.push_back(Edge(pi, pj).stretch(1000));
            }
        }
    }
}
