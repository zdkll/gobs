#-------------------------------------------------
#
# Project created by QtCreator 2017-11-07T15:08:28
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = LineManager
TEMPLATE = app

include (../../../gobs.pri)


LIBS += -L$$GOBS_LIB -lCommModule -lCore -lGPublicSo -lAreaNodeManager -lSqlFunctions

INCLUDEPATH += ../AreaNodeManager


SOURCES += main.cpp\
        linemanager.cpp \
    lmgrunit.cpp \
    lmgrareaview.cpp \
    stationitem.cpp

HEADERS  += linemanager.h \
    lmgrunit.h \
    lmgrareaview.h \
    stationitem.h

FORMS    += linemanager.ui
