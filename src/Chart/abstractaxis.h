#ifndef ABSTRACTAXIS_H
#define ABSTRACTAXIS_H

#include <QObject>

#include "chart_global.h"
#include "graphlayer.h"

class CHARTSHARED_EXPORT AbstractAxis : public GraphLayer
{
public:
    AbstractAxis(QObject *parent = 0);

    inline Qt::Alignment alignment() const{return m_alignment;}
    inline void setAlignment(Qt::Alignment alignment){m_alignment = alignment;}

private:
    Qt::Alignment m_alignment;
};

#endif // ABSTRACTAXIS_H
