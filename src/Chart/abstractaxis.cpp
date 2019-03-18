#include "abstractaxis.h"

#define Default_Scale_Spacing 20

AbstractAxis::AbstractAxis(QObject *parent)
    :GraphLayer(parent),m_minSpacing(Default_Scale_Spacing)
{

}
