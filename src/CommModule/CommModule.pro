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



DEFINES += COMMMODULE_LIBRARY

SOURCES += commmodule.cpp

HEADERS += commmodule.h \
    commmodule_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
