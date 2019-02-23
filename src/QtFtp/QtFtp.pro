#-------------------------------------------------
#
# Project created by QtCreator 2017-12-04T13:51:47
#
#-------------------------------------------------

QT       += core network

QT       -= gui
TARGET = QtFtp
TEMPLATE = lib

include(../../gobs.pri)


DEFINES += QTFTP_LIBRARY


SOURCES += qftp.cpp \
    qurlinfo.cpp

HEADERS += qftp.h\
    qurlinfo.h \
    qtftp_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
