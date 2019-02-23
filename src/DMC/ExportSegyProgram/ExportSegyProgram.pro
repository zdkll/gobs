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


#CONFIG  += debug

#INCLUDEPATH +=

LIBS    += -L$$GOBS_DIR/lib -lCore -lSqlFunctions -lGPublicSo

SOURCES += main.cpp \
    abstractsegyproducer.cpp \
    recvorientedsegy.cpp \
    shotorientedsegy.cpp

HEADERS += \
    abstractsegyproducer.h \
    recvorientedsegy.h \
    shotorientedsegy.h
