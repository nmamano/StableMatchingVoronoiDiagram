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
    allpairsorting.cpp \
    radixsort.cpp \
    neighborheuristic.cpp \
    siteheap.cpp \
    bigreedymatcher.cpp \
    diskgrower.cpp \
    dpoint.cpp \
    npoint.cpp

HEADERS  += mainwindow.h \
    planedisplay.h \
    matchingutils.h \
    benchmarker.h \
    metric.h \
    point.h \
    num.h \
    allpairsorting.h \
    radixsort.h \
    neighborheuristic.h \
    siteheap.h \
    bigreedymatcher.h \
    diskgrower.h \
    dpoint.h \
    npoint.h
