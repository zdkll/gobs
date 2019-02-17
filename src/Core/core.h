#ifndef CORE_H
#define CORE_H

//文档文件
#define Documents_File ".gobs"

#include <QtCore>

#include "macro.h"

namespace Core {

//用户根目录
QString  homePath();

//文档目录
const QString docmentsPath();

//创建文档目录
bool createDocDir();

//程序文件目录
const QString rootPath();

//删除文件夹
bool deleteDirectory(const QString &path);

QJsonObject readJsonDoc(const QString &fileName);

bool saveJsonObject(const QJsonObject &obj,const QString &fileName);

extern QString currentDirPath;
}


#endif // CORE_H
