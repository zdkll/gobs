#include "chartdrawer.h"

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>

ChartDrawer::ChartDrawer(QObject *parent)
    :GraphLayer(parent ),m_wget(0)
    ,m_layerInfo(new LayerInfo(this))
{

}

void ChartDrawer::addAxis(Qt::Alignment alignment, AbstractAxis *axis)
{
    //删除
    auto it = m_alignAxisMp.find(alignment);
    if(it != m_alignAxisMp.end()){
        if(axis != it.value())
            delete it.value();
        m_alignAxisMp.erase(it);
        m_axisAlignMp.remove(axis);
    }
    axis->setAlignment(alignment);
    m_alignAxisMp.insert(alignment,axis);
    m_axisAlignMp.insert(axis,alignment);

    m_allLayers.append(axis);

    axis->setLayerInfo(m_layerInfo);

    axis->setParent(this);
}

AbstractAxis *ChartDrawer::axis(Qt::Alignment alignment) const
{
    auto it = m_alignAxisMp.find(alignment);
    if(it != m_alignAxisMp.end())
        return  it.value();

    return Q_NULLPTR;
}

void ChartDrawer::addGraphLayer(GraphLayer *graphLayer)
{
    m_allLayers.append(graphLayer);

    graphLayer->setGeometry(customLayerGeometry(graphLayer));

    //设置公共图层信息
    graphLayer->setLayerInfo(m_layerInfo);
}

void ChartDrawer::setContentMargins(const QMargins &margin)
{
    m_contentMargins = margin;

    //更新每个子图层geometry
    updateGeometry();
}

void ChartDrawer::setAixsSpaces(const AxisSpaces &spaces)
{
    m_axisSpaces = spaces;

    //更新每个子图层geometry
    updateGeometry();
}

QRect ChartDrawer::customLayerGeometry(GraphLayer *)
{
    //默认返回中央绘图区域
    return graphRect();
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
    GraphLayer::setGeometry(QRect(QPoint(0,0),size));

    //更新每个子图层的geometry
    updateGeometry();

    return false;
}

void ChartDrawer::paintOnWidget()
{
    //绘制图形
    QPainter painter(m_wget);
    //重新绘制总的画布
    this->paint(&painter);
    foreach (GraphLayer *layer, m_allLayers)
        layer->paint(&painter);
    painter.end();
}

bool ChartDrawer::msPress(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_allLayers) {
        if(layer->msPress(e))
            return true;
    }
    return false;
}

bool ChartDrawer::msMove(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_allLayers) {
        if(layer->msMove(e))
            return true;
    }
    return false;
}

bool ChartDrawer::msRelease(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_allLayers) {
        if(layer->msRelease(e))
            return true;
    }
    return false;
}

bool ChartDrawer::msDblClick(QMouseEvent *e)
{
    foreach (GraphLayer *layer, m_allLayers) {
        if(layer->msDblClick(e))
            return true;
    }
    return false;
}


void ChartDrawer::setWidget(QWidget *wg)
{
    m_wget = wg;
    m_wget->installEventFilter(this);
}

void ChartDrawer::updateGeometry()
{
    QVector<GraphLayer* > graphLayers = m_allLayers;
    QRect rect =this->geometry().adjusted(m_contentMargins.left(),m_contentMargins.top(),-m_contentMargins.right(),-m_contentMargins.bottom());

    //先统计坐标轴
    auto it = m_alignAxisMp.begin();
    while(it != m_alignAxisMp.end()){
        graphLayers.removeOne(it.value());
        Qt::Alignment alignMent = it.key();
        switch (alignMent) {
        //左侧坐标轴
        case Qt::AlignLeft:
        {
            it.value()->setGeometry(QRect(rect.left(),rect.top(),m_axisSpaces.left(),rect.height()));
        }
            break;
            //顶部坐标轴
        case Qt::AlignTop:
        {
            it.value()->setGeometry(QRect(rect.left(),rect.top(),rect.width(),m_axisSpaces.top()));
        }
            break;
            //右侧坐标轴
        case Qt::AlignRight:
        {
            it.value()->setGeometry(QRect(rect.right()-m_axisSpaces.right(),rect.top(),m_axisSpaces.right(),rect.height()));
        }
            break;
            //底部坐标轴
        case Qt::AlignBottom:
        {
            it.value()->setGeometry(QRect(rect.left(),rect.bottom()-m_axisSpaces.bottom(),rect.width(),m_axisSpaces.bottom()));
        }
            break;
        default:
            break;
        }
        it++;
    }

    m_layerInfo->graphRect = rect.adjusted(m_axisSpaces.left(),m_axisSpaces.top(),-m_axisSpaces.right(),-m_axisSpaces.bottom());

    foreach(GraphLayer *layer,graphLayers){
        return  layer->setGeometry(customLayerGeometry(layer));//普通图层
    }
}


