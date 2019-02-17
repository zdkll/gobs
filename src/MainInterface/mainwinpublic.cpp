#include "mainwinpublic.h"

MainWinPublic::MainWinPublic()
{

}

const QString MainWinPublic::rootPath()
{
    QDir bin_dir(QApplication::applicationDirPath());
    bin_dir.cdUp();
    return  bin_dir.absolutePath();
}


const QString MainWinPublic::workPath()
{
    QString dir = Core::homePath();
    QString workPath  = Core::docmentsPath()+Current_WorkPath;
    QFile file(workPath);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString dir0 = in.readAll();
        if(!dir0.isEmpty())
            dir = dir0;
        file.close();
    }
    return dir;
}

void MainWinPublic::saveWorkPath(const QString &Path)
{
    QString workPath  = Core::docmentsPath()+Current_WorkPath;
    QFile file(workPath);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out<<Path;
        file.close();
    }
}
