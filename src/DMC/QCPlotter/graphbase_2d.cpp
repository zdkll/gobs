#include "graphbase_2d.h"

#include <assert.h>

//计算纵横某个方向的放大倍数
float ImageBaseFunction::getZoomValue(const int &oriSize,const int &newSize,const float &oriZoomValue){
    if (oriSize == 0)
        throw QString("ERROR:Original map size is 0!");

    return fabs(float(newSize)/float(oriSize)) * oriZoomValue;
}

//根据当前放大倍数，原始取样总数，坐标值（x或y），总长度，算出取样点下标偏移量,注意是偏移量。 f(off) = totalNum * zoom * (X坐标 or Y坐标/rectSize)
//例如，放大倍数为1，总取样点数100个，在长度200的图像轴上的坐标50处取样，取到的数据下标偏移量就是24（从0开始）
int ImageBaseFunction::getCurrentIdxOffset(const float &zoom, const int &totalNum, const int &rectSize, const int &XorY)
{
    assert(rectSize > 0);
    qDebug()<<"total num:"<<totalNum<<" zoomx:"<<zoom<<" xor:"<<XorY;
    return qRound(float(totalNum) *zoom * float(XorY) / float(rectSize));
}

//返回一个合理的波形图step，确保每道数据宽度至少一个像素
int ImageBaseFunction::getGoodStep(int width, int step, int horizNum,const int minPix)
{
    assert(step > 0);
    assert(width > 0);
    assert(horizNum > 0);
    int result,hNum;
    //最低每道保持5个像素
    int time = 1;
    do {
        result = step * time++;
        hNum = ceil(float(horizNum)/float(result));
    }
    while (hNum*minPix > width);

    return result;
}

//从数据中获取每一道的最大值最小值
void ImageBaseFunction::getTraceExtremumFromData(float *data, int sd, int fd, float *valminl, float *valmaxl, float *minGlobal, float *maxGlobal)
{
    //注意，各道集最大值最小值数组空间在外部申请
    assert(data!=NULL);
    assert(fd > 0);
    assert(sd > 0);
    assert(valmaxl != NULL);
    assert(valminl != NULL);

    float valmaxl_tmp = 0.0f,valminl_tmp = 0.0f;

    //以上做法当数据的最小值小于零或最大值不大于零的时候，无法得到其正确的最大值和最小值
    *minGlobal = data[0];
    *maxGlobal = data[0];

    for(int i=0;i<sd;i++)
    {
        valmaxl_tmp = data[i*fd];
        valminl_tmp = data[i*fd];

        for(int j=0;j<fd;j++)
        {
            if(valmaxl_tmp<data[i*fd+j])
                valmaxl_tmp = data[i*fd+j];
            if(valminl_tmp>data[i*fd+j])
                valminl_tmp = data[i*fd+j];
        }
        valmaxl[i] = valmaxl_tmp;
        valminl[i] = valminl_tmp;

        if (*minGlobal > valminl[i])
            *minGlobal = valminl[i];
        if (*maxGlobal < valmaxl[i])
            *maxGlobal = valmaxl[i];
    }
}

