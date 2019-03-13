#include "chartdrawer.h"

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>

ChartDrawer::ChartDrawer(QWidget *wg, QObject *parent)
    :BaseLayer(parent ),m_wget(wg)
    ,m_pix(new QPixmap())
{
    m_wget->installEventFilter(this);

}


bool ChartDrawer::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_wget)
    {
        if(event->type() == QEvent::Resize){
            QResizeEvent *e = static_cast<QResizeEvent *>(event);
            return  wgResize(e->size());
        }else if(event->type() == QEvent::Paint){
            paintOnWidget();
        }else if(event->type() == QEvent::MouseButtonPress
                 || event->type() == QEvent::MouseMove
                 || event->type() == QEvent::MouseButtonRelease
                 || event->type() == QEvent::MouseButtonDblClick){
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if(event->type() == QEvent::MouseButtonPress)
                return msPress(e);
            else if(event->type() == QEvent::MouseMove)
                return msMove(e);
            else if(event->type() == QEvent::MouseButtonRelease)
                return msRelease(e);
            else if(event->type() == QEvent::MouseButtonDblClick)
                return msDblClick(e);
        }
    }

    return QObject::eventFilter(watched,event);
}

bool ChartDrawer::wgResize(const QSize &size)
{
    resize(QRect(QPoint(0,0),size));

    return false;
}

void ChartDrawer::paintOnWidget()
{
    if(paintState() == PS_Resize){
        *m_pix = QPixmap(this->rect().size());
    }
    //重新绘制总的画布
    if(paintState() != PS_NoPaint){
        QPainter pt(m_pix);
        pt.fillRect(this->rect(),Qt::blue);
        foreach (GraphLayer *layer, m_graphLayers)
            layer->paint(&pt);
    }
    //绘制图形
    QPainter painter(m_wget);
    painter.drawPixmap(0,0,*m_pix);
    painter.end();
}

bool ChartDrawer::msPress(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_graphLayers) {
        if(layer->msPress(e))
            return true;
    }
    return false;
}

bool ChartDrawer::msMove(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_graphLayers) {
        if(layer->msMove(e))
            return true;
    }
    return false;
}

bool ChartDrawer::msRelease(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_graphLayers) {
        if(layer->msRelease(e))
            return true;
    }
    return false;
}

bool ChartDrawer::msDblClick(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_graphLayers) {
        if(layer->msDblClick(e))
            return true;
    }
    return false;
}
