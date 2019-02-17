#include "lmgrpublic.h"

#include <QFile>
#include <QTextStream>

LmgrPublic::LmgrPublic()
{

}

void LmgrPublic::calAreaScope(AreaDataInfo *areaData,QRectF &rect)
{
    StaLine  *staLines;
    Station  *stations;
    int lineNum;
    int staionNum;

    //遍历接收点-------------------
    StationInfo *recvStatonInfo = areaData->recvStationInfo;
    staLines = recvStatonInfo->staLines;
    lineNum  = recvStatonInfo->lineNum;
    float minX,maxX,minY,maxY;
    //先保存第一条线第一个点的坐标
    minX=maxX=staLines[0].stations[0].x;
    minY=maxY=staLines[0].stations[0].y;
    for(int i= 0;i<lineNum;i++)
    {
        stations = staLines[i].stations;
        staionNum = staLines[i].staNum;
        for(int j = 0; j<staionNum;j++){
            minX = (minX>stations[j].x)?stations[j].x:minX;
            maxX = (maxX<stations[j].x)?stations[j].x:maxX;

            minY = (minY>stations[j].y)?stations[j].y:minY;
            maxY = (maxY<stations[j].y)?stations[j].y:maxY;
        }
    }

    //炮点
    StationInfo *shotStationInfo = areaData->shotStationInfo;
    staLines = shotStationInfo->staLines;
    lineNum  = shotStationInfo->lineNum;
    for(int i= 0;i<lineNum;i++)
    {
        stations = staLines[i].stations;
        staionNum = staLines[i].staNum;
        for(int j = 0; j<staionNum;j++){
            minX = (minX>stations[j].x)?stations[j].x:minX;
            maxX = (maxX<stations[j].x)?stations[j].x:maxX;

            minY = (minY>stations[j].y)?stations[j].y:minY;
            maxY = (maxY<stations[j].y)?stations[j].y:maxY;
        }
    }

    //返回矩形区域,注意左下角为坐标原点------
    rect.setTopLeft(QPointF(minX,minY));
    rect.setBottomRight(QPointF(maxX,maxY));

}





