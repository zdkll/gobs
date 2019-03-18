#ifndef GRAPHFUNCTIONS_H
#define GRAPHFUNCTIONS_H

#include "gpublicso_global.h"

//离散刻度信息
struct DScaleUnit
{
    DScaleUnit():
        pixCount(1.0),dataCount(1)
      ,pixOffset(0.0),dataOffset(0){}
    float pixCount;  //像素数
    int   dataCount; //包含的数据宽度,一般是1
    float pixOffset; //每道数据展宽的一半
    int   dataOffset;
};

//连续刻度信息
struct CScaleUnit
{
    CScaleUnit():
        corMin(0.0),corStep(1.0),corNum(1){}
    float corMin; //起始值
    float corStep;//步长
    int   corNum; //数量
};

namespace GraphFunctions {

//离散型刻度计算，一般用于不连续刻度，如横坐标显示道等
GPUBLICSOSHARED_EXPORT DScaleUnit makeDeparatedUnit(int pixWidth,int dataNum,int pixLen);

//连续型刻度计算，用于时间，深度，距离等计算,pixwidth 总的刻度宽度；pixlen 最小刻度间隔
GPUBLICSOSHARED_EXPORT CScaleUnit makeContinuousUnit(float min,float max,int pixWidth,int pixLen);
}

#endif // GRAPHFUNCTIONS_H
