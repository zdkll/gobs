#-------------------------------------------------
#
# Project created by QtCreator 2019-02-26T09:33:39
#
#-------------------------------------------------

QT       -= gui

TARGET = Algorithm
TEMPLATE = lib

DEFINES += ALGORITHM_LIBRARY

SOURCES += algorithm.cpp

HEADERS += algorithm.h\
        algorithm_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
