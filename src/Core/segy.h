#ifndef SEGY
#define SEGY

#include<math.h>
#include <stdio.h>
#include <QDir>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <arpa/inet.h>
#endif
#include "core_global.h"

/*包含Segy 头结构体
 *Segy    读写接口
 *
 */
//SEGY std head at 3201 ~ 3600
typedef struct stc_BinaryHeader400
{
    //byte 1
    int jobid;	/* Job identification number */
    //byte 2
    int lineno;	/* * Line number*/
    //byte 3
    int reelno;	/* * Tape reel number*/
    //byte 4
    short ntrace;	/* * ++++ no. of data traces per record */
    //byte 5
    short naux;	/* * no. of auxiliary traces per record */
    //byte 6
    short mudt;	/* 微秒级   内部毫秒级 需要*1000 * sample interval of these data, microseconds */
    //byte 7
    short omudt;	/* sample interval of original field data, microseconds */
    //byte 8
    short nt;	/* *样点数  no. of samples per data trace here; 32767 max - see num_samps below */
    //byte 9
    short ont;	/* no. of samples per original field data trace */
    //byte 10
    short samptype;	/* 数据类型   * data sample format code:
                       1= 4-byte float;
                       2= 4-byte fixed-point
                       3= 2-byte fixed-point;
                       4= 4-byte fixed point with gain code
                       Adding: 0= 4-byte IEEE/Java/Sun/SGI/HP float or Intel Pentium float */
    //byte 11
    short fold;	/* * expected CDP fold */
    //byte 12
    short sort;	/* 文件类型     Trace sorting code: 1= as recorded; 2= CDP ensemble,
                     3= single-fold coninuous profile, 4= horizontally stacked */
    //byte 13
    short sums;	/* Vertical sum code (how many summed) */
    //byte 14
    short sweep0;	/* Sweep frequency start, Hz */
    //byte 15
    short sweepf;	/* Sweep frequency end, Hz */
    //byte 16
    short sweepl;	/* Sweep length, milliseconds */
    //byte 17
    short sweept;	/* Sweep type code: 1=linear; 2=parabolic; 3=exponential; 4=other */
    //byte 18
    short sweeptr;	/* Trace number of sweep channel */
    //byte 19
    short taper0;	/* Sweep trace taper length at start, milliseconds */
    //byte 20
    short taperf;	/* Sweep trace taper length at end, milliseconds */
    //byte 21
    short tapert;	/* Sweep taper type: 1=linear; 2=cos^2; 3=other */
    //byte 22
    short cor;	/* Correlated data traces: 1=no; 2=yes */
    //byte 23
    short gain;	/* Binary gain recovered: 1=yes; 2=no */
    //byte 24
    short recov;	/* Gain recovery method: 1=none; 2= spherical divergence; 3=AGC;
                     4=other */
    //byte 25
    short units;	/* * Measurement system units: 1=meters; 2=feet */
    //byte 26
    short pol;	/* Impulse signal polarity: 1= Increase in pressure or upward
                     geophone case movement gives negative sample value; or
                     2= positive sample value */
    //byte 27
    short vibpol;	/* Vibratory polarity code (seismic signal lags pilot signal by):
                     1=337.5-22.5 deg; 2; 3; 4; 5; 6; 7; 8=292.5-337.5 deg */
    //byte 28
    int num_samps;	/* last officially unassigned 4 bytes go to
                              hold trace length larger than 32767 */
    int unass[84];
    char BH400[400];
}BinaryHead;

//240 bit in the front of every trace
typedef struct stc_TraceHeader240
{
    /* segy - trace identification header */

    int tracl;	/* (1-4)Trace sequence number within line
                             --numbers continue to increase if the
                             same line continues across multiple
                             SEG Y files.
                           */

    int tracr;	/* (5 - 8)Trace sequence number within SEG Y file
                             ---each file starts with trace sequence
                             one
                           */

    int fldr;	/* ( 9 - 12)Original field record number */

    int tracf;	/* (13 - 16)Trace number within original field record */

    int ep;		/* (17-20)energy source point number
                             ---Used when more than one record occurs
                             at the same effective surface location.
                           */

    int cdp;	/* (21-24)Ensemble number (i.e. CDP, CMP, CRP,...) */

    int cdpt;	/* ( 25-28)trace number within the ensemble
                             ---each ensemble starts with trace number one.
                           */

    short trid;	/* (29-30)trace identification code:
                          -1 = Other
                           0 = Unknown
                           1 = Seismic data
                           2 = Dead
                           3 = Dummy
                           4 = Time break
                           5 = Uphole
                           6 = Sweep
                           7 = Timing
                           8 = Water break
                           9 = Near-field gun signature
                          10 = Far-field gun signature
                          11 = Seismic pressure sensor
                          12 = Multicomponent seismic sensor
                                  - Vertical component
                          13 = Multicomponent seismic sensor
                                  - Cross-line component
                          14 = Multicomponent seismic sensor
                                  - in-line component
                          15 = Rotated multicomponent seismic sensor
                                  - Vertical component
                          16 = Rotated multicomponent seismic sensor
                                  - Transverse component
                          17 = Rotated multicomponent seismic sensor
                                  - Radial component
                          18 = Vibrator reaction mass
                          19 = Vibrator baseplate
                          20 = Vibrator estimated ground force
                          21 = Vibrator reference
                          22 = Time-velocity pairs
                          23 ... N = optional use
                                  (maximum N = 32,767)

                          Following are CWP id flags:

                          109 = autocorrelation
                          110 = Fourier transformed - no packing
                               xr[0],xi[0], ..., xr[N-1],xi[N-1]
                          111 = Fourier transformed - unpacked Nyquist
                               xr[0],xi[0],...,xr[N/2],xi[N/2]
                          112 = Fourier transformed - packed Nyquist
                               even N:
                               xr[0],xr[N/2],xr[1],xi[1], ...,
                                  xr[N/2 -1],xi[N/2 -1]
                                  (note the exceptional second entry)
                               odd N:
                               xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
                                  xr[(N-1)/2 -1],xi[(N-1)/2 -1],xi[(N-1)/2]
                                  (note the exceptional second & last entries)
                          113 = Complex signal in the time domain
                               xr[0],xi[0], ..., xr[N-1],xi[N-1]
                          114 = Fourier transformed - amplitude/phase
                               a[0],p[0], ..., a[N-1],p[N-1]
                          115 = Complex time signal - amplitude/phase
                               a[0],p[0], ..., a[N-1],p[N-1]
                          116 = Real part of complex trace from 0 to Nyquist
                          117 = Imag part of complex trace from 0 to Nyquist
                          118 = Amplitude of complex trace from 0 to Nyquist
                          119 = Phase of complex trace from 0 to Nyquist
                          121 = Wavenumber time domain (k-t)
                          122 = Wavenumber frequency (k-omega)
                          123 = Envelope of the complex time trace
                          124 = Phase of the complex time trace
                          125 = Frequency of the complex time trace
                          130 = Depth-Range (z-x) traces
                          201 = Seismic data packed to bytes (by supack1)
                          202 = Seismic data packed to 2 bytes (by supack2)
                          */

    short nvs;	/*(31-32) Number of vertically summed traces yielding
                             this trace. (1 is one trace,
                             2 is two summed traces, etc.)
                           */

    short nhs;	/*33-34 Number of horizontally summed traces yielding
                             this trace. (1 is one trace
                             2 is two summed traces, etc.)
                           */

    short duse;	/*35-36 Data use:
                                  1 = Production
                                  2 = Test
                           */

    int offset;	/* 37-40 Distance from the center of the source point
                             to the center of the receiver group
                             (negative if opposite to direction in which
                             the line was shot).
                           */

    int gelev;	/* 41-44 Receiver group elevation from sea level
                             (all elevations above the Vertical datum are
                             positive and below are negative).
                           */

    int selev;	/*45-48 Surface elevation at source. */

    int sdepth;	/*49-52 Source depth below surface (a positive number). */

    int gdel;	/*53-56 Datum elevation at receiver group. */

    int sdel;	/*57-60 Datum elevation at source. */

    int swdep;	/*61-64 Water depth at source. */

    int gwdep;	/*65-68 Water depth at receiver group. */

    short scalel;	/*69-70 Scalar to be applied to the previous 7 entries
                             to give the real value.
                             Scalar = 1, +10, +100, +1000, +10000.
                             If positive, scalar is used as a multiplier,
                             if negative, scalar is used as a divisor.
                           */

    short scalco;	/*71-72 Scalar to be applied to the next 4 entries
                             to give the real value.
                             Scalar = 1, +10, +100, +1000, +10000.
                             If positive, scalar is used as a multiplier,
                             if negative, scalar is used as a divisor.
                           */

    int  sx;	/*73-76 Source coordinate - X */

    int  sy;	/*77-80 Source coordinate - Y */

    int  gx;	/*81-84 Group coordinate - X */

    int  gy;	/* 85-88  Group coordinate - Y */

    short counit;	/*89-90 Coordinate units: (for previous 4 entries and
                                  for the 7 entries before scalel)
                             1 = Length (meters or feet)
                             2 = Seconds of arc
                             3 = Decimal degrees
                             4 = Degrees, minutes, seconds (DMS)

                          In case 2, the X values are longitude and
                          the Y values are latitude, a positive value designates
                          the number of seconds east of Greenwich
                                  or north of the equator

                          In case 4, to encode +-DDDMMSS
                          counit = +-DDD*10^4 + MM*10^2 + SS,
                          with scalco = 1. To encode +-DDDMMSS.ss
                          counit = +-DDD*10^6 + MM*10^4 + SS*10^2
                          with scalco = -100.
                          */

    short wevel;	/*91-92 Weathering velocity. */

    short swevel;	/*93-94 Subweathering velocity. */

    short sut;	/*95-96 Uphole time at source in milliseconds. */

    short gut;	/*97-98 Uphole time at receiver group in milliseconds. */

    short sstat;	/*99-100 Source static correction in milliseconds. */

    short gstat;	/*101-102 Group static correction  in milliseconds.*/

    short tstat;	/*103-104 Total static applied  in milliseconds.
                             (Zero if no static has been applied.)
                          */

    short laga;	/*105-106 Lag time A, time in ms between end of 240-
                             byte trace identification header and time
                             break, positive if time break occurs after
                             end of header, time break is defined as
                             the initiation pulse which maybe recorded
                             on an auxiliary trace or as otherwise
                             specified by the recording system */

    short lagb;	/*107-108 lag time B, time in ms between the time break
                             and the initiation time of the energy source,
                             may be positive or negative */

    short delrt;	/*109-110 delay recording time, time in ms between
                             initiation time of energy source and time
                             when recording of data samples begins
                             (for deep water work if recording does not
                             start at zero time) */

    short muts;	/*111-112 mute time--start */

    short mute;	/*113-114 mute time--end */

    unsigned short ns;	/*115-116 number of samples in this trace */

    unsigned short dt;	/*117-118 sample interval; in micro-seconds */

    short gain;	/*119-120 gain type of field instruments code:
                                  1 = fixed
                                  2 = binary
                                  3 = floating point
                                  4 ---- N = optional use */

    short igc;	/*121-122 instrument gain constant */

    short igi;	/*123-124 instrument early or initial gain */

    short corr;	/*125-126 correlated:
                                  1 = no
                                  2 = yes */

    short sfs;	/*127-128 sweep frequency at start */

    short sfe;	/*129-130 sweep frequency at end */

    short slen;	/*131-132 sweep length in ms */

    short styp;	/*133-134 sweep type code:
                                  1 = linear
                                  2 = cos-squared
                                  3 = other */

    short stas;	/*135-136 sweep trace length at start in ms */

    short stae;	/*137-138 sweep trace length at end in ms */

    short tatyp;	/*139-140 taper type: 1=linear, 2=cos^2, 3=other */

    short afilf;	/*141-142 alias filter frequency if used */

    short afils;	/*143-144 alias filter slope */

    short nofilf;	/*145-146 notch filter frequency if used */

    short nofils;	/*147-148 notch filter slope */

    short lcf;	/*149-150 low cut frequency if used */

    short hcf;	/*151-152 high cut frequncy if used */

    short lcs;	/*153-154 low cut slope */

    short hcs;	/*155-156 high cut slope */

    short year;	/*157-158 year data recorded */

    short day;	/*159-160 day of year */

    short hour;	/*161-162 hour of day (24 hour clock) */

    short minute;	/*163-164 minute of hour */

    short sec;	/*165-166 second of minute */

    short timbas;	/*167-168 time basis code:
                                  1 = local
                                  2 = GMT
                                  3 = other */

    short trwf;	/*169-170 trace weighting factor, defined as 1/2^N
                             volts for the least sigificant bit */

    short grnors;	/*171-172 geophone group number of roll switch
                             position one */

    short grnofr;	/*173-174 geophone group number of trace one within
                             original field record */

    short grnlof;	/*175-176 geophone group number of last trace within
                             original field record */

    short gaps;	/*177-178 gap size (total number of groups dropped) */

    short otrav;	/*179-180 overtravel taper code:
                                  1 = down (or behind)
                                  2 = up (or ahead) */

    int unass[15];  //===181-240
} TraceHead;


//文件信息
typedef struct stc_SegyInfo
{
    int  ns;             //number of Data samples
    int  sampleInterval; //(us)
    long long traces;    //total traces
}SegyInfo;

//打开方式
typedef int SegyOpenMode;
#define  Read_Only        0
#define  Write_Only       1
#define  Read_Write        2

#define NUM_FILE "00000"

#define CHAR_1   sizeof(char)
#define SHORT_2  sizeof(short)
#define INT_4    sizeof(int)
#define FLOAT_4  sizeof(float)
#define LONG_8   sizeof(long long)
#define DOUBLE_8 sizeof(double)

const int BINARY3200 = 3200; //===
const int BINARY400 = 400;  //===
const int HEADLENTH = 240;  //===
const int BINARY3600 = 3600;
const long long SIZE10G = 10737418240;

#define MAP_MAX_NUM   200
//===
enum eEndian
{
    NOENDI  = -1,
    BIGENDI = 0,
    LITENDI = 1,
    PDBENDI = 2
};

typedef struct openTrace
{
    openTrace()
    {
        g_Endian = BIGENDI;
    }
    int         ns;         //===采样率
    short       dtype;      //===数据类型
    long long   TotalTrace; //===总道数
    long long   TraceNum;
    long long   fileTrace;  //===单文件最大道数
    int         DataLen;    //===数据长度
    eEndian     g_Endian;   //===大小端
    QString     DatafName;  //===数据文件名
    FILE        *fpData;    //===数据文件指针
    SegyInfo    segInfo;    //===数据信息
}OpenTrace;

const char e2a[256]=
{
   0,  1,  2,  3,(char)156,  9,(char)134,127,(char)151,(char)141,(char)142, 11, 12, 13, 14, 15,
   16, 17, 18, 19,(char)157,(char)133,  8,(char)135, 24, 25,(char)146,(char)143, 28, 29, 30, 31,
   (char)128,(char)129,(char)130,(char)131,(char)132, 10, 23, 27,(char)136,(char)137,(char)138,(char)139,(char)140,  5,  6,  7,
   (char)144,(char)145, 22,(char)147,(char)148,(char)149,(char)150,  4,(char)152,(char)153,(char)154,(char)155, 20, 21,(char)158, 26,
   32,(char)160,(char)161,(char)162,(char)163,(char)164,(char)165,(char)166,(char)167,(char)168, 91, 46, 60, 40, 43, 33,
   38,(char)169,(char)170,(char)171,(char)172,(char)173,(char)174,(char)175,(char)176,(char)177, (char)93, 36, 42, 41, 59, 94,
   45, 47,(char)178,(char)179,(char)180,(char)181,(char)182,(char)183,(char)184,(char)185,(char)124, 44, 37, 95, 62, 63,
   (char)186,(char)187,(char)188,(char)189,(char)190,(char)191,(char)192,(char)193,(char)194, 96, 58, 35, 64, 39, 61, 34,
   (char)195, 97, 98, 99,(char)100,(char)101,(char)102,(char)103,(char)104,(char)105,(char)196,(char)197,(char)198,(char)199,(char)200,(char)201,
   (char)202,(char)106,107,108,109,110,111,112,113,114,(char)203,(char)204,(char)205,(char)206,(char)207,(char)208,
   (char)209,(char)126,115,116,117,118,119,(char)120,(char)121,(char)122,(char)210,(char)211,(char)212,(char)213,(char)214,(char)215,
   (char)216,(char)217,(char)218,(char)219,(char)220,(char)221,(char)222,(char)223,(char)224,(char)225,(char)226,(char)227,(char)228,
   (char)229,(char)230,(char)231,
   (char)123, 65, 66, 67, 68, 69, 70, 71, 72, 73,(char)232,(char)233,(char)234,(char)235,(char)236,(char)237,
   (char)125, 74, 75, 76, 77, 78, 79, 80, 81, 82,(char)238,(char)239,(char)240,(char)241,(char)242,(char)243,
   92,(char)159, 83, 84, 85, 86, 87, 88, 89, 90,(char)244,(char)245,(char)246,(char)247,(char)248,(char)249,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57,(char)250,(char)251,(char)252,(char)253,(char)254,(char)255
};

const char a2e[256] =
{
    (char)0,  (char)1,  (char)2,  (char)3,  (char)55, (char)45, (char)46, (char)47, (char)22, (char)5, (char)37, (char)11, (char)12, (char)13, (char)14, (char)15,
    (char)16, (char)17, (char)18, (char)19, (char)60, (char)61, (char)50, (char)38, (char)24, (char)25, (char)63, (char)39, (char)28, (char)29, (char)30, (char)31,
    (char)64, (char)79, (char)127,(char)123,(char)91, (char)108,(char)80, (char)125, (char)77, (char)93, (char)92, (char)78,(char)107, (char)96, (char)75,(char)97,
    (char)240,(char)241,(char)242,(char)243,(char)244,(char)245,(char)246,(char)247,(char)248,(char)249,(char)122, (char)94, (char)76,(char)126,(char)110,(char)111,
    (char)124,(char)193,(char)194,(char)195,(char)196,(char)197,(char)198,(char)199,(char)200,(char)201,(char)209,(char)210,(char)211,(char)212,(char)213,(char)214,
    (char)215,(char)216,(char)217,(char)226,(char)227,(char)228,(char)229,(char)230,(char)231,(char)232,(char)233, (char)74,(char)224, (char)90, (char)95,(char)109,
    (char)121,(char)129,(char)130,(char)131,(char)132,(char)133,(char)134,(char)135,(char)136,(char)137,(char)145,(char)146,(char)147,(char)148,(char)149,(char)150,
    (char)151,(char)152,(char)153,(char)162,(char)163,(char)164,(char)165,(char)166,(char)167,(char)168,(char)169,(char)192,(char)106,(char)208,(char)161,  (char)7,
    (char)32, (char)33, (char)34, (char)35, (char)36, (char)21,  (char)6, (char)23, (char)40, (char)41, (char)42, (char)43, (char)44,  (char)9, (char)10,(char) 27,
    (char)48, (char)49, (char)26, (char)51, (char)52, (char)53,(char) 54,  (char)8, (char)56, (char)57, (char)58, (char)59,  (char)4, (char)20, (char)62,(char)225,
    (char)65, (char)66, (char)67, (char)68, (char)69, (char)70, (char)71, (char)72, (char)73, (char)81, (char)82, (char)83, (char)84, (char)85, (char)86,(char) 87,
    (char)88, (char)89, (char)98, (char)99,(char)100,(char)101,(char)102,(char)103,(char)104,(char)105,(char)112,(char)113,(char)114,(char)115,(char)116,(char)117,
    (char)118, (char)119,(char)120,(char)128,(char)138,(char)139,(char)140,(char)141,(char)142,(char)143,(char)144,(char)154,(char)155,(char)156,(char)157,(char)158,
    (char)159,(char)160,(char)170,(char)171,(char)172,(char)173,(char)174,(char)175,(char)176,(char)177,(char)178,(char)179,(char)180,(char)181,(char)182,(char)183,
    (char)184,(char)185,(char)186,(char)187,(char)188,(char)189,(char)190,(char)191,(char)202,(char)203,(char)204,(char)205,(char)206,(char)207,(char)218,(char)219,
    (char)220,(char)221,(char)222,(char)223,(char)234,(char)235,(char)236,(char)237,(char)238,(char)239,(char)250,(char)251,(char)252,(char)253,(char)254,(char)255
};

class GobsInterface
{
private:
    GobsInterface();
    static GobsInterface *GobsInface;
public:
    static GobsInterface *getInstance()
    {
        if(NULL == GobsInface)
            GobsInface = new GobsInterface;
        return GobsInface;
    }
    //===打开SEGY文件
    int OpenSEGYFileFunc(OpenTrace *openTr, QString SEGYFName, int Mode, char *Bna3200, BinaryHead *BinHead, SegyInfo *DtInfo);
    //====写数据时将400字节卷头小端转换为大端
    void WriteGetBinHead400(BinaryHead *);
    //====读取数据时将400字节卷头大端转换为小端
    void CreateBinHead(BinaryHead *);
    //====读数据时将3200字节卷头转换编码
    void EBD2QStr(char *);
    //====写数据时将3200字节卷头转换编码
    void QStr2EBD(char *);

    //===读取数据
    int ReadSEGYDataFunc(OpenTrace *openTr, long long statnum, long long trnum, TraceHead *trhead, char *trdata, long long *currtr);
    void ChangeTraceDataBigEndian(OpenTrace *openTr, char *tracedata, long long CurrTrNum);
    void ibm_to_ieee( int *from, int *to, int n );
    void ChangeTraceHeadBigEndian(TraceHead *g_TrHead_240, long long CurrTrNum);

    //===关闭数据
    int CloseSEGYDataFunc(OpenTrace *openTr);

    //===写数据
    int WriteSEGYDataFunc(OpenTrace *openTr, TraceHead *trhead, char *trdata);
    void ChangeTraceHeadLittleEndian(TraceHead *trhead);//===写数据将道头小端转换为大端
    void changeEnd(char *trace_data, int ns);           //===写数据将数据小端转换为大端
    void ieee_to_ibm(int from[], int to[], int n);      //===写数据将IEEE数据转换为IBM数据
};


//iointerfaces-----------------------------------------------------
//===打开SEGY数据接口
//===参数说明:
//=== path      ： 文件存放路径
//=== filename  ： 文件名
//=== mode      ： 打开文件模式 0:只读;1:只写;2:读写
//=== Bnary3200 : 3200字节卷头
//=== binary_head：读打开是返回400字节卷头信息,写打开时将400字节卷头写入到SEGY数据文件中
//=== datainfo  ： 当前数据部分信息
//=== fileptr   ： 返回值，文件句柄，用于读写
//=== ok        ： 返回值，成功返回0,失败返回非0
CORESHARED_EXPORT OpenTrace *gobs_open_segy_func(const char*filename, int *mode, char *Bnary3200, BinaryHead *binary_head, SegyInfo *datainfo, int *ok);

//===读取SEGY数据接口
//===参数说明:
//=== fileptr ： open时返回的文件句柄
//=== statnum ： 读取数据的起始道
//=== trnum   ： 读取数据的道数
//=== headbuff:  存放道头
//=== databuff:  存放数据
//=== currtrnum: 实际读取的道数
//=== ok      ： 返回值，成功返回0，失败返回非0
CORESHARED_EXPORT void gobs_read_segy_func(OpenTrace *openTr, long long statnum, long long trnum, TraceHead *headbuff, char *databuff, long long *currtrnum, int *ok);

//===写SEGY数据接口
//===参数说明:
//=== fileptr ： open时返回的文件句柄
//=== statnum ： 写数据的起始道
//=== trnum   ： 写数据的道数
//=== headbuff:  道头
//=== databuff:  数据
//=== ok      ： 返回值，成功返回0，失败返回非0
CORESHARED_EXPORT void gobs_write_segy_func(OpenTrace *openTr, TraceHead *headbuff, char *databuff, int *ok);

//===关闭open时打开的文件
//===参数说明
//===   fileptr : open时返回的文件句柄
CORESHARED_EXPORT void gobs_close_segy_func(OpenTrace *openTr, int *ok);


#endif // SEGY
































