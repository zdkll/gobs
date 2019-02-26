#ifndef GPSPUBLICDEF_H
#define GPSPUBLICDEF_H

#include "geo2xy_utm.h"

//GPS 坐标
struct GpsCoord
{
    GpsCoord(){}
    GpsCoord toXy(){
        GpsCoord xyCord = *this;
        geo2xy_utm(this->x,this->y,&xyCord.x,&xyCord.y);
        return xyCord;
    }
    quint32  utc_sec = 0; //utc毫秒
    double x;
    double y;
};

#endif // GPSPUBLICDEF_H
