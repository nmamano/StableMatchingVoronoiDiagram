#include "planedisplay.h"
#include <iostream>
#include <QtWidgets>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <iomanip>
#include "metric.h"

using namespace std;

PlaneDisplay::PlaneDisplay(int n, int k, QWidget *parent) : QWidget(parent),
    n(n), matcher(n)
{
    setAttribute(Qt::WA_StaticContents);
    mouseMoving = false;
    POINT_COLOR = QColor(qRgb(0, 0, 0));
    EMPTY_COLOR = QColor(qRgb(255, 255, 255));
    BOUNDARY_COLOR = QColor(qRgb(0, 0, 0));

    initRegionColors();

    realCenters = false;
    centers = randomCenters(n, k, realCenters);
    updateRegions();

    shouldPrintCentroids = true;
    shouldPrintStatistics = true;
    shouldPrintIdealPerimeter = false;

    selectedCId = -1;

    constrIter = -1;
    numCentroidMoves = 0;
}

void PlaneDisplay::updateRegions() {
    if (realCenters) {
        vector<DPoint>dcenters = doublePoints(centers);
        plane = matcher.realCentersQuery(dcenters);
    } else {
        plane = matcher.intCentersQuery(intPoints(centers));
    }
}

void PlaneDisplay::initRegionColors() {
    regionColors.push_back(QColor(qRgb(240,163,255)));
    regionColors.push_back(QColor(qRgb(0,117,220)));
    regionColors.push_back(QColor(qRgb(153,63,0)));
    regionColors.push_back(QColor(qRgb(76,0,92)));
    regionColors.push_back(QColor(qRgb(25,25,25)));
    regionColors.push_back(QColor(qRgb(0,92,49)));
    regionColors.push_back(QColor(qRgb(43,206,72)));
    regionColors.push_back(QColor(qRgb(255,204,153)));
    regionColors.push_back(QColor(qRgb(128,128,128)));
    regionColors.push_back(QColor(qRgb(148,255,181)));
    regionColors.push_back(QColor(qRgb(143,124,0)));
    regionColors.push_back(QColor(qRgb(157,204,0)));
    regionColors.push_back(QColor(qRgb(194,0,136)));
    regionColors.push_back(QColor(qRgb(0,51,128)));
    regionColors.push_back(QColor(qRgb(255,164,5)));
    regionColors.push_back(QColor(qRgb(255,168,187)));
    regionColors.push_back(QColor(qRgb(66,102,0)));
    regionColors.push_back(QColor(qRgb(255,0,16)));
    regionColors.push_back(QColor(qRgb(94,241,242)));
    regionColors.push_back(QColor(qRgb(0,153,143)));
    regionColors.push_back(QColor(qRgb(224,255,102)));
    regionColors.push_back(QColor(qRgb(116,10,255)));
    regionColors.push_back(QColor(qRgb(153,0,0)));
    regionColors.push_back(QColor(qRgb(255,255,128)));
    regionColors.push_back(QColor(qRgb(255,255,0)));
    regionColors.push_back(QColor(qRgb(255,80,5)));
}

void PlaneDisplay::setGridSize(int newN)
{
    if (newN == n) return;
    if (newN < 2) return;
    matcher.setN(newN);
    n = newN;
    vector<NPoint> newCenters(0);
    for (const NPoint& c : centers) {
        if (c.di() <= n-1 and c.dj() <= n-1)
            newCenters.push_back(c);
    }
    if (newCenters.empty()) {
        //at least one center
        newCenters = randomCenters(n, 1, realCenters);
    }
    centers = newCenters;
    numCentroidMoves = 0;
    printScene();
}

void PlaneDisplay::setRealCenters(bool useRealCenters)
{
    if (useRealCenters == realCenters) return;
    realCenters = useRealCenters;
    if (!realCenters) {
        moveCentersToNearestLatticePoint();
    }
    numCentroidMoves = 0;
    printScene();
}

void PlaneDisplay::moveCentersToNearestLatticePoint() {
    vector<NPoint> newCenters(0);
    for (int i = 0; i < numCenters(); i++) {
        NPoint p(centers[i].i.round(), centers[i].j.round());
        while (contains(newCenters, p)) {
            p = randomAdjacentPoint(p);
        }
        newCenters.push_back(p);
    }
    centers = newCenters;
}

void PlaneDisplay::setMetric(Metric metric)
{
    if (matcher.getMetric() == metric) return;
    matcher.setMetric(metric);
    numCentroidMoves = 0;
    printScene();
}

void PlaneDisplay::showConstrStep()
{
    if (realCenters) return;

    if (constrIter == -1 or
            numAssignedPoints(constrPlane) == n*n) {
        constrPlane = vector<vector<int>> (n, vector<int> (n, -1));
        constrIter = 0;
    }
    matcher.step(constrPlane, intPoints(centers), constrIter);
    constrIter++;
    printConstrScene();
}

void PlaneDisplay::setRandomCenters(int newNumCenters) {
    centers = randomCenters(n, newNumCenters, realCenters);
    numCentroidMoves = 0;
    printScene();
}

void PlaneDisplay::addCenter(const NPoint &newCenter)
{
    if (!realCenters && contains(centers, newCenter)) return;
    centers.push_back(newCenter);
    numCentroidMoves = 0;
    printScene();
}

void PlaneDisplay::removeCenter(int cId)
{
    if (centers.size() == 1) return;
    centers[cId] = centers[numCenters()-1];
    centers.pop_back();
    numCentroidMoves = 0;
    printScene();
}

NPoint PlaneDisplay::randomAdjacentPoint(const NPoint& p) {
    int dir = qrand()%2;
    int offset = dir == 0? 1 : -1;
    int coord = qrand()%2;
    if (coord == 0) return NPoint(p.di()+offset, p.dj());
    return NPoint(p.di(), p.dj()+offset);
}

void PlaneDisplay::moveCentersToCentroids()
{
    vector<NPoint> ctroids = weightedCentroids(plane, centers, matcher.getMetric(), centroidWeight, realCenters);
    if (areEqual(centers, ctroids)) {
        QMessageBox::information(
            this,
            tr("Stable grid matching"),
            tr("The centers have converged."));
        return;
    }

    centers = vector<NPoint> (0);
    for (NPoint p : ctroids) {
        while (!realCenters && contains(centers, p)) {
            p = randomAdjacentPoint(p);
        }
        centers.push_back(p);
    }
    numCentroidMoves++;
    printScene();
}

int PlaneDisplay::selectedCenter(QPoint qp) {
    for (int i = 0; i < numCenters(); i++) {
        QPoint cqp = npoint2QPoint(centers[i]);
        Metric eucMetric(2);
        double dis = eucMetric.ddist(Point(qp.x(), qp.y()), Point(cqp.x(), cqp.y()));
        if (dis <= CENTER_RAD) return i;
    }
    return -1;
}

void PlaneDisplay::toggleCenter(QPoint qp) {
    int cId = selectedCenter(qp);
    if (cId == -1) {
        NPoint p = qpointToNPoint(qp, realCenters);
        if (p.di() < 0 or p.di() > n-1 or p.dj() < 0 or p.dj() > n-1)
            return;
        addCenter(p);
    } else {
        removeCenter(cId);
    }
}

void PlaneDisplay::moveCenter(int cId, NPoint p) {
    if (!realCenters && contains(centers, p)) return;
    if (p.di() < 0 or p.di() > n-1 or p.dj() < 0 or p.dj() > n-1) return;
    centers[cId] = p;
    numCentroidMoves = 0;
    printScene();
}

bool PlaneDisplay::saveImage(const QString &fileName, const char *fileFormat) const {
    QImage visibleImage = image;
    return visibleImage.save(fileName, fileFormat);
}

void PlaneDisplay::setShowCentroids(bool show) {
     if (shouldPrintCentroids == show) return;
     shouldPrintCentroids = show;
     refreshScene();
}

void PlaneDisplay::setCentroidWeight(Num weight) {
    if (centroidWeight == weight) return;
    centroidWeight = weight;
    numCentroidMoves = 0;
    refreshScene();
}

void PlaneDisplay::setShowStatistics(bool show) {
    if (shouldPrintStatistics == show) return;
    shouldPrintStatistics = show;
    refreshScene();
}

void PlaneDisplay::setShowIdealPerimeter(bool show) {
    if (shouldPrintIdealPerimeter == show) return;
    shouldPrintIdealPerimeter = show;
    refreshScene();
}

void PlaneDisplay::printScene() {
    constrIter = -1;
    updateRegions();
    refreshScene();
}

void PlaneDisplay::printConstrScene() {
    image.fill(qRgb(255, 255, 240)); //white bg
    printRegions(constrPlane);
    if (n <= 50) printLatticePoints();
    printCenters();
    update();
}

void PlaneDisplay::refreshScene() {
    image.fill(qRgb(255, 255, 240));
    printRegions(plane);
    printBoundaries();
    if (n <= 50) printLatticePoints();
    if (shouldPrintIdealPerimeter) printIdealPerimeters();
    printCenters();
    if (shouldPrintCentroids) printCentroids();
    if (shouldPrintStatistics) printStatistics();
    update();
}

QPoint PlaneDisplay::npoint2QPoint(NPoint p) const {
    int x = round((p.i.asDouble()/n)*image.size().width());
    int y = round((p.j.asDouble()/n)*image.size().width());
    x += cellPixSize()/2;
    y += cellPixSize()/2;
    return QPoint(x,y);
}

QPointF PlaneDisplay::npoint2QPointf(NPoint p) const {
    double x = (p.i.asDouble()/n)*image.size().width();
    double y = (p.j.asDouble()/n)*image.size().width();
    x += dcellPixSize()/2;
    y += dcellPixSize()/2;
    return QPointF(x,y);
}

NPoint PlaneDisplay::qpointToNPoint(QPoint qp, bool realPoint) const {
    double dx = ((double)(qp.x()-cellPixSize()/2))/image.size().width();
    double dy = ((double)(qp.y()-cellPixSize()/2))/image.size().width();
    dx *= n;
    dy *= n;
    if (realPoint) return NPoint(dx, dy);
    else return NPoint(round(dx), round(dy));
}

int PlaneDisplay::cellPixSize() const {
    return image.size().width()/n;
}

double PlaneDisplay::dcellPixSize() const {
    return ((double) image.size().width())/n;
}

QColor PlaneDisplay::centerColor(int centerId) const {
    return regionColors[centerId%regionColors.size()];
}

int PlaneDisplay::numLines(const string& s) {
    int numlines = 0;
    for (char c : s) {
        if (c == '\n') numlines++;
    }
    return numlines;
}

string PlaneDisplay::longestLine(const string& s) {
    int width = 0, maxwidth = 0, linestartpos = 0, longestlinestartpos = 0, index = 0;
    for (char c : s) {
        if (c == '\n') {
            if (width > maxwidth) {
                maxwidth = width;
                longestlinestartpos = linestartpos;
            }
            width = 0;
            linestartpos = index+1;
        } else width++;
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
    ss <<"Num ("<<(realCenters?"real":"integer")<<") centers: "<<centers.size()<<endl;
    Metric metric = matcher.getMetric();
    double d1 = avgDistPointCenter(plane, centers, metric);
    ss <<"Iteration: "<<numCentroidMoves<<endl;
    ss <<"Point-center dist: "<<d1<<endl;
    double d2 = avgDistCenterCentroid(plane, centers, metric, centroidWeight, realCenters);
    ss <<"Center-centroid dist: "<<d2<<endl;
    ss <<"Centroid weight (p): "<<centroidWeight.asStr()<<endl;

    string s = ss.str();
    printBoxedText(s);
}

void PlaneDisplay::printLatticePoints() {
    QPainter painter(&image);
    painter.setPen(POINT_COLOR);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QPoint qp = npoint2QPoint(NPoint(i,j));
            painter.drawEllipse(qp, POINT_RAD, POINT_RAD);
        }
    }
}

void PlaneDisplay::printCenters() {
    QPainter painter(&image);
    int k = numCenters();
    for (int i = 0; i < k; i++) {
        QPoint qp = npoint2QPoint(centers[i]);
        painter.setPen(Qt::black);
        painter.setBrush(centerColor(i));
        painter.drawEllipse(qp, CENTER_RAD, CENTER_RAD);
    }
}

void PlaneDisplay::printCentroids() {
    QPainter painter(&image);
    int k = numCenters();
    vector<NPoint> ctrdois = weightedCentroids(plane, centers, matcher.getMetric(), centroidWeight, realCenters);
    for (int i = 0; i < k; i++) {
        QPoint qp = npoint2QPoint(ctrdois[i]);
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
    int k = numCenters();
    vector<int> quotas = centerQuotas(n, k);
    for (int cId = 0; cId < k; cId++) {
        NPoint c = centers[cId];
        QPoint qc = npoint2QPoint(c);
        double area = quotas[cId];
        Metric metric = matcher.getMetric();
        if (metric.val == 2) {
            double rad = areaToRad(area);
            int qRad = round(((double)rad/n)*image.size().width());
            painter.setPen(QPen(POINT_COLOR, 2));
            painter.drawEllipse(qc, qRad-2, qRad-2);
            painter.drawEllipse(qc, qRad+2, qRad+2);
            painter.setPen(QPen(centerColor(cId), 2));
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
            painter.setPen(QPen(centerColor(cId), 2));
            pol[0].rx()-=2; pol[1].ry()-=2; pol[2].rx()+=2; pol[3].ry()+=2;
            painter.drawConvexPolygon(pol);
        } else if (metric.isInf()) {
            double side = sqrt(area);
            int qSide = round(((double)side/n)*image.size().width());
            int x = qc.x()-qSide/2, y = qc.y()-qSide/2;
            painter.setPen(QPen(POINT_COLOR, 2));
            painter.drawRect(x-2, y-2, qSide+4, qSide+4);
            painter.drawRect(x+2, y+2, qSide-4, qSide-4);
            painter.setPen(QPen(centerColor(cId), 2));
            painter.drawRect(x, y, qSide, qSide);
        } else {
            //not available
        }
    }
}

void PlaneDisplay::printRegions(const vector<vector<int>>& currPlane)
{
    QPainter painter(&image);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            QPoint qp = npoint2QPoint(NPoint(i,j));
            int cId = currPlane[i][j];
            if (cId == -1) {
                painter.setPen(EMPTY_COLOR);
                painter.setBrush(EMPTY_COLOR);
            } else {
                painter.setPen(centerColor(cId));
                painter.setBrush(centerColor(cId));
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
    double r = dcellPixSize();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n-1; j++) {
            QPointF qp = npoint2QPointf(NPoint(i, j));
            if (plane[i][j] != plane[i][j+1]) {
                QPointF left(qp.x()-r/2, qp.y()+r/2);
                QPointF right(qp.x()+r/2, qp.y()+r/2);
                painter.drawLine(left, right);
            }
        }
    }
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n; j++) {
            QPointF qp = npoint2QPointf(NPoint(i, j));
            if (plane[i][j] != plane[i+1][j]) {
                QPointF top(qp.x()+r/2, qp.y()-r/2);
                QPointF bottom(qp.x()+r/2, qp.y()+r/2);
                painter.drawLine(top, bottom);
            }
        }
    }
}

void PlaneDisplay::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        selectedCId = selectedCenter(event->pos());
        lastPointPointedByMouse = event->pos();
        mouseMoving = true;
        mouseMoved = false;
    }
}

void PlaneDisplay::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && mouseMoving) {
        if (selectedCId != -1) {
            moveCenter(selectedCId, qpointToNPoint(event->pos(), realCenters));
            mouseMoved = true;
        }
    }
}

void PlaneDisplay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && mouseMoving) {
        if (not mouseMoved) {
            toggleCenter(event->pos());
        }
        selectedCId = -1;
        mouseMoving = false;
        mouseMoved = false;
    }
}

void PlaneDisplay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void PlaneDisplay::resizeEvent(QResizeEvent *event) {
    int sideLength = qMin(width(), height());
    QSize newSize(sideLength, sideLength);
    QImage newImage(newSize, QImage::Format_RGB32);
    image = newImage;
    QWidget::resizeEvent(event);
    refreshScene();
}
