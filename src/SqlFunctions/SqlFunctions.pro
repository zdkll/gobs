#-------------------------------------------------
#
# Project created by QtCreator 2017-12-20T17:44:35
#
#-------------------------------------------------
#数据库操作接口库

QT       += core sql


TARGET = SqlFunctions

TEMPLATE = lib

include (../../gobs.pri)

DEFINES += SQLFUNCTIONS_LIBRARY


SOURCES += main.cpp \
    sqlfunctions.cpp

HEADERS += \
    sqlfunctions.h \
    sqlmacro.h \
    sqlfunctions_global.h
