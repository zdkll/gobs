#include "ToolsAlgo.h"

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

void gobs_1D_interpolation_regular (
    float	*data_in,	int		ni1,		float	xii,		float	dxi,
    float	*data_out,	int		no1,		float	xoi,		float	dxo,
    float	xinl,		float	xinr,		int		inter_type,	int		*isok
)
{
    int		ix;
    float	*xout;

    *isok = 0;

    if (NULL == data_in)
    {
        fprintf (stderr, "Error! Input data NULL\n");
        *isok = -1;
        return ;
    }

    if (NULL == data_out)
    {
        fprintf (stderr, "Error! Output space NULL\n");
        *isok = -2;
        return ;
    }

    //根据输入的参数选择不同的插值算法。
    switch (inter_type)
    {

    case LINEARINTER:	// surname = 1
        intlin_re(ni1, dxi, xii, data_in, xinl, xinr, no1, dxo, xoi, data_out);
        break ;

    case SINCINTER:	// surname = 5
        xout = (float *)malloc (sizeof(float) * no1);
        for (ix=0; ix<no1; ix++)
            xout[ix] = xoi+ ix * dxo;
        ints8r (ni1, dxi, xii, data_in, xinl, xinr, no1, xout, data_out);
        free (xout);
        break ;
    }

    return ;
}


void intlin_re(int nin, float dxi, float xii, float yin[], float yinl, float yinr,
            int nout, float dxo, float xoi, float yout[])
/*****************************************************************************
规则一维线性插值
evaluate y(x) via linear interpolation of y(x[0]), y(x[1]), ...
******************************************************************************
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
******************************************************************************
Notes:
xin values must be monotonically increasing or decreasing.

Extrapolation of the function y(x) for xout values outside the range
spanned by the xin values in performed as follows:

    For monotonically increasing xin values,
        yout=yinl if xout<xin[0], and yout=yinr if xout>xin[nin-1].

    For monotonically decreasing xin values,
        yout=yinl if xout>xin[0], and yout=yinr if xout<xin[nin-1].

If nin==1, then the monotonically increasing case is used.
*****************************************************************************/
{
    static int idx;
    int ix, jout;
    float x;
    float *xin;
    float *xout;
    xin  = (float *)malloc (sizeof(float) * nin);
    xout = (float *)malloc (sizeof(float) * nout);

    for (ix= 0; ix< nin;  ix++)
        xin[ix]  = xii+ ix * dxi;
    for (ix= 0; ix< nout; ix++)
        xout[ix] = xoi+ ix * dxo;

    /* if input x values are monotonically increasing, then */
    if (xin[0]<=xin[nin-1]) {
        for (jout=0; jout<nout; jout++) {
            x = xout[jout];
            if (x<xin[0])
                yout[jout] = yinl;
            else if (x>xin[nin-1])
                yout[jout] = yinr;
            else if (x==xin[nin-1] || nin==1)
                yout[jout] = yin[nin-1];
            else {
                xindex(nin,xin,x,&idx);
                yout[jout] = yin[idx]+(x-xin[idx])
                        *(yin[idx+1]-yin[idx])
                        /(xin[idx+1]-xin[idx]);
            }
        }

        /* else, if input x values are monotonically decreasing, then */
    } else {
        for (jout=0; jout<nout; jout++) {
            x = xout[jout];
            if (x>xin[0])
                yout[jout] = yinl;
            else if (x<xin[nin-1])
                yout[jout] = yinr;
            else if (x==xin[nin-1] || nin==1)
                yout[jout] = yin[nin-1];
            else {
                xindex(nin,xin,x,&idx);
                yout[jout] = yin[idx]+(x-xin[idx])
                        *(yin[idx+1]-yin[idx])
                        /(xin[idx+1]-xin[idx]);
            }
        }
    }
    free(xin);
    free(xout);
}
void xindex (int nx, float ax[], float x, int *index)
/*****************************************************************************
determine index of x with respect to an array of x values
******************************************************************************
Input:
nx		number of x values in array ax
ax		array[nx] of monotonically increasing or decreasing x values
x		the value for which index is to be determined
index		index determined previously (used to begin search)

Output:
index		for monotonically increasing ax values, the largest index
        for which ax[index]<=x, except index=0 if ax[0]>x;
        for monotonically decreasing ax values, the largest index
        for which ax[index]>=x, except index=0 if ax[0]<x
******************************************************************************
Notes:
This function is designed to be particularly efficient when called
repeatedly for slightly changing x values; in such cases, the index
returned from one call should be used in the next.
*****************************************************************************/
{
    int lower,upper,middle,step;

    /* initialize lower and upper indices and step */
    lower = *index;
    if (lower<0) lower = 0;
    if (lower>=nx) lower = nx-1;
    upper = lower+1;
    step = 1;

    /* if x values increasing */
    if (ax[nx-1]>ax[0]) {

        /* find indices such that ax[lower] <= x < ax[upper] */
        while (lower>0 && ax[lower]>x) {
            upper = lower;
            lower -= step;
            step += step;
        }
        if (lower<0) lower = 0;
        while (upper<nx && ax[upper]<=x) {
            lower = upper;
            upper += step;
            step += step;
        }
        if (upper>nx) upper = nx;

        /* find index via bisection */
        while ((middle=(lower+upper)>>1)!=lower) {
            if (x>=ax[middle])
                lower = middle;
            else
                upper = middle;
        }

        /* else, if not increasing */
    } else {

        /* find indices such that ax[lower] >= x > ax[upper] */
        while (lower>0 && ax[lower]<x) {
            upper = lower;
            lower -= step;
            step += step;
        }
        if (lower<0) lower = 0;
        while (upper<nx && ax[upper]>=x) {
            lower = upper;
            upper += step;
            step += step;
        }
        if (upper>nx) upper = nx;

        /* find index via bisection */
        while ((middle=(lower+upper)>>1)!=lower) {
            if (x<=ax[middle])
                lower = middle;
            else
                upper = middle;
        }
    }

    /* return lower index */
    *index = lower;
}



void ints8r (int nxin, float dxin, float fxin, float yin[],
             float yinl, float yinr, int nxout, float xout[], float yout[])
/*****************************************************************************
一维规则数据插值
Interpolation of a uniformly-sampled real function y(x) via a
table of 8-coefficient sinc approximations; maximum error for frequiencies
less than 0.6 nyquist is less than one percent.
******************************************************************************
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
******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the xout values are not restricted
to lie within the range of sample locations defined by nxin, dxin, and
fxin.
*****************************************************************************/
{
    static float table[NTABLE_INTERP][LTABLE];
    static int tabled=0;
    int jtable;
    float frac;

    /* tabulate sinc interpolation coefficients if not already tabulated */
    if (!tabled) {
        for (jtable=1; jtable< NTABLE_INTERP- 1; jtable++){
            frac = (float)jtable/(float)(NTABLE_INTERP-1);
            mksinc(frac,LTABLE,&table[jtable][0]);
        }
        for (jtable= 0; jtable<LTABLE; jtable++){
            table[0][jtable] = 0.0;
            table[NTABLE_INTERP-1][jtable] = 0.0;
        }
        table[0][LTABLE/2-1] = 1.0;
        table[NTABLE_INTERP-1][LTABLE/2] = 1.0;
        tabled = 1;
    }

    /* interpolate using tabulated coefficients */
    intt8r(NTABLE_INTERP,table,nxin,dxin,fxin,yin,yinl,yinr,nxout,xout,yout);

}


void mksinc (float d, int lsinc, float sinc[])
{
    int j;
    double s[20],a[20],c[20],work[20],fmax;

    /* compute auto-correlation and cross-correlation arrays */
    fmax = 0.066+0.265*log((double)lsinc);
    fmax = (fmax<1.0)?fmax:1.0;
    for (j=0; j<lsinc; j++) {
        a[j] = dsinc(fmax*j);
        c[j] = dsinc(fmax*(lsinc/2-j-1+d));
    }

    /* solve symmetric Toeplitz system for the sinc approximation */
    stoepd(lsinc,a,c,s,work);
    for (j=0; j<lsinc; j++)
        sinc[j] = s[j];
}


void stoepd (int n, double r[], double g[], double f[], double a[])
{
    int i,j;
    double v,e,c,w,bot;

    if (r[0] == 0.0) return;

    a[0] = 1.0;
    v = r[0];
    f[0] = g[0]/r[0];

    for (j=1; j<n; j++) {

        /* solve Ra=v as in Claerbout, FGDP, p. 57 */
        a[j] = 0.0;
        f[j] = 0.0;
        for (i=0,e=0.0; i<j; i++)
            e += a[i]*r[j-i];
        c = e/v;
        v -= c*e;
        for (i=0; i<=j/2; i++) {
            bot = a[j-i]-c*a[i];
            a[i] -= c*a[j-i];
            a[j-i] = bot;
        }

        /* use a and v above to get f[i], i = 0,1,2,...,j */
        for (i=0,w=0.0; i<j; i++)
            w += f[i]*r[j-i];
        c = (w-g[j])/v;
        for (i=0; i<=j; i++)
            f[i] -= c*a[j-i];
    }
}


double dsinc (double x)
{
    double pix;
    if (x==0.0) {
        return 1.0;
    } else {
        pix = PI*x;
        return sin(pix)/pix;
    }
}

void intt8r (int ntable, float table[][8],
int nxin, float dxin, float fxin, float yin[], float yinl, float yinr,
int nxout, float xout[], float yout[])
{
    int ioutb,nxinm8,ixout,ixoutn,kyin,ktable,itable;
    float xoutb,xoutf,xouts,xoutn,frac,fntablem1,yini,sum,
            *yin0,*table00,*pyin,*ptable;

    /* compute constants */
    ioutb = -3-8;
    xoutf = fxin;
    xouts = 1.0/dxin;
    xoutb = 8.0-xoutf*xouts;
    fntablem1 = (float)(ntable-1);
    nxinm8 = nxin-8;
    yin0 = &yin[0];
    table00 = &table[0][0];

    /* loop over output samples */
    for (ixout=0; ixout<nxout; ixout++) {

        /* determine pointers into table and yin */
        xoutn = xoutb+xout[ixout]*xouts;
        ixoutn = (int)xoutn;
        kyin = ioutb+ixoutn;
        pyin = yin0+kyin;
        frac = xoutn-(float)ixoutn;
        ktable = frac>=0.0?frac*fntablem1+0.5:(frac+1.0)*fntablem1-0.5;
        ptable = table00+ktable*8;

        /* if totally within input array, use fast method */
        if (kyin>=0 && kyin<=nxinm8) {
            yout[ixout] =
                    pyin[0]*ptable[0]+
                    pyin[1]*ptable[1]+
                    pyin[2]*ptable[2]+
                    pyin[3]*ptable[3]+
                    pyin[4]*ptable[4]+
                    pyin[5]*ptable[5]+
                    pyin[6]*ptable[6]+
                    pyin[7]*ptable[7];

            /* else handle end effects with care */
        } else {

            /* sum over 8 tabulated coefficients */
            for (itable=0,sum=0.0; itable<8; itable++,kyin++) {
                if (kyin<0)
                    yini = yinl;
                else if (kyin>=nxin)
                    yini = yinr;
                else
                    yini = yin[kyin];
                sum += yini*(*ptable++);
            }
            yout[ixout] = sum;
        }
    }
}

