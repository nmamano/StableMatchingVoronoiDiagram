#ifndef PLANEDISPLAY_H
#define PLANEDISPLAY_H

#include <QWidget>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "num.h"
#include "npoint.h"
#include "matchingutils.h"
#include "circlegrower.h"

using namespace std;

class PlaneDisplay : public QWidget
{
    Q_OBJECT
public:
    PlaneDisplay(int n = 50, int k = 10, QWidget *parent = 0);

    //logic
    void setGridSize(int newN);
    int getGridSize() const { return n; }
    int numPoints() const { return n*n; }

    void setRealCenters(bool useRealCenters);
    bool usingRealCenters() const { return real; }
    void setRandomCenters(int newNumCenters);
    void addCenter(const NPoint &newCenter);
    void removeCenter(int cId);
    int numCenters() const { return centers.size(); }

    void setCentroidWeight(Num weight);
    Num getCentroidWeight() const { return centroidWeight; }

    void moveCentersToCentroids();
    void undoMoves();

    void setMetric(Metric metric);
    Metric getMetric() { return matcher.getMetric(); }

    //gui
    bool saveImage(const QString &fileName, const char *fileFormat) const;

    void setShowCentroids(bool show);
    void setShowStatistics(bool show);
    void setShowIdealPerimeter(bool show);
    void setGraphColoring(bool show);

    void showConstrStep();

protected:

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    //logic
    int n;
    vector<vector<int>> plane;
    CircleGrower matcher;
    void updateRegions();

    vector<NPoint> centers;
    bool real;
    void moveCentersToNearestLatticePoint();

    Num centroidWeight;
    int numCentroidMoves() const;

    void toggleCenter(QPoint qp);
    void moveCenter(int cId, NPoint p);
    static NPoint randomAdjacentPoint(const NPoint &p);

    vector<unsigned int> planeHashHistory;
    void resetPlaneHistory();
    vector<NPoint> firstCenters;

    //GUI
    QImage image;

    void initRegionColors();
    vector<QColor> regionColors;
    int numColors() const;

    QColor centerColor(int centerId) const;
    bool enableGraphColoring;
    vector<int> graphColoringColors;

    vector<int> graphColoring() const;
    vector<unordered_set<int> > findNeighborRegions() const;
    static int leastUsedColor(const vector<int> &usages, const vector<bool> &allowedColors);


    const int CENTER_RAD = 5;
    const int CENTROID_RAD = 3;
    const int POINT_RAD = 1;
    QColor POINT_COLOR;
    QColor EMPTY_COLOR;
    QColor BOUNDARY_COLOR;

    //printing
    void printScene();
    void refreshSceneAndColors();
    void refreshScene();

    void printLatticePoints();
    void printCenters();
    void printBoundaries();

    void printRegions(const vector<vector<int> > &currPlane);

    void printCentroids();
    bool shouldPrintCentroids;

    void printStatistics();
    bool shouldPrintStatistics;
    void printBoxedText(string s);
    static int numLines(const string &s);
    static string longestLine(const string &s);

    void printIdealPerimeters();
    bool shouldPrintIdealPerimeter;
    static double areaToRad(double area);
    static double areaToSquareDiag(double area);


    void printConstrScene();
    vector<vector<int>> constrPlane;
    int constrIter;

    QPoint npoint2QPoint(NPoint p) const;
    QPointF npoint2QPointf(NPoint p) const;
    NPoint qpointToNPoint(QPoint qp, bool realPoint) const;
    int cellPixSize() const;
    double dcellPixSize() const;

    //mouse
    bool mouseMoving;
    bool mouseMoved;
    QPoint lastPointPointedByMouse;

    int selectedCenter(QPoint qp);
    int selectedCId;


};

#endif // PLANEDISPLAY_H
