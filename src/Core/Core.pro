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



DEFINES += CORE_LIBRARY

SOURCES += \
    core.cpp \
    spsfiles.cpp \
    segy.cpp \
    assistant.cpp \
    projectarea.cpp \
    geo2xy_utm.cpp

HEADERS += \
    core.h \
    public.h \
    macro.h \
    spsfiles.h \
    segy.h \
    assistant.h \
    core_global.h \
    projectarea.h \
    geo2xy_utm.h

win32{
LIBS += -lwsock32
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
