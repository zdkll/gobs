﻿#ifndef CHARTDRAWER_H
#define CHARTDRAWER_H

#include <QObject>
#include <QWidget>

#include "graphlayer.h"

//图层管理类
class ChartDrawer : public BaseLayer
{
public:
    ChartDrawer(QWidget *wg,QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);//过滤处理窗口事件

    bool wgResize(const QSize& size);//大小变化
    void paintOnWidget();

    bool msPress(QMouseEvent *e);
    bool msMove(QMouseEvent *e);
    bool msRelease(QMouseEvent *e);
    bool msDblClick(QMouseEvent *e);

private:
    QWidget *m_wget;    //绘制窗口
    QPixmap *m_pix;
    QVector<GraphLayer* > m_graphLayers;//管理图层
};

#endif // CHARTDRAWER_H
