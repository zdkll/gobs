#-------------------------------------------------
#
# Project created by QtCreator 2017-11-22T16:32:00
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = AreaNodeManager
TEMPLATE = lib

include (../../../gobs.pri)

#debug
#CONFIG += debug

DESTDIR = $$GOBS_DIR/lib

INCLUDEPATH  += $$GOBS_DIR/include

LIBS += -L$$GOBS_LIB -lCore -lGPublicSo -lGBSqlFunctions

SOURCES += main.cpp\
        assignnodedlg.cpp \
    deploysystem.cpp \
    rndeployments.cpp \
    lmgrpublic.cpp

HEADERS  += assignnodedlg.h \
    deploysystem.h \
    rndeployments.h \
    lmgrpublic.h

FORMS    += assignnodedlg.ui \
         deploysystem.ui \
         rndeployments.ui
