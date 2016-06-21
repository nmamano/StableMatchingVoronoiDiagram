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

private:
    void createActions();
    void createMenus();
    bool saveFile(const QByteArray &fileFormat);

    PlaneDisplay *planeDisplay;
    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;

    QList<QAction*> saveAsActs;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QAction *gridSizeAct;
    QAction *centersAct;
};

#endif // MAINWINDOW_H
