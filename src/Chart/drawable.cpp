#include "drawable.h"

#include  <QRect>
#include <QPainter>

Drawable::Drawable()
    :m_paintState(PS_NoPaint)
    ,m_backgroundColor(Qt::transparent)
{

}

void Drawable::dataChanged()
{
    m_paintState =  PS_DataChanged;
}

void Drawable::setGeometry(const QRect &rect)
{
    m_rect = rect;
    m_paintState =  PS_Resize;
    m_pix = QPixmap(rect.size());
}

void Drawable::paint(QPainter *painter)
{
    if(m_paintState ==  PS_DataChanged)
        calData();
    if(m_paintState != PS_NoPaint){
        m_pix.fill(m_backgroundColor);
        QPainter pt(&m_pix);
        rePaint(&pt); //绘制
        pt.end();
        m_paintState = PS_NoPaint;
    }

    //绘制到主图层
    painter->drawPixmap(this->geometry(),m_pix);
}


