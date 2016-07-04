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
    void moveCentersToCentroids();
    void showCentroids(bool show);
    void showStatistics(bool show);
    void showIdealPerimeter(bool show);

    void setLInftyMetric();
    void setL2Metric();
    void setL1Metric();

    void showConstrStep();
private:
    void createActions();
    void createMenus();
    bool saveFile(const QByteArray &fileFormat);

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
    QAction *gridSizeAct;
    QAction *centersAct;
    QActionGroup *metricAct;
    QAction *L1MetricAct;
    QAction *L2MetricAct;
    QAction *LInftyMetricAct;

    QAction *showConstrStepAct;

    QAction *showCentroidsAct;
    QAction *showStatisticsAct;
    QAction *showIdealPerimeterAct;

};

#endif // MAINWINDOW_H
