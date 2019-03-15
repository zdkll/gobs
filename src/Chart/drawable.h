﻿#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QObject>
#include <QRect>
#include <QPixmap>

class QPainter;
class Drawable
{
public:
    enum ePaintState //绘制状态
    {
        PS_NoPaint = 0,
        PS_DataChanged  = 1,
        PS_Resize   = 2
    };
    Drawable();

    void dataChanged(); //数据改变

    void setGeometry(const QRect& rect);//绘图区域改变
    inline QRect geometry()const {return m_rect;}

    void paint(QPainter *pt);//绘制函数

    ePaintState paintState() const{return m_paintState;}

protected:
    virtual void calData(){}//计算数据
    virtual void rePaint(QPainter* /*pt*/){}//重新绘制(数据或者绘图范围改变都要重新绘制)

private:
    QRect          m_rect;
    ePaintState m_paintState;
};

#endif // DRAWABLE_H
