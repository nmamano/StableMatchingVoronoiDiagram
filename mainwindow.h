#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QAction *showCentroidsAct;
    QAction *showStatisticsAct;
};

#endif // MAINWINDOW_H
