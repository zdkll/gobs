#-------------------------------------------------
#
# Project created by QtCreator 2017-11-09T09:45:42
#
#-------------------------------------------------

QT       += gui core widgets

TARGET = CommModule

TEMPLATE = lib

include(../../gobs.pri)


LIBS += -L$$GOBS_DIR/lib -lCore

DESTDIR = $$GOBS_DIR/lib

DEFINES += COMMMODULE_LIBRARY

SOURCES += commmodule.cpp

HEADERS += commmodule.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
