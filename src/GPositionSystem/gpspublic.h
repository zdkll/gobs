#ifndef GPSPUBLIC_H
#define GPSPUBLIC_H

#include <QtGlobal>
#include <functional>

//GPS 坐标
struct   GpsCoord
{
    GpsCoord();
    GpsCoord toXy();

    quint32  utc_sec = 0; //utc毫秒
    double x;
    double y;
};


typedef std::function<void (const QString &)> ShowTextFunc;


#endif // GPSPUBLIC_H
