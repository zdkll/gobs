#ifndef ABSTRACTAXIS_H
#define ABSTRACTAXIS_H

#include <QObject>

#include "graphlayer.h"

class AbstractAxis : public GraphLayer
{
public:
    AbstractAxis(QObject *parent = 0);

};

#endif // ABSTRACTAXIS_H
