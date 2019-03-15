#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QObject>
#include <QRect>
#include <QPixmap>
#include "chart_global.h"

class QPainter;
class CHARTSHARED_EXPORT  Drawable
{
public:
    enum ePaintState //绘制状态
    {
        PS_NoPaint = 0,
        PS_DataChanged  = 1,
        PS_Resize   = 2
    };
    Drawable();
    virtual ~Drawable(){}

    void dataChanged(); //数据改变

    void setGeometry(const QRect& rect);//绘图区域改变
    inline QRect geometry()const {return m_rect;}

    inline void setBackgroundColor(const QColor& color){m_backgroundColor = color;}
    inline QColor backgroundColor()const{return m_backgroundColor;}

    void paint(QPainter *pt);//绘制函数

    ePaintState paintState() const{return m_paintState;}

    //获取pixmap
    QPixmap pixmap()const {return m_pix;}

protected:
    virtual void calData(){}//计算数据
    virtual void rePaint(QPainter* /*pt*/){}//重新绘制(数据或者绘图范围改变都要重新绘制)

private:
    QRect          m_rect;
    ePaintState  m_paintState;
    QPixmap     m_pix;
    QColor        m_backgroundColor;
};

#endif // DRAWABLE_H
