#ifndef GRAPHBASE_2D_H
#define GRAPHBASE_2D_H

#define ZOOM_MIN_PIXEL 3
#define LIMIT_SCALE_DATA_IN_MAP 1.0f //设定放大极限

//图像区Margin---------------------
#define  Left_Scale_Margin    60
#define  Top_Scale_Margin     48
#define  Right_Scale_Margin   60
#define  Bottom_Scale_Margin  20

//拖动框 标记长度--------------------
#define Drawer_Border_Length   35
#define Drawer_Border_Margin   3

//缩放距离限制
#define Zoom_MIN_LENGTH        5

//刻度间隔------------------------
#define Min_Hori_Scale_Space      50
#define Min_Vert_Scale_Space      40

//主刻度长度
#define Main_Tick_Length          8
#define Sub_Tick_Length           4
//次刻度数量
#define Sub_Tick_Num              10


#include <QtCore>
#include "gsegy.h"

//当前数据信息
struct GatherInfo
{
    GatherInfo():
        NTraces(0){}
    QString gatherName;
    int Ns;     //采样点数
    int sampleInterval; //(us)采样间隔,微秒级对应距离mm, 毫秒对应距离M米
    int DType;  //数据类型
    long long NTraces;//读取道数
    int gatherNum;//总的道集
    int gatherNo; //当前道集号
};

//绘图操作
enum ePaintAction
{
    NODRAW = 1,
    DRAW = 3,
    REDRAW = 4,
    OBJDRAW = 5
};

//图像模式
enum eGraphicMode
{
    VD = 1,
    VA = 2,
    VAWG = 3,
    WG = 4,
    VDWG = 5,
    VDVA = 6
};

//数据最大值属性: Global，Trace
enum eScale
{
    Global = 0,
    Section = 1,
    Trace = 2,
    AGC = 3,
    GAGC =4
};
enum GridPosition
{
    NoneGrid = 0x00,
    HorizontalGrid = 0x1,
    VerticalGrid  = 0x2,
    BothGrid      = HorizontalGrid | VerticalGrid
};
//填充模式
enum eBrushMode
{
    NoBrush = 0,
    BrushPositive = 1,
    BrushNegative = 2,
    BrushBoth = 3
};

//鼠标功能状态
enum eMouseFunction
{
    MF_NOTHING = 0,
    MF_ZOOM = 1,
    MF_PICK = 2,
    MF_ANALYSE = 4
};

enum DataType
{
    Short_Type = 0,
    Int_Type   = 1,
    Float_Type = 2
};
enum VertScaleType
{
    ScaleTime = 0,
    ScaleDepth = 1
};

//道头,默认显示tracf
struct HeaderFieldPos
{
    HeaderFieldPos():
        fieldName("tracf")
      ,dType(Int_Type)
      ,pos(12){}
    QString  fieldName;
    DataType dType;
    int      pos;//位置,相对于道头首地址位置

    //计算坐标值
    static int headerValue(char *header,int pos,DataType dType)
    {
        int value;
        switch (dType) {
        case Short_Type:
            value = *(short*)(header+pos);
            break;
        case Int_Type:
            value = *(int*)(header+pos);
            break;
        case Float_Type:
            value = *(float*)(header+pos);
            break;
        default:
            value = *(float*)(header+pos);
            break;
        }
        return value;
    }
};

//图像属性信息结构体
struct GraphAttribute
{
    GraphAttribute()
        :dispStep(1)
        ,mode(VAWG)
        ,scaleType(Global)
        ,brushMode(BrushPositive)
        ,gainPercent(1.0)
        ,interType(2)
        ,gridPosition(HorizontalGrid)
        ,horiAxesMinLen(Min_Hori_Scale_Space)
        ,vertAxesMinLen(Min_Vert_Scale_Space)
        ,mouseFunction(MF_NOTHING)
        ,vertScaleType(ScaleTime)
        ,components(1){}

    int          dispStep;//显示道间隔
    eGraphicMode mode;
    eScale       scaleType;
    eBrushMode   brushMode;
    float        gainPercent;
    int          interType;
    GridPosition gridPosition;
    int          horiAxesMinLen;
    int          vertAxesMinLen;


    //当前图像鼠标状态
    eMouseFunction mouseFunction;
    //显示道头位置
    HeaderFieldPos showedHeadFieldPos;
    VertScaleType  vertScaleType;
    int            components;   //分量数，1或者4分量
};


const QStringList showHeaderPosList = QStringList()<<"Trace No."
                                                  <<"FFID"
                                                 <<"TraceSeqFFID"
                                                <<"TraceSeqLine"
                                               <<"TraceSeqReel"
                                              <<"CDP"
                                             <<"OFFSET"
                                            <<"Source Point"
                                           <<"Source X"
                                          <<"Source Y"
                                         <<"Receiver X"
                                        <<"Receiver Y"
                                       <<"Receiver Line"
                                      <<"Receiver No.";

namespace ImageBaseFunction{
//计算纵横某个方向的放大倍数
float getZoomValue(const int &oriSize,const int &newSize,const float &oriZoomValue);

//根据当前放大倍数，原始取样总数，坐标值（x或y），总长度，算出取样点下标偏移量。 f(off) = totalNum * zoom * (X坐标 or Y坐标/rectSize)
//例如，放大倍数为1，总取样点数100个，在长度200的图像轴上的坐标50处取样，取到的数据下标偏移量就是24（从0开始）
int   getCurrentIdxOffset(const float &zoom, const int &totalNum, const int &rectSize, const int &XorY);

//波形图step计算，在预设值的基础上优化抽稀，确保每道数据宽度至少一个像素,如果原始设置就能保证的话，直接返回原始step
int getGoodStep(int width, int step, int horizNum,const int minPix = 4);

//从数据中统计道极值/全局极值
void getTraceExtremumFromData(float *data, int sd, int fd, float *valminl, float *valmaxl, float *minGlobal, float *maxGlobal);

}

#endif // GRAPHBASE_2D_H
