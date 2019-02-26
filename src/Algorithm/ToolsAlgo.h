#ifndef TOOLSALGO_H
#define TOOLSALGO_H


//Interpolation method
#define LINEARINTER 1
#define SINCINTER   2

#ifndef NTABLE_INTERP
#define NTABLE_INTERP 121
#endif

#ifndef LTABLE
#define LTABLE 8
#endif

#ifndef PI
#define PI 3.14159265358979323746265
#endif

#include <stdio.h>
#include <cmath>

#include "algorithm_global.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
 *
 * 	一维规则数据插值
 * 	float	*data_in:	输入。已知的输入数据，大小为ni1；
 * 	int		ni1:		输入。已知数据第一维方向的个数；
 * 	float	xii:		输入。已知数据第一维方向的起始坐标；
 * 	float	dxi:		输入。已知数据第一维方向的坐标间隔；
 * 	float	*data_out:	输出。插值结果，大小为no1，要求已申请好内存；
 * 	int		no1:		输入。输出数据第一维方向的个数；
 * 	float	xoi:		输入。输出数据第一维方向的起始坐标；
 * 	float	dxo:		输入。输出数据第一维方向的坐标间隔；
 * 	float	xinl:		输入。第一维方向坐标小于已知数据最小坐标时的外延值；
 * 	float	xinr:		输入。第一维方向坐标大于已知数据最大坐标时的外延值；
 * 	int		inter_type:	输入。插值算法标记。不同的值表示不同的插值算法，相应的宏参考头文件；
 * 							目前可用的类型有：
 * 								LINEARINTER：	线性插值；
 * 								SINCINTER：		Sinc插值；
 * 								HERMITEINTER：	Hermite插值；
 * 								CSPLINEINTER:	三次样条插值。
 * 								CBSPLINEINTER：	三阶B样条插值；
 * 	int		*isok:		输出。函数结束后存储函数返回值。0表示函数正常结束，非0表示异常。
 *
 */
ALGORITHMSHARED_EXPORT void gobs_1D_interpolation_regular(float *data_in, int ni1, float xii, float dxi,
                                                          float *data_out, int no1, float xoi, float dxo,
                                                          float xinl, float xinr, int inter_type, int *isok);



/*
规则一维线性插值
evaluate y(x) via linear interpolation of y(x[0]), y(x[1]), ...

Input:
nin		length of xin and yin arrays
xin		array[nin] of monotonically increasing or decreasing x values
yin		array[nin] of input y(x) values
yinl	value used to extraplate y(x) to left of input yin values
yinr	value used to extraplate y(x) to right of input yin values
nout		length of xout and yout arrays
xout		array[nout] of x values at which to evaluate y(x)

Output:
yout		array[nout] of linearly interpolated y(x) values

Notes:
xin values must be monotonically increasing or decreasing.

Extrapolation of the function y(x) for xout values outside the range
spanned by the xin values in performed as follows:

    For monotonically increasing xin values,
        yout=yinl if xout<xin[0], and yout=yinr if xout>xin[nin-1].

    For monotonically decreasing xin values,
        yout=yinl if xout>xin[0], and yout=yinr if xout<xin[nin-1].

If nin==1, then the monotonically increasing case is used.
*/
void intlin_re(int nin, float dxi, float xii, float yin[], float yinl, float yinr,
               int nout, float dxo, float xoi, float yout[]);


/*
一维规则数据插值
Interpolation of a uniformly-sampled real function y(x) via a
table of 8-coefficient sinc approximations; maximum error for frequiencies
less than 0.6 nyquist is less than one percent.

Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array[nxin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values a which y(x) is output
xout		array[nxout] of x values at which y(x) is output

Output:
yout		array[nxout] of output y(x):  yout[0] = y(xout[0]), etc.

Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the xout values are not restricted
to lie within the range of sample locations defined by nxin, dxin, and
fxin.
*/
void ints8r (int nxin, float dxin, float fxin, float yin[],
             float yinl, float yinr, int nxout, float xout[], float yout[]);

/*determine index of x with respect to an array of x values*/
void xindex (int nx, float ax[], float x, int *index);

/*interpolation function used by 1D sinc interpolation*/
void mksinc (float d, int lsinc, float sinc[]);

void stoepd (int n, double r[], double g[], double f[], double a[]);
double dsinc (double x);

void intt8r (int ntable, float table[][8],
int nxin, float dxin, float fxin, float yin[], float yinl, float yinr,
int nxout, float xout[], float yout[]);

#ifdef __cplusplus
}
#endif

#endif // TOOLSALGO_H
