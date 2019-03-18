#ifndef PUBLICFUNCTIONS_H
#define PUBLICFUNCTIONS_H

#include <QtCore>
#include "gpublicso_global.h"

class PublicFunctions
{
public:
    PublicFunctions();
};

//计算根据数据文件名计算时间,返回QDateTime
//字符串格式：2017-12-31 12:36:23.234
QString dataFile2TimeStr(QString fnamein, double TC);

extern QDateTime gFromDateTime;
extern QDateTime gToDateTime;
extern double    m_TCValue;

GPUBLICSOSHARED_EXPORT void  setDownloadPeriod(QDateTime fromTime,QDateTime toTime);
GPUBLICSOSHARED_EXPORT void  setTcValue(const double &value);
GPUBLICSOSHARED_EXPORT QDateTime dataFile2DateTime(QString fnamein, double TC);

//判断某个文件名是否在时间段内
GPUBLICSOSHARED_EXPORT bool isFileInPeriod(const QString filename);

GPUBLICSOSHARED_EXPORT int swap3(unsigned char* P);


#endif // PUBLICFUNCTIONS_H
