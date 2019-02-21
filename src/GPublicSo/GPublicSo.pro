#-------------------------------------------------
#
# Project created by QtCreator 2017-11-21T16:10:12
#
#-------------------------------------------------

QT       +=  widgets

TARGET = GPublicSo
TEMPLATE = lib

include(../../gobs.pri)

DESTDIR = $$GOBS_LIB
DEFINES += GPUBLICSO_LIBRARY


SOURCES += publicwidgets.cpp \
    publicfunctions.cpp \
    gobs_interpolation.cpp

HEADERS += publicwidgets.h \
    publicfunctions.h \
    ToolsAlgo.h \
    gpublicso_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
