#-------------------------------------------------
#
# Project created by QtCreator 2019-02-26T09:33:39
#
#-------------------------------------------------

QT       -= gui

TARGET = Algorithm
TEMPLATE = lib

include(../../gobs.pri)

DEFINES += ALGORITHM_LIBRARY

SOURCES += algorithm.cpp \
    geo2xy_utm.cpp \
    gobs_interpolation.cpp

HEADERS += algorithm.h\
        algorithm_global.h \
        geo2xy_utm.h \
        ToolsAlgo.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
