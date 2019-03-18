#include "axis.h"

#include <QPainter>
#include <QDebug>

#include "graphfunctions.h"

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
    //pt->drawRect(QRect(0,0,geometry().width()-1,geometry().height()-1));

    if(this->alignment() == Qt::AlignBottom)
        drawBottomAxis(pt);
    else if(this->alignment() == Qt::AlignLeft)
        drawLeftAxis(pt);
    else if(this->alignment() == Qt::AlignTop)
        drawTopAxis(pt);
    else if(this->alignment() == Qt::AlignRight)
        drawBottomAxis(pt);
}

void ValueAxis::drawBottomAxis(QPainter *pt)
{
    //转化为局部坐标
    QRect graphRect = layerInfo()->graphRect;
    int dx =  graphRect.left() - this->geometry().left();//dx 一般大于0

    float phyWidth = m_maxVal - m_minVal;

    //计算刻度
    CScaleUnit cscaleUnit = GraphFunctions::makeContinuousUnit(m_minVal,m_maxVal,graphRect.width(),this->minSpacing());
   // qDebug()<<cscaleUnit.corMin<<cscaleUnit.corStep<<cscaleUnit.corNum;

    float value;
    int x;
    QFontMetrics fontMetrics(pt->font());
    QString text;
    for(int i=0;i<cscaleUnit.corNum;i++){
        value = (cscaleUnit.corMin +  i* cscaleUnit.corStep);
        x = dx+ std::roundf((value-m_minVal)/phyWidth*graphRect.width());

        pt->drawLine(x,1, x,6);
        text = QString::number(value,'g',4);
        pt->drawText(x- fontMetrics.width(text)/2,fontMetrics.height()+6,text);
    }
}

void ValueAxis::drawLeftAxis(QPainter *pt)
{
    //转化为局部坐标
    QRect graphRect = layerInfo()->graphRect;
    int dy =  this->geometry().bottom() - graphRect.bottom() ;//dy 一般大于0

    float phyWidth = m_maxVal - m_minVal;

    //计算刻度
    CScaleUnit cscaleUnit = GraphFunctions::makeContinuousUnit(m_minVal,m_maxVal,graphRect.width(),this->minSpacing());

    float value;
    int x = this->geometry().width();
    int y0 = this->geometry().height()-dy;
    float y;
    QFontMetrics fontMetrics(pt->font());
    QString text;
    for(int i=0;i<cscaleUnit.corNum;i++){
        value = (cscaleUnit.corMin +  i* cscaleUnit.corStep);
        y =  y0 - std::roundf((value-m_minVal)/phyWidth*graphRect.height());

        pt->drawLine(x,y, x-5,y);

        text = QString::number(value,'g',4);
        pt->drawText(x- fontMetrics.width(text)-5,y+fontMetrics.height()/2,text);
    }
}

void ValueAxis::drawTopAxis(QPainter *pt)
{

}

void ValueAxis::drawRightAxis(QPainter *pt)
{

}
