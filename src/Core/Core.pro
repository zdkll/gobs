#-------------------------------------------------
#
# Project created by QtCreator 2017-11-06T15:16:34
#
#-------------------------------------------------

QT      += core

TARGET = Core
TEMPLATE = lib
CONFIG += plugin

include (../../gobs.pri)

DESTDIR = $$GOBS_LIB

#CONFIG +=debug

SOURCES += \
    core.cpp \
    public.cpp \
    spsfiles.cpp \
    segy.cpp \
    assistant.cpp

HEADERS += \
    core.h \
    public.h \
    macro.h \
    spsfiles.h \
    segy.h \
    assistant.h

win32{
LIBS += -lwsock32
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
