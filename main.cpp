#include <QApplication>
#include <iostream>
#include "mainwindow.h"
#include "benchmarker.h"

int main(int argc, char *argv[])
{
//    Benchmarker::runExperiments();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}


/* To do:
 * Bicolor annuli region drawing
 * comment code
 * document
 * disable specific colors for colorblind people ?
 *
 *
 * use unsigned short for grid indices and center indices
 *
 */
