#include "mainwindow.h"
#include "planedisplay.h"
#include "num.h"
#include <QtWidgets>

MainWindow::MainWindow()
{
    QTime now = QTime::currentTime();
    qsrand(now.msec());
//    qsrand(0);
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
           "- Integer or real valued centers.\n"
           "- Move centers to weighted centroids.\n"
           "- Show centroids, statistics, and ideal perimeters.\n"
           "- Support for any distance in the L norm family.\n"
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
        planeDisplay->numCenters(),
        1, planeDisplay->numPoints(), 1, &ok);

    if (ok) planeDisplay->setRandomCenters(newNum);
}


void MainWindow::changeCentroidWeight()
{
    bool ok;
    QString newWeight = QInputDialog::getText(this, tr("Centroid weight"),
           tr("Select centroid weight (a real number)"),
           QLineEdit::Normal,
           planeDisplay->getCentroidWeight().asQstr(),
           &ok);
    if (ok && Num::isNum(newWeight)) {
        Num weight = Num(newWeight);
        if (!weight.isInf())
            planeDisplay->setCentroidWeight(weight);
    }
}

void MainWindow::changeMetric()
{
    bool ok;
    QString newMetric = QInputDialog::getText(this, tr("Centroid weight"),
           tr("Select metric (a real number >= 1 or 'inf')"),
           QLineEdit::Normal,
           planeDisplay->getMetric().asQstr(),
           &ok);
    if (ok && Num::isNum(newMetric)) {
        Num l(newMetric);
        if (l.val >= 1)
            planeDisplay->setMetric(l);

        setMetricsUnchecked();
        if (l.val == 1) L1MetricAct->setChecked(true);
        else if (l.isInf()) LInftyMetricAct->setChecked(true);
        else if (l.val == 2) L2MetricAct->setChecked(true);
        else otherMetricAct->setChecked(true);
    }
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
    if (show && otherMetricAct->isChecked()) {
        QMessageBox::information(
            this,
            tr("Stable grid matching"),
            tr("The ideal perimeters are only visible for metrics L_2, L_1, and L_infinity."));

    }
    planeDisplay->setShowIdealPerimeter(show);
}

void MainWindow::showConstrStep()
{
    if (planeDisplay->usingRealCenters()) {
        QMessageBox::information(
            this,
            tr("Stable grid matching"),
            tr("Select integer centers to see the construction steps."));
        return;
    }
    planeDisplay->showConstrStep();
}

void MainWindow::setMetricsUnchecked() {
    L1MetricAct->setChecked(false);
    L2MetricAct->setChecked(false);
    LInftyMetricAct->setChecked(false);
    otherMetricAct->setChecked(false);
}

void MainWindow::setL1Metric()
{
    setMetricsUnchecked();
    L1MetricAct->setChecked(true);
    planeDisplay->setMetric(Num(1));
}

void MainWindow::setRealCenters()
{
    planeDisplay->setRealCenters(true);
}

void MainWindow::setDiscreteCenters()
{
    planeDisplay->setRealCenters(false);
}

void MainWindow::setL2Metric()
{
    setMetricsUnchecked();
    L2MetricAct->setChecked(true);
    planeDisplay->setMetric(Num(2));
}

void MainWindow::setLInftyMetric()
{
    setMetricsUnchecked();
    LInftyMetricAct->setChecked(true);
    planeDisplay->setMetric(Num::inf());
}

void MainWindow::createActions()
{
    for (QByteArray format : QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());
        if (text != "PNG..." && text != "JPEG...") continue;
        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
        if (text == "PNG...") {
            action->setShortcut(Qt::CTRL + Qt::Key_S);
        }
    }

    aboutAct = new QAction(tr("About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(Qt::Key_Escape);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    gridSizeAct = new QAction(tr("Grid size..."), this);
    connect(gridSizeAct, SIGNAL(triggered()), this, SLOT(changeSize()));
    gridSizeAct->setShortcut(Qt::Key_N);

    centersAct = new QAction(tr("Num centers..."), this);
    connect(centersAct, SIGNAL(triggered()), this, SLOT(changeCenters()));
    centersAct->setShortcut(Qt::Key_K);

    moveCentersToCentroidsAct = new QAction(tr("Move centers to centroids"), this);
    connect(moveCentersToCentroidsAct, SIGNAL(triggered()), this, SLOT(moveCentersToCentroids()));
    moveCentersToCentroidsAct->setShortcut(Qt::Key_Right);

    centroidWeightAct = new QAction(tr("Centroid weight..."), this);
    connect(centroidWeightAct, SIGNAL(triggered()), this, SLOT(changeCentroidWeight()));
    centroidWeightAct->setShortcut(Qt::Key_P);

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
    showIdealPerimeterAct->setChecked(false);
    connect(showIdealPerimeterAct, SIGNAL(triggered(bool)), this, SLOT(showIdealPerimeter(bool)));

    L1MetricAct = new QAction(tr("Manhattan distance"), this);
    L1MetricAct->setCheckable(true);
    connect(L1MetricAct, SIGNAL(triggered()), this, SLOT(setL1Metric()));

    L2MetricAct = new QAction(tr("Euclidean distance"), this);
    L2MetricAct->setCheckable(true);
    connect(L2MetricAct, SIGNAL(triggered()), this, SLOT(setL2Metric()));
    L2MetricAct->setChecked(true);

    LInftyMetricAct = new QAction(tr("Chebyshev distance"), this);
    LInftyMetricAct->setCheckable(true);
    connect(LInftyMetricAct, SIGNAL(triggered()), this, SLOT(setLInftyMetric()));

    otherMetricAct = new QAction(tr("Other"), this);
    otherMetricAct->setCheckable(true);
    connect(otherMetricAct, SIGNAL(triggered()), this, SLOT(changeMetric()));
    otherMetricAct->setShortcut(Qt::Key_M);

    discreteCentersAct = new QAction(tr("Discrete centers"), this);
    discreteCentersAct->setCheckable(true);
    connect(discreteCentersAct, SIGNAL(triggered()), this, SLOT(setDiscreteCenters()));
    discreteCentersAct->setShortcut(Qt::Key_D);

    realCentersAct = new QAction(tr("Real centers"), this);
    realCentersAct->setCheckable(true);
    connect(realCentersAct, SIGNAL(triggered()), this, SLOT(setRealCenters()));
    realCentersAct->setShortcut(Qt::Key_R);

    centerType = new QActionGroup(this);
    centerType->addAction(discreteCentersAct);
    centerType->addAction(realCentersAct);
    discreteCentersAct->setChecked(true);

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
    optionMenu->addAction(centroidWeightAct);
    optionMenu->addSeparator();
    optionMenu->addAction(L1MetricAct);
    optionMenu->addAction(L2MetricAct);
    optionMenu->addAction(LInftyMetricAct);
    optionMenu->addAction(otherMetricAct);
    optionMenu->addSeparator();
    optionMenu->addAction(discreteCentersAct);
    optionMenu->addAction(realCentersAct);
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





