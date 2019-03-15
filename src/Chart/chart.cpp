#include "chart.h"

#include "chartdrawer.h"

Chart::Chart(QWidget *parent)
    :QWidget(parent)
    ,m_drawer(0)
{

}

Chart::Chart(ChartDrawer *drawer, QWidget *parent)
    :QWidget(parent)
    ,m_drawer(drawer)
{
  m_drawer->setWidget(this);
}

void Chart::setCharDrawer(ChartDrawer *drawer)
{
    m_drawer = drawer;
    m_drawer->setWidget(this);

    //更新m_drawer 区域
    if(this->isVisible())
        m_drawer->wgResize(this->size());
}
