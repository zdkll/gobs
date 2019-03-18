#include "graphfunctions.h"

#include <assert.h>

DScaleUnit GraphFunctions::makeDeparatedUnit(int pixWidth,int dataNum,int pixLen)
{
    assert(dataNum >0);
    DScaleUnit unit;
    //找到合适的数据间隔
    int   dataCount = 1;//数据间隔从1开始计算
    float pix       =  float(pixWidth)/float(dataNum);
    while(pix*dataCount<pixLen){
        if(dataCount+1>dataNum)
            break;
        dataCount++;
    }
    unit.dataCount = dataCount;
    unit.pixCount  = pix*dataCount;

    //数据偏移
    unit.dataOffset = dataCount/2;

    //像素偏移
    unit.pixOffset  = int(dataCount/2) * pix + pix/2.0f;

    return unit;
}

CScaleUnit GraphFunctions::makeContinuousUnit(float min,float max,int pixWidth,int pixLen)
{
    //标准步长的规范 /5 或者 /10
    float c_scale_standard_step[6] = {0.1,0.2,0.25,0.5,1.0};

    CScaleUnit unit;
    //刻度太小分不清
    if((max - min )<10e-5)
    {
        unit.corMin = (max + min)/2.0f;
        unit.corNum = 1;
        unit.corStep = 0;

        return unit;
    }

    //主刻度数(段)-------
    int numTicks = pixWidth/pixLen;
    numTicks = numTicks<1?1:numTicks;

    //步长
    float corStep = (max- min)/numTicks;
    //步长数量级
    int temp  = ceil(log(corStep)/log(10));
    //步长规范到 0-1
    float newCorStep = corStep/pow(10,temp);

    //定步长
    float standard_step;

    int i=0;
    for(;i<6;i++)
    {
        if(newCorStep<=c_scale_standard_step[i]){
            standard_step = c_scale_standard_step[i];
            break;
        }
    }
    float finalStep = standard_step*pow(10,temp);

    //修正
    float cormin = finalStep*ceil(min/finalStep);
    float cormax = finalStep*floor(max/finalStep);
    int   corNum = qRound((cormax - cormin)/finalStep) +1;

    unit.corMin  = cormin;
    unit.corNum  = corNum;
    unit.corStep = finalStep;

    return unit;
}

