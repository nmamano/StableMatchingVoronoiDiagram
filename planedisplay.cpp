#include "planedisplay.h"
#include <iostream>
#include <QtWidgets>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "metric.h"

using namespace std;

PlaneDisplay::PlaneDisplay(int n, int k, QWidget *parent) : QWidget(parent),
    n(n), matcher(n)
{
    setAttribute(Qt::WA_StaticContents);
    moving = false;
    POINT_COLOR = QColor(qRgb(0, 0, 0));
    EMPTY_COLOR = QColor(qRgb(255, 255, 255));
    BOUNDARY_COLOR = QColor(qRgb(0, 0, 0));

    initRegionColors();

    centers = randomCenters(n, k);
    plane = matcher.query(centers);

    shouldPrintCentroids = true;
    shouldPrintStatistics = true;
    shouldPrintIdealPerimeter = false;

    selected_cid = -1;

    constrIter = -1;
    moveCentersIter = 0;
}


void PlaneDisplay::initRegionColors() {
    region_colors.push_back(QColor(qRgb(240,163,255)));
    region_colors.push_back(QColor(qRgb(0,117,220)));
    region_colors.push_back(QColor(qRgb(153,63,0)));
    region_colors.push_back(QColor(qRgb(76,0,92)));
    region_colors.push_back(QColor(qRgb(25,25,25)));
    region_colors.push_back(QColor(qRgb(0,92,49)));
    region_colors.push_back(QColor(qRgb(43,206,72)));
    region_colors.push_back(QColor(qRgb(255,204,153)));
    region_colors.push_back(QColor(qRgb(128,128,128)));
    region_colors.push_back(QColor(qRgb(148,255,181)));
    region_colors.push_back(QColor(qRgb(143,124,0)));
    region_colors.push_back(QColor(qRgb(157,204,0)));
    region_colors.push_back(QColor(qRgb(194,0,136)));
    region_colors.push_back(QColor(qRgb(0,51,128)));
    region_colors.push_back(QColor(qRgb(255,164,5)));
    region_colors.push_back(QColor(qRgb(255,168,187)));
    region_colors.push_back(QColor(qRgb(66,102,0)));
    region_colors.push_back(QColor(qRgb(255,0,16)));
    region_colors.push_back(QColor(qRgb(94,241,242)));
    region_colors.push_back(QColor(qRgb(0,153,143)));
    region_colors.push_back(QColor(qRgb(224,255,102)));
    region_colors.push_back(QColor(qRgb(116,10,255)));
    region_colors.push_back(QColor(qRgb(153,0,0)));
    region_colors.push_back(QColor(qRgb(255,255,128)));
    region_colors.push_back(QColor(qRgb(255,255,0)));
    region_colors.push_back(QColor(qRgb(255,80,5)));
}

void PlaneDisplay::setGridSize(int newN)
{
    if (newN == n) return;
    if (newN < 2) return;
    matcher.setN(newN);
    n = newN;
    vector<Point> newCenters(0);
    for (const Point& c : centers) {
        if (c.i < n and c.j < n)
            newCenters.push_back(c);
    }
    if (newCenters.empty()) {
        //at least one center
        newCenters = randomCenters(n, 1);
    }
    centers = newCenters;
    moveCentersIter = 0;
    printScene();
}

void PlaneDisplay::setMetric(Metric metric)
{
    if (matcher.getMetric() == metric) return;
    matcher.setMetric(metric);
    moveCentersIter = 0;
    printScene();
}

void PlaneDisplay::showConstrStep()
{
    if (constrIter == -1 or
            numAssignedPoints(constrPlane) == n*n) {
        constrPlane = vector<vector<int>> (n, vector<int> (n, -1));
        constrIter = 0;
    }
    matcher.step(constrPlane, centers, constrIter);
    constrIter++;
    printConstrScene();
}

void PlaneDisplay::setRandomCenters(int numCenters) {
    centers = randomCenters(n, numCenters);
    moveCentersIter = 0;
    printScene();
}

void PlaneDisplay::addCenter(const Point &newCenter)
{
    if (contains(centers, newCenter)) return;
    centers.push_back(newCenter);
    moveCentersIter = 0;
    printScene();
}

void PlaneDisplay::removeCenter(int center_id)
{
    if (centers.size() == 1) return;
    centers.erase(centers.begin()+center_id);
    moveCentersIter = 0;
    printScene();
}

Point PlaneDisplay::randomAdjacentPoint(const Point& p) {
    int dir = qrand()%2;
    int offset = dir == 0? 1 : -1;
    int coord = qrand()%2;
    if (coord == 0) return Point(p.i+offset, p.j);
    return Point(p.i, p.j+offset);
}

void PlaneDisplay::moveCentersToCentroids()
{
    cout << "move centers to centroids " << endl;
    int k = centers.size();
    vector<Point> ctroids = weightedCentroids(plane, k, centers, matcher.getMetric(), centroidWeight);
    centers = vector<Point> (0);
    for (Point p : ctroids) {
        while (contains(centers, p)) {
            p = randomAdjacentPoint(p);
        }
        centers.push_back(p);
    }
    moveCentersIter++;
    printScene();
}

int PlaneDisplay::selectedCenter(QPoint qp) {
    for (int i = 0; i < (int)centers.size(); i++) {
        QPoint cqp = point2QPoint(centers[i]);
        Metric eucMetric(2);
        double dis = eucMetric.ddist(Point(qp.x(), qp.y()), Point(cqp.x(), cqp.y()));
        if (dis <= CENTER_RAD) return i;
    }
    return -1;
}

void PlaneDisplay::toggleCenter(QPoint qp) {
    int c_id = selectedCenter(qp);
    if (c_id == -1) {
        Point p = QPointToPoint(qp);
        if (p.i < 0 or p.i >= n or p.j < 0 or p.j >= n)
            return;
        addCenter(p);
    } else {
        removeCenter(c_id);
    }
}

void PlaneDisplay::moveCenter(int c_id, Point p) {
    if (contains(centers, p)) return;
    if (p.i < 0 or p.i >= n or p.j < 0 or p.j >= n)
        return;
    centers[c_id] = p;
    moveCentersIter = 0;
    printScene();
}

bool PlaneDisplay::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    return visibleImage.save(fileName, fileFormat);
}

void PlaneDisplay::setShowCentroids(bool show)
{
     if (shouldPrintCentroids == show) return;
     shouldPrintCentroids = show;
     refreshScene();
}

void PlaneDisplay::setCentroidWeight(Num weight) {
    if (centroidWeight == weight) return;
    centroidWeight = weight;
    moveCentersIter = 0;
    refreshScene();
}

void PlaneDisplay::setShowStatistics(bool show)
{
    if (shouldPrintStatistics == show) return;
    shouldPrintStatistics = show;
    refreshScene();
}

void PlaneDisplay::setShowIdealPerimeter(bool show)
{
    if (shouldPrintIdealPerimeter == show) return;
    shouldPrintIdealPerimeter = show;
    refreshScene();
}

void PlaneDisplay::printScene()
{
    constrIter = -1;
    updateRegions();
    refreshScene();
}

void PlaneDisplay::printConstrScene() {
    image.fill(qRgb(255, 255, 240));
    printRegions(constrPlane);
    if (n <= 50) printGrid();
    printCenters();
    update();
}

void PlaneDisplay::refreshScene()
{
    cout << "refresh scene" << endl;
    image.fill(qRgb(255, 255, 240));
    printRegions(plane);
    printBoundaries();
    if (n <= 50) printGrid();
    if (shouldPrintIdealPerimeter) printIdealPerimeters();
    printCenters();
    if (shouldPrintCentroids) printCentroids();
    if (shouldPrintStatistics) printStatistics();
    update();
}

QPoint PlaneDisplay::point2QPoint(Point p) {
    int x = round(((double)p.i/n)*image.size().width());
    int y = round(((double)p.j/n)*image.size().height());
    x += cellPixSize()/2;
    y += cellPixSize()/2;
    return QPoint(x,y);
}

Point PlaneDisplay::QPointToPoint(QPoint p)
{
    double dx = (double)(p.x()-cellPixSize()/2)/image.size().width();
    double dy = (double)(p.y()-cellPixSize()/2)/image.size().height();
    dx = round(dx*n);
    dy = round(dy*n);
    return Point(dx, dy);
}

int PlaneDisplay::cellPixSize() {
    return image.size().width()/n;
}

QColor PlaneDisplay::centerColor(int centerId) {
    return region_colors[centerId%region_colors.size()];
}

int PlaneDisplay::numLines(const string& s) {
    int numlines = 0;
    for (char c : s) {
        if (c == '\n') {
            numlines++;
        }
    }
    return numlines;
}

string PlaneDisplay::longestLine(const string& s) {
    int width = 0;
    int maxwidth = 0;
    int linestartpos = 0;
    int longestlinestartpos = 0;
    int index = 0;
    for (char c : s) {
        if (c == '\n') {
            if (width > maxwidth) {
                maxwidth = width;
                longestlinestartpos = linestartpos;
            }
            width = 0;
            linestartpos = index+1;
        } else {
            width++;
        }
        index++;
    }
    return s.substr(longestlinestartpos, maxwidth);
}

void PlaneDisplay::printBoxedText(string s) {
    QPainter painter(&image);
    painter.setPen(QColor(qRgb(0, 0, 0)));

    QString qs = QString::fromStdString(s);
    QFont font("Consolas", 14);
    painter.setFont(font);

    QFontMetrics fm(font);
    int sheight = fm.height()*numLines(s);
    int swidth = 5+fm.width(QString::fromStdString(longestLine(s)));
    QRect rect(QPoint(20, 20), QSize(swidth, sheight));

    painter.fillRect(rect, QBrush(QColor(255, 255, 255, 220)));
    painter.drawRect(rect);
    rect = QRect(QPoint(22, 20), QSize(swidth, sheight));
    painter.drawText(rect, qs);
}

void PlaneDisplay::printStatistics() {

    stringstream ss;
    ss << setprecision(3) << fixed;
    ss <<"Metric: L_"<<matcher.getMetric()<<endl;
    ss <<"Grid size: "<<n<<endl;
    ss <<"Num centers: "<<centers.size()<<endl;
    Metric metric = matcher.getMetric();
    double d1 = avgDistPointCenter(plane, centers, metric);
    ss <<"Iteration: "<<moveCentersIter<<endl;
    ss <<"Point-center dist: "<<d1<<endl;
    double d2 = avgDistCenterCentroid(plane, centers, metric, centroidWeight);
    ss <<"Center-centroid dist: "<<d2<<endl;
    ss <<"Weight (p): "<<centroidWeight.toStr()<<endl;

    string s = ss.str();
    printBoxedText(s);
}

void PlaneDisplay::printGrid()
{
    QPainter painter(&image);
    painter.setPen(POINT_COLOR);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QPoint qp = point2QPoint(Point(i,j));
            painter.drawEllipse(qp, POINT_RAD, POINT_RAD);
        }
    }
}

void PlaneDisplay::printCenters()
{
    QPainter painter(&image);
    int k = centers.size();
    for (int i = 0; i < k; i++) {
        QPoint qp = point2QPoint(centers[i]);
        painter.setPen(Qt::black);
        painter.setBrush(centerColor(i));
        painter.drawEllipse(qp, CENTER_RAD, CENTER_RAD);
    }
}

void PlaneDisplay::printCentroids()
{
    QPainter painter(&image);
    int k = centers.size();
    vector<Point> centrds = weightedCentroids(plane, k, centers, matcher.getMetric(), centroidWeight);
    for (int i = 0; i < k; i++) {
        QPoint qp = point2QPoint(centrds[i]);
        painter.setPen(Qt::white);
        painter.setBrush(centerColor(i));
        painter.drawEllipse(qp, CENTROID_RAD, CENTROID_RAD);

    }
}

double PlaneDisplay::areaToRad(double area) {
    double pi = 4*atan(1);
    return sqrt(area/pi);
}

double PlaneDisplay::areaToSquareDiag(double area) {
    double side = sqrt(area);
    return side*sqrt(2);
}

void PlaneDisplay::printIdealPerimeters() {
    QPainter painter(&image);
    int k = centers.size();
    vector<int> quotas = centerQuotas(n, k);
    for (int c_id = 0; c_id < k; c_id++) {
        Point c = centers[c_id];
        QPoint qc = point2QPoint(c);
        double area = quotas[c_id];
        Metric metric = matcher.getMetric();
        if (metric.val == 2) {
            double rad = areaToRad(area);
            int qRad = round(((double)rad/n)*image.size().width());
            painter.setPen(QPen(POINT_COLOR, 2));
            painter.drawEllipse(qc, qRad-2, qRad-2);
            painter.drawEllipse(qc, qRad+2, qRad+2);
            painter.setPen(QPen(centerColor(c_id), 2));
            painter.drawEllipse(qc, qRad, qRad);
        } else if (metric.val == 1) {
            double rad = areaToSquareDiag(area)/2;
            int qRad = round(((double)rad/n)*image.size().width());
            QVector<QPoint> points;
            points.push_back(QPoint(qc.x()-qRad, qc.y()));
            points.push_back(QPoint(qc.x(), qc.y()-qRad));
            points.push_back(QPoint(qc.x()+qRad, qc.y()));
            points.push_back(QPoint(qc.x(), qc.y()+qRad));
            QPolygon pol(points);
            painter.setPen(QPen(POINT_COLOR, 2));
            pol[0].rx()-=2; pol[1].ry()-=2; pol[2].rx()+=2; pol[3].ry()+=2;
            painter.drawConvexPolygon(pol);
            pol[0].rx()+=4; pol[1].ry()+=4; pol[2].rx()-=4; pol[3].ry()-=4;
            painter.drawConvexPolygon(pol);
            painter.setPen(QPen(centerColor(c_id), 2));
            pol[0].rx()-=2; pol[1].ry()-=2; pol[2].rx()+=2; pol[3].ry()+=2;
            painter.drawConvexPolygon(pol);
        } else if (metric.isInf()) {
            double side = sqrt(area);
            int qSide = round(((double)side/n)*image.size().width());
            int x = qc.x()-qSide/2, y = qc.y()-qSide/2;
            painter.setPen(QPen(POINT_COLOR, 2));
            painter.drawRect(x-2, y-2, qSide+4, qSide+4);
            painter.drawRect(x+2, y+2, qSide-4, qSide-4);
            painter.setPen(QPen(centerColor(c_id), 2));
            painter.drawRect(x, y, qSide, qSide);
        } else {
            //not available
        }
    }
}

void PlaneDisplay::updateRegions() {
    cout << "update regions" << endl;
    plane = matcher.query(centers);
}

void PlaneDisplay::printRegions(const vector<vector<int>>& curr_plane)
{
    QPainter painter(&image);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QPoint qp = point2QPoint(Point(i,j));
            int center_id = curr_plane[i][j];
            if (center_id == -1) {
                painter.setPen(EMPTY_COLOR);
                painter.setBrush(EMPTY_COLOR);
            } else {
                painter.setPen(centerColor(center_id));
                painter.setBrush(centerColor(center_id));
            }
            int r = cellPixSize();
            painter.drawRect(qp.x() - r/2, qp.y() - r/2, r, r);
        }
    }
}

void PlaneDisplay::printBoundaries() {
    QPainter painter(&image);
    painter.setPen(BOUNDARY_COLOR);
    painter.setBrush(BOUNDARY_COLOR);
    int r = cellPixSize();
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-1; j++) {
            QPoint qp = point2QPoint(Point(i, j));
            if (plane[i][j] != plane[i][j+1]) {
                QPoint left(qp.x()-r/2, qp.y()+r/2);
                QPoint right(qp.x()+r/2, qp.y()+r/2);
                painter.drawLine(left, right);
            }
            if (plane[i][j] != plane[i+1][j]) {
                QPoint top(qp.x()+r/2, qp.y()-r/2);
                QPoint bottom(qp.x()+r/2, qp.y()+r/2);
                painter.drawLine(top, bottom);
            }
        }
    }
}

void PlaneDisplay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        selected_cid = selectedCenter(event->pos());
        lastPoint = event->pos();
        moving = true;
        moved = false;
    }
}

void PlaneDisplay::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && moving) {
        if (selected_cid != -1) {
            moveCenter(selected_cid, QPointToPoint(event->pos()));
            moved = true;
        }
    }
}

void PlaneDisplay::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && moving) {
        if (not moved) {
            toggleCenter(event->pos());
        }
        selected_cid = -1;
        moving = false;
        moved = false;
    }
}

void PlaneDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void PlaneDisplay::resizeEvent(QResizeEvent *event)
{
    int sideLength = qMin(width(), height());
    //if (abs(sideLength-image.size().width()) < 5) return;
    QSize newSize(sideLength, sideLength);
    QImage newImage(newSize, QImage::Format_RGB32);
    image = newImage;
    QWidget::resizeEvent(event);
    refreshScene();
}

int PlaneDisplay::numPoints() {
    return n*n;
}


