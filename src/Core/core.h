#ifndef CORE_H
#define CORE_H

//文档文件
#define Documents_File ".gobs"

#include <QtCore>

#include "macro.h"
#include "core_global.h"

namespace Core {
//用户根目录
CORESHARED_EXPORT QString  homePath();

//文档目录
CORESHARED_EXPORT const QString docmentsPath();

//创建文档目录
CORESHARED_EXPORT bool createDocDir();

//程序文件目录
CORESHARED_EXPORT const QString rootPath();

//删除文件夹
CORESHARED_EXPORT bool deleteDirectory(const QString &path);

CORESHARED_EXPORT QJsonObject readJsonDoc(const QString &fileName);

CORESHARED_EXPORT bool saveJsonObject(const QJsonObject &obj,const QString &fileName);

CORESHARED_EXPORT extern QString currentDirPath;
}


#endif // CORE_H
