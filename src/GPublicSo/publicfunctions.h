#ifndef PUBLICFUNCTIONS_H
#define PUBLICFUNCTIONS_H

#include <QtCore>

class PublicFunctions
{
public:
    PublicFunctions();
};


//计算根据数据文件名计算时间,返回QDateTime
//字符串格式：2017-12-31 12:36:23.234
extern "C" QString dataFile2TimeStr(QString fnamein, double TC);

extern QDateTime gFromDateTime;
extern QDateTime gToDateTime;
extern double    m_TCValue;

extern "C" void  setDownloadPeriod(QDateTime fromTime,QDateTime toTime);
extern "C" void  setTcValue(const double &value);
extern "C" QDateTime dataFile2DateTime(QString fnamein, double TC);

//判断某个文件名是否在时间段内
extern "C" bool isFileInPeriod(const QString filename);

extern "C" int swap3(unsigned char* P);

#endif // PUBLICFUNCTIONS_H
