 #include "gpspublic.h"
#include "geo2xy_utm.h"

GpsCoord::GpsCoord()
{

}
GpsCoord GpsCoord::toXy()
{
    GpsCoord xyCord = *this;
    geo2xy_utm(this->x,this->y,&xyCord.x,&xyCord.y);
    return xyCord;
}
