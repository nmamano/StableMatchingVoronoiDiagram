#-------------------------------------------------
#
# Project created by QtCreator 2016-06-18T14:08:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app
TEMPLATE = app
QMAKE_CXXFLAGS += -O3

SOURCES += matchingutils.cpp \
    main.cpp\
    mainwindow.cpp \
    planedisplay.cpp \
    benchmarker.cpp \
    metric.cpp \
    point.cpp \
    num.cpp \
    radixsort.cpp \
    bigreedymatcher.cpp \
    dpoint.cpp \
    npoint.cpp \
    pairheap.cpp \
    circlegrower.cpp \
    pairsort.cpp \
    matching.cpp \
    voronoidiagram.cpp \
    triangle.cpp \
    circle.cpp \
    polygon.cpp \
    edge.cpp \
    voronoitree.cpp \
    galeshapley.cpp

HEADERS  += mainwindow.h \
    planedisplay.h \
    matchingutils.h \
    benchmarker.h \
    metric.h \
    point.h \
    num.h \
    radixsort.h \
    bigreedymatcher.h \
    dpoint.h \
    npoint.h \
    pairheap.h \
    circlegrower.h \
    pairsort.h \
    matching.h \
    voronoidiagram.h \
    triangle.h \
    circle.h \
    polygon.h \
    edge.h \
    voronoitree.h \
    galeshapley.h
