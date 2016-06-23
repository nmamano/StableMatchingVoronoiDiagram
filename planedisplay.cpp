#include "planedisplay.h"
#include "matchingutils.h"
#include "fastmatcher.h"
#include <iostream>
#include <QtWidgets>
#include <string>
#include <sstream>
#include <iomanip>

PlaneDisplay::PlaneDisplay(int n, int k, QWidget *parent) : QWidget(parent),
    n(n), matcher(n)
{
    setAttribute(Qt::WA_StaticContents);
    moving = false;
    POINT_COLOR = QColor(qRgb(0, 0, 0));
    initRegionColors();

    centers = randomCenters(n, k);
    plane = matcher.query(centers);

    shouldPrintCentroids = true;
    shouldPrintStatistics = true;
    selected_cid = -1;
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

void PlaneDisplay::setGridSize(int newSize)
{
    if (newSize < 2) return;
    matcher.updateP(newSize);
    n = newSize;
    std::vector<Point> newCenters(0);
    for (const Point& c : centers) {
        if (c.i < n and c.j < n)
            newCenters.push_back(c);
    }
    centers = newCenters;
    printScene();
}

void PlaneDisplay::setCenters(const std::vector<Point> &newCenters)
{
    centers = newCenters;
    printScene();
}

void PlaneDisplay::setRandomCenters(int numCenters) {
    centers = randomCenters(n, numCenters);
    printScene();
}

void PlaneDisplay::addCenter(const Point &newCenter)
{
    if (contains(centers, newCenter)) return;
    centers.push_back(newCenter);
    printScene();
}

void PlaneDisplay::removeCenter(int center_id)
{
    if (centers.size() == 1) return;
    centers.erase(centers.begin()+center_id);
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
    int k = centers.size();
    vector<Point> ctroids = centroids(plane, k);
    centers = vector<Point> (0);
    for (Point p : ctroids) {
        while (contains(centers, p)) {
            p = randomAdjacentPoint(p);
        }
        centers.push_back(p);
    }
    printScene();
}

int PlaneDisplay::selectedCenter(QPoint qp) {
    for (int i = 0; i < (int)centers.size(); i++) {
        QPoint cqp = point2QPoint(centers[i]);
        double dis = Point(qp.x(), qp.y()).dist(Point(cqp.x(), cqp.y()));
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
    centers[c_id] = p;
    printScene();
}

bool PlaneDisplay::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    return visibleImage.save(fileName, fileFormat);
}

void PlaneDisplay::setShowCentroids(bool show)
{
     shouldPrintCentroids = show;
     refreshScene();
}

void PlaneDisplay::setShowStatistics(bool show)
{
    shouldPrintStatistics = show;
    refreshScene();
}

void PlaneDisplay::printScene()
{
    updateRegions();
    refreshScene();
}

void PlaneDisplay::refreshScene()
{
    image.fill(qRgb(255, 255, 240));
    printRegions();
    printGrid();
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

void PlaneDisplay::printStatistics() {
    QPainter painter(&image);
    painter.setPen(QColor(qRgb(0, 0, 0)));

    stringstream ss;
    ss << setprecision(3) << fixed;
    ss <<"Grid size: "<<n<<endl;
    ss <<"Num centers: "<<centers.size()<<endl;
    double d1 = avgDistPointCenter(plane, centers);
    ss <<"Point-center dist: "<<d1<<endl;
    double d2 = avgDistCenterCentroid(plane, centers);
    ss <<"Center-centroid dist: "<<d2<<endl;

    string s = ss.str();
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

void PlaneDisplay::printGrid()
{
    if (n > 50) return;
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
    vector<Point> centrs = centroids(plane, k);
    for (int i = 0; i < k; i++) {
        QPoint qp = point2QPoint(centrs[i]);
        painter.setPen(Qt::white);
        painter.setBrush(centerColor(i));
        painter.drawEllipse(qp, CENTROID_RAD, CENTROID_RAD);

    }
}

void PlaneDisplay::updateRegions() {
    plane = matcher.query(centers);
}

void PlaneDisplay::printRegions()
{
    QPainter painter(&image);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QPoint qp = point2QPoint(Point(i,j));
            int center_id = plane[i][j];
            painter.setPen(centerColor(center_id));
            painter.setBrush(centerColor(center_id));
            int r = cellPixSize();
            painter.drawRect(qp.x() - r/2, qp.y() - r/2, r, r);
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


