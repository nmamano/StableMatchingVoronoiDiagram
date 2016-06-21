#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "benchmarker.h"

int main(int argc, char *argv[])
{
//    Benchmarker benchmarker;
//    benchmarker.run();
//    return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}


/* To do:
 * Bicolor annuli region drawing
 * Center dragging
 * Add k-means iteration buttons
 * Display grid size, number of centers, and statistics
 * optimize printscene
 * optimize resize
 * highlight centroids
 * comment code
 */
