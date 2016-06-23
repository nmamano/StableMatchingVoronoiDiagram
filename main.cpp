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
 * comment code
 * document
 */
