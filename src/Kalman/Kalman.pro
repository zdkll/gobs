TARGET = Kalman

TEMPLATE = lib
CONFIG += console c++11
CONFIG -= app_bundle


include (../../gobs.pri)

INCLUDEPATH += armadillo_bits \
                               arma_pengsub

DEFINES += KALMAN_LIBRARY

LIBS += -L$$GOBS_THIRDPARTY_LIB -llibblas -lliblapack

SOURCES += main.cpp \
    kalman.cpp

HEADERS += \
    arma.h \
    complex_TJU_PENG.h \
    kalman.h \
    armadillo \
    kalman_global.h
