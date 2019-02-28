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


INCLUDEPATH += $$GOBS_DIR/include

INCLUDEPATH += $$GOBS_SRC/GPositionSystem

LIBS += -L$$GOBS_DIR/lib -lCore -lSqlFunctions -lGPublicSo -lGPositionSystem


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


win32{
target.files  = $$GOBS_THIRDPARTY/lib/*.dll
target.path = $$GOBS_BIN
INSTALLS += target
}

