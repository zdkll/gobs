#ifndef GPSPUBLIC_H
#define GPSPUBLIC_H

#include <QtGlobal>

//GPS 坐标
struct   GpsCoord
{
    GpsCoord();
    GpsCoord toXy();

    quint32  utc_sec = 0; //utc毫秒
    double x;
    double y;
};

#endif // GPSPUBLIC_H
