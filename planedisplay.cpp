#include "planedisplay.h"
#include "matchingutils.h"
#include "fastmatcher.h"
#include <iostream>
#include <QtWidgets>

PlaneDisplay::PlaneDisplay(int n, int k, QWidget *parent) : QWidget(parent),
    n(n), matcher(n)
{
    setAttribute(Qt::WA_StaticContents);
    moving = false;
    POINT_COLOR = QColor(qRgb(0, 0, 0));
    initRegionColors();

    centers = randomCenters(n, k);
    plane = matcher.query(centers);
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

void PlaneDisplay::toggleCenter(QPoint qp) {
    Point p = QPointToPoint(qp);
    if (p.i < 0 or p.i >= n or p.j < 0 or p.j >= n)
        return;

    for (int i = 0; i < (int)centers.size(); i++) {
        if (p == centers[i]) {
            removeCenter(i);
            return;
        }
    }
    addCenter(p);
}

bool PlaneDisplay::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    return visibleImage.save(fileName, fileFormat);
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
        lastPoint = event->pos();
        moving = true;
    }
}

void PlaneDisplay::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && moving) {

    }
}

void PlaneDisplay::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && moving) {
        moving = false;
        toggleCenter(event->pos());
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


