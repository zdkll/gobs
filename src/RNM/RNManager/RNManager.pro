#-------------------------------------------------
#
# Project created by QtCreator 2017-11-07T15:10:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = RNManager
TEMPLATE = app

include (../../../gobs.pri)


LIBS += -L$$GOBS_LIB  -lCommModule -lCore -lGPublicSo -lRNMBaseLib -lQtFtp


DESTDIR = $$GOBS_DIR/bin

INCLUDEPATH += $$GOBS_DIR/include  \
      $$GOBS_DIR/src/QtFtp


INCLUDEPATH +=../RNMBaseLib \

SOURCES += main.cpp\
           rnmanager.cpp \
           rnmgrunit.cpp\
           rnmsidebar.cpp \
         rnmgraphview.cpp


HEADERS  += rnmanager.h \
            rnmgrunit.h \
            rnmsidebar.h \
          rnmgraphview.h

FORMS    += rnmanager.ui \
            rnmsidebar.ui


