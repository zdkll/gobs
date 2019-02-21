#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QProcess>
#include "core.h"
#include "core_global.h"

class CORESHARED_EXPORT Assistant
{
public:
    Assistant();
    static bool showAssistant(const QString &moduleName);
};

#endif // ASSISTANT_H
