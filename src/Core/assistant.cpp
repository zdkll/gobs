#include "assistant.h"
#include <QDesktopServices>

Assistant::Assistant()
{
}

bool Assistant::showAssistant(const QString &moduleName)
{
    QString m_EnvPath = Core::rootPath();
    QString  urlFileName = m_EnvPath+QString("/doc/GOBS_%1_Help.htm").arg(moduleName);

    return QDesktopServices::openUrl(QUrl::fromLocalFile(urlFileName));
}

