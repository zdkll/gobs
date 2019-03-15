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


//图层，最基础的绘图元素,内建数据管理模型
class  CHARTSHARED_EXPORT GraphLayer : public BaseLayer
{
public:
    GraphLayer(QObject *parent = 0);

};

#endif // GRAPHLAYER_H
