#-------------------------------------------------
#
# Project created by QtCreator 2017-11-07T15:11:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = DataMgrControl
TEMPLATE = app

include (../../../gobs.pri)


LIBS += -L$$GOBS_LIB  -lCore -lCommModule -lGPublicSo -lQCPlotter


INCLUDEPATH += ../QCPlotter

#debug
#CONFIG += debug

SOURCES += main.cpp\
        datamgrcontrol.cpp \
        dmgrunit.cpp

HEADERS  += datamgrcontrol.h \
    dmgrunit.h

FORMS    += datamgrcontrol.ui \
    exportsegydlg.ui \
    opensegydlg.ui   \
    plotparamdlg.ui
