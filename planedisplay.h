#ifndef PLANEDISPLAY_H
#define PLANEDISPLAY_H

#include <QWidget>
#include <vector>
#include <iostream>
#include "num.h"
#include "matchingutils.h"
#include "diskgrower.h"

class PlaneDisplay : public QWidget
{
    Q_OBJECT
public:
    PlaneDisplay(int n = 300, int k = 40, QWidget *parent = 0);

    void setGridSize(int newN);
    void setRandomCenters(int numCenters);
    void addCenter(const Point& newCenter);
    void removeCenter(int center_id);
    void moveCentersToCentroids();

    bool saveImage(const QString &fileName, const char *fileFormat);

    void setShowCentroids(bool show);
    void setShowStatistics(bool show);
    void setShowIdealPerimeter(bool show);

    int getGridSize() { return n; }
    int getNumCenters() { return centers.size(); }
    QString getCentroidWeight() { return centroidWeight.toQstr(); }
    int numPoints();

    void setMetric(Metric metric);

    void showConstrStep();

    void setCentroidWeight(Num weight);

public slots:
protected:

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void printScene();
    void refreshScene();

    void initRegionColors();
    std::vector<QColor> region_colors;
    QColor centerColor(int centerId);

    const int CENTER_RAD = 5;
    const int CENTROID_RAD = 3;
    const int POINT_RAD = 1;
    QColor POINT_COLOR;
    QColor EMPTY_COLOR;
    QColor BOUNDARY_COLOR;

    int n;
    std::vector<std::vector<int>> plane;
    std::vector<Point> centers;

    QPoint point2QPoint(Point p);\
    Point QPointToPoint(QPoint p);
    int cellPixSize();

    void printGrid();
    void printCenters();
    void printRegions(const vector<vector<int> > &curr_plane);

    DiskGrower matcher;
    void updateRegions();

    bool moving;
    bool moved;
    QImage image;
    QPoint lastPoint;

    bool shouldPrintCentroids;
    bool shouldPrintStatistics;
    bool shouldPrintIdealPerimeter;

    void toggleCenter(QPoint qp);

    void printCentroids();
    Point randomAdjacentPoint(const Point &p);
    int selectedCenter(QPoint qp);

    int selected_cid;

    void moveCenter(int c_id, Point p);
    void printStatistics();
    int numLines(const string &s);
    string longestLine(const string &s);

    vector<vector<int>> constrPlane;
    int constrIter;

    void printConstrScene();
    void printIdealPerimeters();
    double areaToRad(double area);
    double areaToSquareDiag(double area);

    Num centroidWeight;
    int moveCentersIter;

    void printBoxedText(string s);
    void printBoundaries();
signals:

};

#endif // PLANEDISPLAY_H
