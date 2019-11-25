#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QMainWindow>

class PlaneDisplay;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

protected:

private slots:
    void save();
    void about();
    void changeSize();
    void changeCenters();
    void changeCentroidWeight();
    void changeMetric();
    void changeAppetite();

    void moveCentersToCentroids();
    void undoMoves();

    void showCentroids(bool show);
    void showStatistics(bool show);
    void showIdealPerimeter(bool show);
    void showVoronoi(bool show);
    void showDelaunay(bool show);
    void enableGraphColoring(bool show);
    void enableVoronoiOnly(bool show);
    void enableVoronoiTree(bool show);

    void setLInftyMetric();
    void setL2Metric();
    void setL1Metric();
    void setRealCenters();
    void setDiscreteCenters();

    void showConstrStep();
    void showNextRegion();
    void showNextCircle();
    void showPrevCircle();

private:
    void createActions();
    void createMenus();
    bool saveFile(const QByteArray &fileFormat);
    void setMetricsUnchecked();

    PlaneDisplay *planeDisplay;
    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QList<QAction*> saveAsActs;
    QAction *exitAct;
    QAction *aboutAct;

    QAction *moveCentersToCentroidsAct;
    QAction *undoMovesAct;
    QAction *gridSizeAct;
    QAction *centersAct;
    QAction *appetiteAct;
    QAction *centroidWeightAct;

    QAction *L1MetricAct;
    QAction *L2MetricAct;
    QAction *LInftyMetricAct;
    QAction *otherMetricAct;

    QAction *showConstrStepAct;
    QAction *showNextRegionAct;
    QAction *showNextCircleAct;
    QAction *showPrevCircleAct;

    QAction *showCentroidsAct;
    QAction *showStatisticsAct;
    QAction *showIdealPerimeterAct;
    QAction *graphColoringAct;
    QAction *voronoiOnlyAct;
    QAction *voronoiTreeAct;

    QAction *showVoronoiAct;
    QAction *showDelaunayAct;

    QActionGroup *centerType;
    QAction *discreteCentersAct;
    QAction *realCentersAct;

};

#endif // MAINWINDOW_H
