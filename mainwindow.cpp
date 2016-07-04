#include "mainwindow.h"
#include "planedisplay.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
    QTime now = QTime::currentTime();
    qsrand(now.msec());
    planeDisplay = new PlaneDisplay;
    setCentralWidget(planeDisplay);
    createActions();
    createMenus();
    setWindowTitle(tr("Stable Grid Matching"));
    resize(940, 960);
}


MainWindow::~MainWindow()
{
}

void MainWindow::save()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Stable Grid Matching"),
        tr("Each point is assigned to a center in a "
           "way that every center has the same number of points "
           "assigned and the matching is stable (in the Stable "
           "Marriage Problem sense).\n"
           "\n"
           "Features:\n"
           "- Add, remove, or move centers with mouse.\n"
           "- Move centers to centroids.\n"
           "- Show centroids, statistics, and ideal perimeters.\n"
           "- Support for Euclidean, Manhattan, and Chebyshev distances.\n"
           "- Simulate matching algorithm step by step.\n"));
}

void MainWindow::changeSize()
{
    bool ok;
    int newSize = QInputDialog::getInt(this, tr("Grid"),
        tr("Select grid size:"),
        planeDisplay->getGridSize(),
        2, 1000, 1, &ok);
    if (ok) planeDisplay->setGridSize(newSize);
}

void MainWindow::changeCenters()
{
    bool ok;
    int newNum = QInputDialog::getInt(this, tr("Centers"),
        tr("Select number of centers:"),
        planeDisplay->getNumCenters(),
        1, planeDisplay->numPoints(), 1, &ok);

    if (ok) planeDisplay->setRandomCenters(newNum);
}

void MainWindow::moveCentersToCentroids()
{
    planeDisplay->moveCentersToCentroids();
}

void MainWindow::showCentroids(bool show)
{
    planeDisplay->setShowCentroids(show);
}

void MainWindow::showStatistics(bool show)
{
    planeDisplay->setShowStatistics(show);
}

void MainWindow::showIdealPerimeter(bool show)
{
    planeDisplay->setShowIdealPerimeter(show);
}

void MainWindow::setL1Metric()
{
    planeDisplay->setDistMetric(1);
}

void MainWindow::showConstrStep()
{
    planeDisplay->showConstrStep();
}

void MainWindow::setL2Metric()
{
    planeDisplay->setDistMetric(2);
}

void MainWindow::setLInftyMetric()
{
    planeDisplay->setDistMetric(-1);
}

void MainWindow::createActions()
{
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }

    aboutAct = new QAction(tr("About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(Qt::Key_Escape);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    gridSizeAct = new QAction(tr("Grid size..."), this);
    connect(gridSizeAct, SIGNAL(triggered()), this, SLOT(changeSize()));
    gridSizeAct->setShortcut(Qt::Key_G);

    centersAct = new QAction(tr("Num centers..."), this);
    connect(centersAct, SIGNAL(triggered()), this, SLOT(changeCenters()));
    centersAct->setShortcut(Qt::Key_C);

    moveCentersToCentroidsAct = new QAction(tr("Move centers to centroids"), this);
    connect(moveCentersToCentroidsAct, SIGNAL(triggered()), this, SLOT(moveCentersToCentroids()));
    moveCentersToCentroidsAct->setShortcut(Qt::Key_Right);

    showCentroidsAct = new QAction(tr("Show centroids"), this);
    showCentroidsAct->setCheckable(true);
    showCentroidsAct->setChecked(true);
    connect(showCentroidsAct, SIGNAL(triggered(bool)), this, SLOT(showCentroids(bool)));

    showStatisticsAct = new QAction(tr("Show statistics"), this);
    showStatisticsAct->setCheckable(true);
    showStatisticsAct->setChecked(true);
    connect(showStatisticsAct, SIGNAL(triggered(bool)), this, SLOT(showStatistics(bool)));

    showIdealPerimeterAct = new QAction(tr("Show ideal perimeters"), this);
    showIdealPerimeterAct->setCheckable(true);
    showIdealPerimeterAct->setChecked(true);
    connect(showIdealPerimeterAct, SIGNAL(triggered(bool)), this, SLOT(showIdealPerimeter(bool)));

    L1MetricAct = new QAction(tr("Manhattan distance"), this);
    L1MetricAct->setCheckable(true);
    connect(L1MetricAct, SIGNAL(triggered()), this, SLOT(setL1Metric()));

    L2MetricAct = new QAction(tr("Euclidean distance"), this);
    L2MetricAct->setCheckable(true);
    connect(L2MetricAct, SIGNAL(triggered()), this, SLOT(setL2Metric()));

    LInftyMetricAct = new QAction(tr("Chebyshev distance"), this);
    LInftyMetricAct->setCheckable(true);
    connect(LInftyMetricAct, SIGNAL(triggered()), this, SLOT(setLInftyMetric()));

    metricAct = new QActionGroup(this);
    metricAct->addAction(L1MetricAct);
    metricAct->addAction(L2MetricAct);
    metricAct->addAction(LInftyMetricAct);
    L2MetricAct->setChecked(true);

    connect(moveCentersToCentroidsAct, SIGNAL(triggered()), this, SLOT(moveCentersToCentroids()));
    moveCentersToCentroidsAct->setShortcut(Qt::Key_Right);

    showConstrStepAct = new QAction(tr("Show construction step"), this);
    connect(showConstrStepAct, SIGNAL(triggered()), this, SLOT(showConstrStep()));
    showConstrStepAct->setShortcut(Qt::Key_S);
}

void MainWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("File"), this);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("Options"), this);
    optionMenu->addAction(gridSizeAct);
    optionMenu->addAction(centersAct);
    optionMenu->addAction(moveCentersToCentroidsAct);
    optionMenu->addSeparator();
    optionMenu->addAction(L1MetricAct);
    optionMenu->addAction(L2MetricAct);
    optionMenu->addAction(LInftyMetricAct);
    optionMenu->addSeparator();
    optionMenu->addAction(showConstrStepAct);

    viewMenu = new QMenu(tr("View"), this);
    viewMenu->addAction(showCentroidsAct);
    viewMenu->addAction(showStatisticsAct);
    viewMenu->addAction(showIdealPerimeterAct);

    helpMenu = new QMenu(tr("Help"), this);
    helpMenu->addAction(aboutAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                initialPath, tr("%1 Files (*.%2);;All Files (*)").arg(
                    QString::fromLatin1(fileFormat.toUpper())).arg(
                        QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) return false;
    return planeDisplay->saveImage(fileName, fileFormat.constData());
}





