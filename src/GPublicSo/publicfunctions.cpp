#include "publicfunctions.h"

#include <assert.h>

PublicFunctions::PublicFunctions()
{

}


QString dataFile2TimeStr(QString fnamein, double TC)
{
    //文件名长度 = 11
    assert(fnamein.length() == 12);

    unsigned int date[8],fnlength;
    short year, month, day,  hour, min, sec,smsec;//jday,
    double msec;
    double pclk;

    int i;
    bool ok;
    QString Datatime;
    // QString syear, smonth, sday, shour, smin, ssec,Datatime;//sjday,

    fnlength=fnamein.length();

    /* get the date and time information for the file name */
    for (i=0;i<8;i++) date[i]=fnamein.mid(i,1).toInt(&ok,16);

    year  = (date[0] << 2 | date[1] >> 2)  & 0x1f;  /* 0x0f=00011111 */
    month = (date[1] << 2 | date[2] >> 2)  & 0x0f;  /* 0x0f=00001111 */
    day   = (date[2] << 3 | date[3] >> 1)  & 0x1f;  /* 0x1f=00011111 */
    hour  = (date[3] << 4 | date[4])       & 0x1f;  /* 0x1f=00011111 */
    min   = (date[5] << 2 | date[6] >> 2)  & 0x3f;  /* 0x3f=00111111 */
    sec   = (date[6] << 4 | date[7])       & 0x3f;  /* 0x3f=00111111 */

    year  = year + 2000;
    if( year == 2000 ){
        year += 16;
    }

    //jday  = julian_day((long)year, (long)month, (long)day);
    //jday  = jday - julian_day((long)year,1,1) + 1;

    int in_msec = fnamein.mid(9,3).toInt(&ok,16);

    //msec
    pclk = TC/256.f;

    msec = double(in_msec)*4096.f*1000.f/pclk;
    smsec = qRound(msec);

    //qDebug()<<"msec:"<<msec;

    Datatime = QString("%1-%2-%3 %4:%5:%6.%7")
            .arg(year)
            .arg(month,2,10,QChar('0'))
            .arg(day,2,10,QChar('0'))
            .arg(hour,2,10,QChar('0'))
            .arg(min,2,10,QChar('0'))
            .arg(sec,2,10,QChar('0'))
            .arg(smsec,3,10,QChar('0'));

    return Datatime;
}


QDateTime gFromDateTime = QDateTime();
QDateTime gToDateTime = QDateTime();
double    m_TCValue = 3145728772;

void  setDownloadPeriod(QDateTime fromTime,QDateTime toTime)
{
    gFromDateTime = fromTime;
    gToDateTime = toTime;
}
void  setTcValue(const double &value)
{
    m_TCValue = value;
}

QDateTime dataFile2DateTime(QString fnamein, double TC)
{
    //文件名长度 = 12
    assert(fnamein.length() == 12);

    unsigned int date[8],fnlength;
    short year, month, day,  hour, min, sec,smsec;//jday,
    double msec;
    double pclk;

    int i;
    bool ok;
    QString Datatime;
    //QString syear, smonth, sday, shour, smin, ssec,Datatime;//sjday,

    fnlength=fnamein.length();

    /* get the date and time information for the file name */
    for (i=0;i<8;i++) date[i]=fnamein.mid(i,1).toInt(&ok,16);
    year  = (date[0] << 2 | date[1] >> 2)  & 0x1f;  /* 0x0f=00011111 */
    month = (date[1] << 2 | date[2] >> 2)  & 0x0f;  /* 0x0f=00001111 */
    day   = (date[2] << 3 | date[3] >> 1)  & 0x1f;  /* 0x1f=00011111 */
    hour  = (date[3] << 4 | date[4])       & 0x1f;  /* 0x1f=00011111 */
    min   = (date[5] << 2 | date[6] >> 2)  & 0x3f;  /* 0x3f=00111111 */
    sec   = (date[6] << 4 | date[7])       & 0x3f;  /* 0x3f=00111111 */

    year  = year + 2000;
    if( year == 2000 ){
        year += 16;
    }

    //jday  = julian_day((long)year, (long)month, (long)day);
    //jday  = jday - julian_day((long)year,1,1) + 1;

    int in_msec = fnamein.mid(9,3).toInt(&ok,16);

    //msec
    pclk =  TC/256.f;
    msec =  double(in_msec)*4096.0*1000.0/pclk;

    smsec = qRound(msec);

    Datatime = QString("%1-%2-%3 %4:%5:%6.%7")
            .arg(year)
            .arg(month,2,10,QChar('0'))
            .arg(day,2,10,QChar('0'))
            .arg(hour,2,10,QChar('0'))
            .arg(min,2,10,QChar('0'))
            .arg(sec,2,10,QChar('0'))
            .arg(smsec,3,10,QChar('0'));

    return QDateTime::fromString(Datatime,"yyyy-MM-dd HH:mm:ss.zzz");
}

//起始时间、结束时间和TC值在Ftpmanager界面管理设置
bool isFileInPeriod(const QString filename)
{
    QDateTime dateTime = dataFile2DateTime(filename,m_TCValue);
    qDebug()<<"file time:"<<dateTime.toString("yyyy-MM-dd HH:mm:ss");
    if(dateTime>=gFromDateTime && dateTime<=gToDateTime)
        return true;
    return false;
}

int swap3(unsigned char* P)
{
    int res;
    if(P[0]&0x80)
    {
        res = ((unsigned int)(0XFF000000) +(((unsigned int)P[0])<<16) +
                (((unsigned int)P[1])<<8) + (unsigned int)P[2]);
    }
    else
    {
        res = (int)((((unsigned int)P[0])<<16)+(((unsigned int)P[1])<<8)+
                (unsigned int)P[2]);
    }
    return res;
}

