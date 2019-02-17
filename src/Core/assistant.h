#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QProcess>
#include "core.h"

class Assistant
{
public:
    Assistant();
    static bool showAssistant(const QString &moduleName);
};

#endif // ASSISTANT_H
