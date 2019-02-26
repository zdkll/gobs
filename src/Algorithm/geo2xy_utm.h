#ifndef GEO2XY_UTM_H
#define GEO2XY_UTM_H

#include "core_global.h"

#ifdef __cplusplus
extern "C"{
#endif

CORESHARED_EXPORT void geo2xy_utm( double lon, double lat, double *x, double *y);

#ifdef __cplusplus
}
#endif

#endif // GEO2XY_UTM_H
