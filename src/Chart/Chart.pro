#-------------------------------------------------
#
# Project created by QtCreator 2019-03-13T11:18:22
#
#-------------------------------------------------

QT       += widgets

TARGET = Chart
TEMPLATE = lib

DEFINES += CHART_LIBRARY

SOURCES += chart.cpp

HEADERS += chart.h\
        chart_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
