#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T16:51:45
#
#-------------------------------------------------

QT       += core  sql

QT       -= gui


TARGET = ExportSegyProgram
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../../gobs.pri)

LIBS  += -L$$GOBS_DIR/lib -lCore -lSqlFunctions -lGPublicSo  -lAlgorithm
LIBS  +=  -L$$GOBS_THIRDPARTY_LIB -llibfftw3f-3


INCLUDEPATH += $$GOBS_SRC/Algorithm \
       $$GOBS_THIRDPARTY/include


SOURCES += main.cpp \
    abstractsegyproducer.cpp \
    recvorientedsegy.cpp \
    shotorientedsegy.cpp \
    responseprocessorf.cpp

HEADERS += \
    abstractsegyproducer.h \
    recvorientedsegy.h \
    shotorientedsegy.h \
    responseprocessorf.h
