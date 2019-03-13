#include "chartdrawer.h"

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>

ChartDrawer::ChartDrawer(QWidget *wg, QObject *parent)
    :BaseLayer(parent ),m_wget(wg)
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
             paint();
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

    return false;
}

void ChartDrawer::paint()
{

}

bool ChartDrawer::msPress(QMouseEvent *e)
{

    return false;
}

bool ChartDrawer::msMove(QMouseEvent *e)
{
  return false;
}

bool ChartDrawer::msRelease(QMouseEvent *e)
{
 return false;
}

bool ChartDrawer::msDblClick(QMouseEvent *e)
{
return false;
}
