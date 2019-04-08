#-------------------------------------------------
#
# Project created by QtCreator 2019-02-26T09:33:39
#
#-------------------------------------------------

QT       -= gui

TARGET = Algorithm
TEMPLATE = lib

include(../../gobs.pri)

LIBS += -L$$GOBS_THIRDPARTY_LIB -llibblas -lliblapack -lliblapacke

DEFINES += ALGORITHM_LIBRARY ADD_   HAVE_LAPACK_CONFIG_H  LAPACK_COMPLEX_STRUCTURE

INCLUDEPATH += $$GOBS_THIRDPARTY_LIB/include


SOURCES += algorithm.cpp \
    geo2xy_utm.cpp \
    gobs_interpolation.cpp \
    gobs_location.cpp

HEADERS += algorithm.h\
        algorithm_global.h \
        geo2xy_utm.h \
        ToolsAlgo.h \
    gobs_location.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
