#include "drawable.h"

#include  <QRect>
#include <QPixmap>

Drawable::Drawable()
    :m_pix(new QPixmap())
    ,m_paintState(PS_NoPaint)
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

void Drawable::paint()
{
    if(m_paintState ==  PS_DataChanged)
        calData();
    else if(m_paintState ==  PS_Resize)
        *m_pix = QPixmap(m_rect.size());

    if(m_paintState != PS_NoPaint){
        rePaint(); //绘制
        m_paintState = PS_NoPaint;
    }
}


