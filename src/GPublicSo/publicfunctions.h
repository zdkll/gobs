#ifndef PUBLICFUNCTIONS_H
#define PUBLICFUNCTIONS_H

#include <QtCore>

class PublicFunctions
{
public:
    PublicFunctions();
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

//计算根据数据文件名计算时间,返回QDateTime
//字符串格式：2017-12-31 12:36:23.234
QString dataFile2TimeStr(QString fnamein, double TC);

extern QDateTime gFromDateTime;
extern QDateTime gToDateTime;
extern double    m_TCValue;

void  setDownloadPeriod(QDateTime fromTime,QDateTime toTime);
void  setTcValue(const double &value);
QDateTime dataFile2DateTime(QString fnamein, double TC);

//判断某个文件名是否在时间段内
bool isFileInPeriod(const QString filename);

int swap3(unsigned char* P);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // PUBLICFUNCTIONS_H
