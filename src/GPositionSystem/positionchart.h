#ifndef POSITIONCHART_H
#define POSITIONCHART_H

#include "gpspublic.h"
#include  "chart.h"
#include  "chartdrawer.h"

class  PositionChartDrawer;
class  ValueAxis;
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
    void updateAxisScope(const DataScope& dataScope);

    PositionChartDrawer *m_posChartDrawer;
    ValueAxis                   *m_axisX,*m_axisY;
};


//以下为自定义图层，继承自GraphLayer 图层---
//主图层区域
class PositionChartDrawer : public ChartDrawer
{
public:
    PositionChartDrawer(QObject *parent = 0);

    //设置数据
    void setData(const QVector<GpsCoord>& cord1s,const QVector<GobsCoord>& cord2s);

    void addGpsPoint(const GpsCoord& cord);
    void addGobsPoint(const GobsCoord &cord);

    void setGpsPoint(int idx,const GpsCoord& cord);
    void setGobsPoint(int idx,const GobsCoord &cord);

    DataScope dataScope()const {return m_dataScope;}

protected:
    virtual void calData();//计算数据
    virtual void rePaint(QPainter* pt);//重新绘制(数据或者绘图范围改变都要重新绘制)

private:
    void calDataScope();
    void expandDataScope(DataScope *datascope);

    //绘制GPS点
    void drawGpsPoints(QPainter *painter);
    //绘制
    void drawGobsPoints(QPainter *painter);

    DataScope m_dataScope;

    QVector<GpsCoord>   m_gpsCords;
    QVector<GobsCoord> m_gobsCords;
};



#endif // POSITIONCHART_H
