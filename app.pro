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
    matcher.cpp \
    fastmatcher.cpp \
    benchmarker.cpp

HEADERS  += mainwindow.h \
    planedisplay.h \
    matcher.h \
    fastmatcher.h \
    matchingutils.h \
    benchmarker.h
