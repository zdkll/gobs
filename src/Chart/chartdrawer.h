#ifndef CHARTDRAWER_H
#define CHARTDRAWER_H

#include <QObject>
#include <QWidget>
#include <QMap>

#include "graphlayer.h"
#include "abstractaxis.h"
#include "chart_global.h"

typedef QMargins AxisSpaces;

class Chart;
class GraphLayer;
class AbstractAxis;
//主图层，图层管理类,管理所有自图层,负责绘制主图形区域
class CHARTSHARED_EXPORT ChartDrawer : public GraphLayer
{
public:
    ChartDrawer(QObject *parent = 0);

    //刻度
    void addAxis(Qt::Alignment alignment,AbstractAxis *axis);
    AbstractAxis *axis(Qt::Alignment alignment) const;

    //添加普通图层，绘制范围默认在图像区域
    void addGraphLayer(GraphLayer* graphLayer);

    //设置整个图像的margins
    void   setContentMargins(const QMargins& margin);
    inline QMargins contentMargins() const {return m_contentMargins;}

    //设置坐标轴的空间
    void   setAixsSpaces(const AxisSpaces& spaces);
    inline AxisSpaces axisSpaces()const {return m_axisSpaces;}

    inline QRect graphRect()const {return m_layerInfo->graphRect;}

    inline void draw(){m_wget->update();}

protected:
    bool eventFilter(QObject *watched, QEvent *event);//过滤处理窗口事件

    virtual bool wgResize(const QSize& size);//依附的窗口大小变化
    void paintOnWidget();//绘制窗口

    //窗口事件
    virtual bool msPress(QMouseEvent *e);
    virtual bool msMove(QMouseEvent *e);
    virtual bool msRelease(QMouseEvent *e);
    virtual bool msDblClick(QMouseEvent *e);

    //返回图层绘图区域/自定义普通图层
    virtual QRect customLayerGeometry(GraphLayer* layer);

private:
    void setWidget(QWidget *wg);//设置所属的窗口
    //更新所有子图层的绘图区域
    void updateGeometry();

    QWidget     *m_wget;    //绘制窗口
    LayerInfo    *m_layerInfo; //图层信息

    QMargins      m_contentMargins;
    AxisSpaces   m_axisSpaces;//坐标轴区域，可变的，坐标轴实际占位置为空

    QMap<AbstractAxis *,Qt::Alignment>  m_axisAlignMp; //坐标轴-方位
    QMap<Qt::Alignment,AbstractAxis *>  m_alignAxisMp;//方位-坐标轴
    QVector<GraphLayer* > m_allLayers;//所有图层

    friend class Chart;
};

#endif // CHARTDRAWER_H
