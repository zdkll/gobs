#ifndef AXIS_H
#define AXIS_H

#include "abstractaxis.h"

//X坐标轴-Y坐标轴-连续值-统一处理
class ValueAxis  : public AbstractAxis
{
public:
    ValueAxis(QObject *parent = 0);

    inline void setMin(float minVal){m_minVal = minVal;dataChanged();}
    inline float min() const{return m_minVal;}

    inline void setMax(float maxVal){m_maxVal = maxVal;dataChanged();}
    inline float max() const{return m_maxVal;}

protected:
    virtual void calData();//计算数据
    virtual void rePaint(QPainter* pt);//重新绘制(数据或者绘图范围改变都要重新绘制)

private:
    //绘制底部坐标
    void drawBottomAxis(QPainter* pt);
    void drawLeftAxis(QPainter* pt);
    void drawTopAxis(QPainter* pt);
    void drawRightAxis(QPainter* pt);

private:
    float m_minVal  = 0;
    float m_maxVal = 0;
};

#endif // AXIS_H
