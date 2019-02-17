#-------------------------------------------------
#
# Project created by QtCreator 2017-11-06T09:45:39
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = gobs
TEMPLATE = app

include(../../gobs.pri)

DESTDIR = $$GOBS_BIN

INCLUDEPATH += $$GOBS_DIR/include

LIBS += -L$$GOBS_DIR/lib -lCore -lGBSqlFunctions -lGPublicSo


SOURCES += main.cpp\
        mainwindow.cpp \
    mainwinpublic.cpp \
    mainwinunit.cpp \
    exportspsfilesdlg.cpp \
    logindlg.cpp

HEADERS  += mainwindow.h \
    mainwinpublic.h \
    mainwinunit.h \
    exportspsfilesdlg.h \
    logindlg.h

FORMS    += mainwindow.ui \
    exportspsfilesdlg.ui \
    logindlg.ui




