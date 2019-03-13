#include "drawable.h"

#include  <QRect>
#include <QPixmap>

Drawable::Drawable()
    :m_paintState(PS_NoPaint)
{

}

void Drawable::dataChanged()
{
    m_paintState =  PS_DataChanged;
}

void Drawable::resize(const QRect &rect)
{
    m_rect = rect;
    m_paintState =  PS_Resize;
}

void Drawable::paint(QPainter *pt)
{
    if(m_paintState ==  PS_DataChanged)
        calData();
    if(m_paintState != PS_NoPaint){
        rePaint(pt); //绘制
        m_paintState = PS_NoPaint;
    }
}


