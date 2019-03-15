#ifndef CHART_H
#define CHART_H

#include "chart_global.h"

#include <QWidget>

class ChartDrawer;
//自己设计的绘图窗口,使用ChartDrawer绘制在此窗口上
class CHARTSHARED_EXPORT Chart : public QWidget
{
    Q_OBJECT
public:
    Chart(QWidget *parent = 0);
    Chart( ChartDrawer *drawer,QWidget *parent = 0);

    //设置绘图控件
    ChartDrawer *drawer()const {return m_drawer;}
    void setCharDrawer(ChartDrawer  *drawer);

private:
    ChartDrawer  *m_drawer;
};

#endif // CHART_H
