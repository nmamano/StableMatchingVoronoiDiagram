#ifndef PLANEDISPLAY_H
#define PLANEDISPLAY_H

#include <QWidget>
#include <vector>
#include "matchingutils.h"
#include "fastmatcher.h"

class PlaneDisplay : public QWidget
{
    Q_OBJECT
public:
    PlaneDisplay(int n = 200, int k = 100, QWidget *parent = 0);

    void setGridSize(int newSize);
    void setCenters(const std::vector<Point>& centers);
    void setRandomCenters(int numCenters);
    void addCenter(const Point& newCenter);
    void removeCenter(int center_id);

    bool saveImage(const QString &fileName, const char *fileFormat);

    int getGridSize() { return n; }
    int getNumCenters() { return centers.size(); }
    int numPoints();

    void init();
public slots:
    void printScene();
    void refreshScene();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void initRegionColors();
    std::vector<QColor> region_colors;
    QColor centerColor(int centerId);

    const int CENTER_RAD = 5;
    const int POINT_RAD = 1;
    QColor POINT_COLOR;

    int n;
    std::vector<std::vector<int>> plane;
    std::vector<Point> centers;

    QPoint point2QPoint(Point p);\
    Point QPointToPoint(QPoint p);
    int cellPixSize();

    void printGrid();
    void printCenters();
    void printRegions();

    FastMatcher matcher;
    void updateRegions();

    bool moving;
    QImage image;
    QPoint lastPoint;

    void toggleCenter(QPoint qp);

signals:

};

#endif // PLANEDISPLAY_H
