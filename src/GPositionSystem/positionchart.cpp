#include "positionchart.h"

#include <assert.h>
#include <QPainter>

#include <QDebug>

PositionChart::PositionChart(QWidget *parent)
    :Chart(parent)
{
    initChart();
}

void PositionChart::setData(const QVector<GpsCoord> &cord1s, const QVector<GobsCoord> &cord2s)
{
    m_posChartDrawer->setData(cord1s,cord2s);

    DataScope dataScope = m_posChartDrawer->dataScope();

    m_axisX->setMin(dataScope.minX);
    m_axisX->setMax(dataScope.maxX);

    m_axisY->setMin(dataScope.minY);
    m_axisY->setMax(dataScope.maxY);

    m_posChartDrawer->draw();
}

void PositionChart::addGpsPoint(const GpsCoord &cord)
{
    m_posChartDrawer->addGpsPoint(cord);

    DataScope dataScope = m_posChartDrawer->dataScope();

    m_axisX->setMin(dataScope.minX);
    m_axisX->setMax(dataScope.maxX);

    m_axisY->setMin(dataScope.minY);
    m_axisY->setMax(dataScope.maxY);

    m_posChartDrawer->draw();
}

void PositionChart::addGobsPoint(const GobsCoord &cord)
{
    m_posChartDrawer->addGobsPoint(cord);

    DataScope dataScope = m_posChartDrawer->dataScope();

    m_axisX->setMin(dataScope.minX);
    m_axisX->setMax(dataScope.maxX);

    m_axisY->setMin(dataScope.minY);
    m_axisY->setMax(dataScope.maxY);

    m_posChartDrawer->draw();
}

void PositionChart::setGpsPoint(int idx, const GpsCoord &cord)
{
    m_posChartDrawer->setGpsPoint(idx,cord);
}

void PositionChart::setGobsPoint(int idx, const GobsCoord &cord)
{
    m_posChartDrawer->setGobsPoint(idx,cord);
}



void PositionChart::initChart()
{
    //设置ChartDrawer
    m_posChartDrawer = new PositionChartDrawer(this);
    this->setCharDrawer(m_posChartDrawer);

    //向ChartDrawer 设置坐标轴
    m_axisX = new ValueAxis(this);
    m_axisY = new ValueAxis(this);

    m_posChartDrawer->addAxis(Qt::AlignBottom,m_axisX);
    m_posChartDrawer->addAxis(Qt::AlignLeft,m_axisY);

    //设置margin 和 坐标轴占的空间
    m_posChartDrawer->setContentMargins(QMargins(5,5,5,5));
    m_posChartDrawer->setAixsSpaces(AxisSpaces(40,20,40,20));
}



///PositionChartDrawer----
PositionChartDrawer::PositionChartDrawer(QObject *parent)
    :ChartDrawer(parent)
{
    this->setBackgroundColor(Qt::white);

}

void PositionChartDrawer::setData(const QVector<GpsCoord> &cord1s, const QVector<GobsCoord> &cord2s)
{
    m_gpsCords   = cord1s;
    m_gobsCords = cord2s;

    //计算数据范围
    calDataScope();

    dataChanged();
}

void PositionChartDrawer::addGpsPoint(const GpsCoord &cord)
{
    m_gpsCords.append(cord);

    //计算数据范围
    calDataScope();
    dataChanged();
}

void PositionChartDrawer::addGobsPoint(const GobsCoord &cord)
{
    m_gobsCords.append(cord);

    //计算数据范围
    calDataScope();
    dataChanged();
}

void PositionChartDrawer::setGpsPoint(int idx, const GpsCoord &cord)
{
    assert(idx>=0 && idx<m_gpsCords.size());
    m_gpsCords[idx] = cord;

    //计算数据范围
    calDataScope();
    dataChanged();
}

void PositionChartDrawer::setGobsPoint(int idx, const GobsCoord &cord)
{
    assert(idx>=0 && idx<m_gobsCords.size());
    m_gobsCords[idx] = cord;

    //计算数据范围
    calDataScope();
    dataChanged();
}


void PositionChartDrawer::calData()
{

}

void PositionChartDrawer::rePaint(QPainter *pt)
{
    //绘制边框
    pt->setPen(Qt::black);
    pt->drawRect(graphRect());

    //绘制GPS轨迹曲线
    pt->save();



    //绘制Gobs分布点

    pt->restore();
}

void PositionChartDrawer::calDataScope()
{
    if(m_gpsCords.size()<1){
        m_dataScope.minX = m_dataScope.maxX = 0;
        m_dataScope.minY = m_dataScope.maxY = 0;
        return;
    }

    m_dataScope.minX = m_gpsCords[0].x;
    m_dataScope.maxX= m_gpsCords[0].x;

    m_dataScope.minY = m_gpsCords[0].y;
    m_dataScope.maxY= m_gpsCords[0].y;

    //统计GPS
    for(int i=1;i<m_gpsCords.size();i++){
        m_dataScope.minX = std::min(m_dataScope.minX,float(m_gpsCords[i].x));
        m_dataScope.maxX= std::max(m_dataScope.maxX,float(m_gpsCords[i].x));

        m_dataScope.minY = std::min(m_dataScope.minY,float(m_gpsCords[i].y));
        m_dataScope.maxY= std::max(m_dataScope.maxY,float(m_gpsCords[i].y));
    }

    if(m_gobsCords.size()<1){
        m_dataScope.minZ =m_dataScope.maxZ = 0;
        return;
    }

    m_dataScope.minZ =m_dataScope.maxZ = m_gobsCords[0].z;
    //统计GOBS
    for(int i=0;i<m_gobsCords.size();i++){
        m_dataScope.minX = std::min(m_dataScope.minX,float(m_gobsCords[i].x));
        m_dataScope.maxX= std::max(m_dataScope.maxX,float(m_gobsCords[i].x));

        m_dataScope.minY = std::min(m_dataScope.minY,float(m_gobsCords[i].y));
        m_dataScope.maxY= std::max(m_dataScope.maxY,float(m_gobsCords[i].y));

        m_dataScope.minZ = std::min(m_dataScope.minZ,float(m_gobsCords[i].z));
        m_dataScope.maxZ= std::max(m_dataScope.maxZ,float(m_gobsCords[i].z));
    }
}


///ValueAxis----
ValueAxis::ValueAxis(QObject *parent)
    :AbstractAxis(parent)
{

}

void ValueAxis::calData()
{

}

void ValueAxis::rePaint(QPainter *pt)
{
    pt->setPen(Qt::blue);
    pt->drawRect(QRect(0,0,geometry().width()-1,geometry().height()-1));
    pt->drawText(QRect(0,0,geometry().width()-1,geometry().height()-1),Qt::AlignCenter,QString::number(this->alignment()));
}


