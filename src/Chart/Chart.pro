#-------------------------------------------------
#
# Project created by QtCreator 2019-03-13T11:18:22
#
#-------------------------------------------------

QT       += widgets

TARGET = Chart
TEMPLATE = lib

include (../../gobs.pri)

CONFIG += c++11

DEFINES += CHART_LIBRARY

SOURCES += chart.cpp \
    chartdrawer.cpp \
    abstractaxis.cpp \
    graphlayer.cpp \
    drawable.cpp

HEADERS += chart.h\
        chart_global.h \
    chartdrawer.h \
    abstractaxis.h \
    graphlayer.h \
    drawable.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
