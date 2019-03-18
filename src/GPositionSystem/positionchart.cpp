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

    QPoint  beginPt = graphRect().bottomLeft();
    int height = graphRect().height();
    int width  = graphRect().width();

    //绘制GPS轨迹曲线
    pt->save();
    //移动到图像左下角
    pt->translate(graphRect().bottomLeft());

    float x,y;
    float phyWidth = m_dataScope.maxX - m_dataScope.minX;
    float phyHeight = m_dataScope.maxY - m_dataScope.minY;

    QScopedArrayPointer<QPoint> pts(new QPoint[m_gpsCords.size()]);
    for(int i=0;i<m_gpsCords.size();i++){
        x =beginPt.x()+ (m_gpsCords[i].x- m_dataScope.minX)/phyWidth*width;
        y =beginPt.y()- (m_gpsCords[i].y- m_dataScope.minY)/phyHeight*height;
        pts[i] = QPoint(x,y);

        //描点
        pt->drawEllipse(pts[i],2,2);
    }

    //绘制Gobs分布点
    pt->drawPoints(pts.data(),m_gpsCords.size());

    //绘制轨迹
    pt->drawPolyline(pts.data(),m_gpsCords.size());

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

    //往外延伸1/10范围
    float dx = (m_dataScope.maxX - m_dataScope.minX)*0.05;
    float dy = (m_dataScope.maxY - m_dataScope.minY)*0.05;
    m_dataScope.maxX += dx;
    m_dataScope.minX  -= dx;

    m_dataScope.minY  -=dy;
    m_dataScope.maxY +=dy;

    //统计GOBS
    if(m_gobsCords.size()<1){
        m_dataScope.minZ =m_dataScope.maxZ = 0;
        //往外延伸1/10范围
        expandDataScope(&m_dataScope);
        return;
    }

    m_dataScope.minZ =m_dataScope.maxZ = m_gobsCords[0].z;

    for(int i=0;i<m_gobsCords.size();i++){
        m_dataScope.minX = std::min(m_dataScope.minX,float(m_gobsCords[i].x));
        m_dataScope.maxX= std::max(m_dataScope.maxX,float(m_gobsCords[i].x));

        m_dataScope.minY = std::min(m_dataScope.minY,float(m_gobsCords[i].y));
        m_dataScope.maxY= std::max(m_dataScope.maxY,float(m_gobsCords[i].y));

        m_dataScope.minZ = std::min(m_dataScope.minZ,float(m_gobsCords[i].z));
        m_dataScope.maxZ= std::max(m_dataScope.maxZ,float(m_gobsCords[i].z));
    }

    //往外延伸1/10范围
    expandDataScope(&m_dataScope);
}

void PositionChartDrawer::expandDataScope(DataScope *datascope)
{
    float dx = (datascope->maxX - datascope->minX)*0.05;
    float dy = (datascope->maxY - datascope->minY)*0.05;
    datascope->maxX += dx;
    datascope->minX  -= dx;

    datascope->minY  -=dy;
    datascope->maxY +=dy;
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


