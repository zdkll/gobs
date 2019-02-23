#include "geo2xy_utm.h"
#include <math.h>

#ifndef PI
#define PI 3.1415923
#endif

short getUTMZone(double lat, double lon);
void convLLtoTM(double a, double f, double lat, double lon, \
    double lon0, double *x, double *y);

/*--------------------------------------------------------------------------------------
    Transfer given Geographic coordinate to Cartesian XY coordinate
----------------------------------------------------------------------------------------
INPUT:
    lon: longitude degree
    lat: latitude degree
OUTPUT:
    x: x-coordinate
    y: y-coordinate
----------------------------------------------------------------------------------------
Credits: Yuan WANG; IGGCAS; Oct 2017
----------------------------------------------------------------------------------------*/
void geo2xy_utm( double lon, double lat, double *x, double *y)
{
    short zone;
    double a, f, lon0, xx, yy;
    double xoff, ysoff, ynoff;

    /* UTM zone number and letter designator */
    zone = getUTMZone(lat, lon);

    /* Central meridian of the zone in degrees */
    lon0 = (double) ((zone-1)*6 - 180 + 3);

    a = 6378137.;
    f = 1./298.257223563;

    /* Transverse Mercator projection */
    convLLtoTM(a, f, lat, lon, lon0, &xx, &yy);

    /* Remove false Easting and Northing */
    xoff = 500000.0;
    ynoff = 0.;
    ysoff = 10000000.0;
    xx += xoff;
    yy += (zone<0) ? ysoff : ynoff;


    /* get x & y */
    *x = xx;
    *y = yy;

    return;
}

short getUTMZone(double lat, double lon)
/************************************************************************
getUTMZone - get the UTM zone number

*************************************************************************
Input:
lat     geographical latitude in degrees
lon     geographical longitude in degrees

Output:
        returns the UTM zone number

*************************************************************************
Notes:
Does computations as doubles. The latitude is positive on the northern
hemisphere and negative on the southern hemisphere. UTM coordinates
are defined between 80S and 84N.
The longitude is negative west of the zero-meridian (Greenwich), i.e.
its range of values is -180.0 ... 179.99999.
*************************************************************************
Author: Nils Maercklin, 30 March 2007
*************************************************************************/
{
    short zone; /* UTM zone number */

    /* Make sure the longitude is between -180 and 179.999 deg */
    lon = (lon+180.)-floor((lon+180.)/360.)*360.-180.;

    /* Zone number */
    zone = (short)((lon + 180.)/6.) + 1;
    if (lat >= 56.0 && lat < 64.0 && lon >= 3.0 && lon < 12.0) zone = 32;

    /* Svalbard zones */
    if (lat >= 72.0 && lat < 84.0) {
        if      (lon >= 0.0  && lon <  9.0) zone = 31;
        else if (lon >= 9.0  && lon < 21.0) zone = 33;
        else if (lon >= 21.0 && lon < 33.0) zone = 35;
        else if (lon >= 33.0 && lon < 42.0) zone = 37;
    }

    /* Return zone number */
    return zone;
}

void convLLtoTM(double a, double f, double lat, double lon, \
    double lon0, double *x, double *y)
/************************************************************************
convLLtoTM - convert latitude and longitude to Transverse Mercator
             Easting and Northing (e.g. UTM)
*************************************************************************
Input:
a       ellipsoid simimajor axis (in m)
f       ellipsoid flattening
lat     geographical latitude in degrees
lon     geographical longitude in degrees
lon0    central meridian (longitude) in degrees

Output:
x       Easting (Transverse Mercator grid)
y       Northing (Transverse Mercator grid)

*************************************************************************
Notes:
Does computations as doubles. The longitude is negative west of the
zero-meridian (Greenwich), i.e. its range of values is -180.0 ... 179.99999.
The latitude is positive on the northern hemisphere and negative on the
southern hemisphere.
For standard UTM coordinates add always a false Easting of 500 km and a
false Northing of 10000 km on the southern hemisphere. UTM coordinates
are only defined between 80S and 84N.

Reference:
J. P. Snyder (1987). Map Projections - A Working Manual.
    U.S. Geological Survey Professional Paper 1395, 383 pages.
    U.S. Government Printing Office.

This function is adopted from a Perl routine in Geo-Coordinates-UTM-0.06
by G. Crookham (CPAN, March 2007).
*************************************************************************
Author: Nils Maercklin, 30 March 2007
*************************************************************************/
{
    double e2;   /* eccentricity squared, e2 = f(2-f) */
    double ep2;  /* eccentricity prime squared */
    double k0, cn, ct, cc, ca, cm;

    /* Convert latitude and longitudes to radians */
    lat  *= PI / 180.0;
    lon  *= PI / 180.0;
    lon0 *= PI / 180.0;

    /* Ellipsoid parameters */
    e2  = f * (2.0 - f);
    ep2 = e2 / (1.0 - e2);

    /* Some constants */
    k0 = 0.9996;
    cn = a / sqrt(1.0-e2*sin(lat)*sin(lat));
    ct = tan(lat)*tan(lat);
    cc = ep2*cos(lat)*cos(lat);
    ca = cos(lat)*(lon-lon0);

    cm = a * ((1.0 - e2/4. - 3.*e2*e2/64. - 5.*e2*e2*e2/256.)*lat \
         - (3.*e2/8. + 3.*e2*e2/32.+  45.*e2*e2*e2/1024.)*sin(2.*lat) \
         + (15.*e2*e2/256. + 45*e2*e2*e2/1024.)*sin(4.*lat) \
         - (35.*e2*e2*e2/3072.)*sin(6.*lat));

    /* Transverse Mercator Easting */
    (*x) = k0 * cn * (ca + (1.-ct+cc) * ca*ca*ca/6. \
           + (5. - 18.*ct + ct*ct + 72.*cc - 58.*ep2) * ca*ca*ca*ca*ca/120.);

    /* Transverse Mercator Northing */
    (*y) = k0 * (cm + cn*tan(lat) * (ca*ca/2. + \
           (5. - ct + 9.*cc + 4.*cc*cc) * ca*ca*ca*ca/24. \
           + (61. - 58.*ct + ct*ct + 600.*cc - 330.*ep2) * \
             ca*ca*ca*ca*ca*ca/720.));
}

