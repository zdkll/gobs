#include "positionchart.h"

#include <assert.h>

PositionChart::PositionChart(QWidget *parent)
    :Chart(parent)
{
    initChart();
}

void PositionChart::setData(const QVector<GpsCoord> &cord1s, const QVector<GobsCoord> &cord2s)
{
    m_gpsCords   = cord1s;
    m_gobsCords = cord2s;
}

void PositionChart::addGpsPoint(const GpsCoord &cord)
{
    m_gpsCords.append(cord);
}

void PositionChart::addGobsPoint(const GobsCoord &cord)
{
    m_gobsCords.append(cord);

}

void PositionChart::setGpsPoint(int idx, const GpsCoord &cord)
{
    assert(idx>=0 && idx<m_gpsCords.size());
    m_gpsCords[idx] = cord;
}

void PositionChart::setGobsPoint(int idx, const GobsCoord &cord)
{
    assert(idx>=0 && idx<m_gobsCords.size());
    m_gobsCords[idx] = cord;
}

void PositionChart::calData()
{

}

void PositionChart::rePaint(QPainter *pt)
{

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
}



///PositionChartDrawer----
PositionChartDrawer::PositionChartDrawer(QObject *parent)
    :ChartDrawer(parent)
{

}



///ValueAxis----
ValueAxis::ValueAxis(QObject *parent)
    :AbstractAxis(parent)
{

}
