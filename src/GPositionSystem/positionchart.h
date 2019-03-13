#ifndef POSITIONCHART_H
#define POSITIONCHART_H

#include  "chart.h"
#include "gpspublic.h"

//定位系统图像控件
class PositionChart : public Chart
{
public:
    PositionChart(QWidget *parent = 0);

    //设置数据
    void setData(const QVector<GpsCoord>& cord1s,const QVector<GobsCoord>& cord2s);

    void addGpsPoint(const GpsCoord& cord);
    void addGobsPoint(const GobsCoord &cord);

    void setGpsPoint(int idx,const GpsCoord& cord);
    void setGobsPoint(int idx,const GobsCoord &cord);

private:
    void initChart();//初始化图层

    QVector<GpsCoord>   m_gpsCords;
    QVector<GobsCoord> m_gobsCords;
};



//以下为自定义图层，继承自GraphLayer 图层




#endif // POSITIONCHART_H
