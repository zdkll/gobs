#include "graphlayer.h"

GraphLayer::GraphLayer(QObject *parent)
    :BaseLayer(parent)
    ,m_layerInfo(0)
{

}

void GraphLayer::draw()
{
    if(m_layerInfo)
        m_layerInfo->mLayer->draw();
}
