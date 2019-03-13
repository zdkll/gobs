#ifndef CHART_H
#define CHART_H

#include "chart_global.h"

#include <QObject>

#include "chartdrawer.h"

#include "abstractaxis.h"

//自己设计的绘图模块，可以适用于各种图像绘制的框架
class CHARTSHARED_EXPORT Chart : public QWidget
{
    Q_OBJECT
public:
    enum AxesFlag
    {
        XTopAxes
    };
    Chart(QWidget *parent = 0);

    //设置绘图控件
    ChartDrawer *drawer(){return m_drawer;}

private:

private:
    ChartDrawer  *m_drawer;
};

#endif // CHART_H
