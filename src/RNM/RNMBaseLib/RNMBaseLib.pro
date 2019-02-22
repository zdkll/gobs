#-------------------------------------------------
#
# Project created by QtCreator 2017-11-23T14:18:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include (../../../gobs.pri)

TARGET = RNMBaseLib
TEMPLATE = lib

include (../../../gobs.pri)

DEFINES += RNMBASELIB_LIBRARY

DESTDIR = $$GOBS_DIR/lib

LIBS += -L$$GOBS_DIR/lib -lCore -lGPublicSo -lQtFtp


INCLUDEPATH += $$GOBS_DIR/include \
   $$GOBS_DIR/src/QtFtp

SOURCES += searchthread.cpp \
    rnitems.cpp \
    nodequery.cpp \
    rnmpublic.cpp \
    nodeftp.cpp \
    rnftpmanager.cpp \
    statusreport.cpp

HEADERS += searchthread.h \
    rnitems.h \
    nodequery.h \
    rnmpublic.h \
    nodeftp.h \
    rnftpmanager.h \
    statusreport.h \
    rnmbaselib_global.h

FORMS += \
    rnftpmanager.ui \
    statusreport.ui
