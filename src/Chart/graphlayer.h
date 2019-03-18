#ifndef GRAPHLAYER_H
#define GRAPHLAYER_H

#include <QObject>

#include "drawable.h"
#include "chart_global.h"



class QMouseEvent;
class BaseLayer : public QObject,public Drawable
{
public:
    BaseLayer(QObject *parent = 0):QObject(parent){}

    virtual bool msPress(QMouseEvent *){return false;}
    virtual bool msMove(QMouseEvent *){return false;}
    virtual bool msRelease(QMouseEvent *){return false;}
    virtual bool msDblClick(QMouseEvent *){return false;}
};

class GraphLayer;

struct LayerInfo
{
    LayerInfo(GraphLayer* mlayer):mLayer(mlayer){}

    GraphLayer  *mLayer;//主图层
    QRect           graphRect;
};

//图层，最基础的绘图元素,内建数据管理模型
class  CHARTSHARED_EXPORT GraphLayer : public BaseLayer
{
public:
    GraphLayer(QObject *parent = 0);

    void setLayerInfo(LayerInfo *layerInfo){m_layerInfo = layerInfo;}
    LayerInfo *layerInfo(){return m_layerInfo;}
    virtual void draw();

private:
    LayerInfo *m_layerInfo;
};

#endif // GRAPHLAYER_H
