#ifndef DRAWABLE_H
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
    virtual void resize(const QRect& rect);//绘图区域改变
    virtual void paint();//绘制函数

    QPixmap pixmap()const {return *m_pix;}

protected:
    virtual void calData(){}//计算数据
    virtual void rePaint(){}//重新绘制(数据或者绘图范围改变都要重新绘制)

private:
    QRect          m_rect;
    QPixmap    *m_pix;
    ePaintState m_paintState;
};

#endif // DRAWABLE_H
