#include "core.h"
#include <QCoreApplication>


QString Core::currentDirPath = QDir::currentPath();

QString Core::homePath()
{
    return QDir::homePath();
}

const QString Core::docmentsPath()
{
    return homePath()+Dir_Separator+Documents_File;
}

bool Core::createDocDir()
{
    QDir gobsDir(docmentsPath());
    if(!gobsDir.exists())
        return gobsDir.mkdir(docmentsPath());
    return true;
}

const QString Core::rootPath()
{
    QDir bin_dir(QCoreApplication::applicationDirPath());
    bin_dir.cdUp();
    return  bin_dir.absolutePath();
}

bool Core::deleteDirectory(const QString &path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
            fi.dir().remove(fi.fileName());
        else
            deleteDirectory(fi.absoluteFilePath());
    }
    return dir.rmpath(dir.absolutePath());
}

QJsonObject Core::readJsonDoc(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return QJsonObject();

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    return doc.object();
}

bool Core::saveJsonObject(const QJsonObject &obj,const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;
    QJsonDocument doc;
    doc.setObject(obj);

    file.write(doc.toJson());
    file.close();
    return true;
}


