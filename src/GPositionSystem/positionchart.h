#ifndef POSITIONCHART_H
#define POSITIONCHART_H

#include  "chart.h"

//定位系统图像控件
class PositionChart : public Chart
{
public:
    PositionChart(QWidget *parent = 0);

private:
    void initChart();//初始化图层

};



//以下为自定义图层，继承自GraphLayer 图层




#endif // POSITIONCHART_H
