#-------------------------------------------------
#
# Project created by QtCreator 2017-11-21T16:10:12
#
#-------------------------------------------------

QT       +=  widgets

TARGET = GPublicSo
TEMPLATE = lib

include(../../gobs.pri)

DEFINES += GPUBLICSO_LIBRARY


SOURCES += publicwidgets.cpp \
    publicfunctions.cpp \
    graphfunctions.cpp

HEADERS += publicwidgets.h \
    publicfunctions.h \
    gpublicso_global.h \
    graphfunctions.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
