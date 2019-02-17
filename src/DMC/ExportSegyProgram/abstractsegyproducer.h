#ifndef ABSTRACTSEGYPRODUCER_H
#define ABSTRACTSEGYPRODUCER_H

#include "gpublic.h"

//文件最大大小，120M
#define Max_Data_File_Size  120*1024*1024

#define DateTime_Format_     "yyyy-MM-dd HH:mm:ss.zzz"
#define DateTime_Format_Sec  "yyyy-MM-dd HH:mm:ss"
#define DateTime_Format_Day  "yyyy-MM-dd"

//每个数据文件
struct FileInfo
{
    FileInfo():fp(0),data(0){}
    bool      exist;
    QString   fileName;//文件名
    QString   path;
    long long size;    //文件大小,除最后一个文件外存在的文件外，其他都是120M
    QDateTime dateTime;//"yyyy-MM-dd HH:mm:ss.zzz"
    FILE     *fp;
    unsigned char  *data;//数据
};

//有效数据文件总的信息
struct DataFileInfo
{
    ~DataFileInfo(){
        qDeleteAll(fileInfos);
        fileInfos.clear();
    }

    double             TC;//TC值
    double             timeErr;  //钟差   Sec
    int                frequency;//采样率 Hz
    long long          NTPS_all; //总的采样个数
    QDateTime          startTime;//第一个采样文件时间基准值

    QString            dataPath;//文件路径

    QList <FileInfo* > fileInfos;//有效数据文件列表
};

//投放设备信息
struct DepolyedDevice
{
    QString device;
    QString ip;
    QString dataFile;
    float   line;
    int     station;
    float   x;
    float   y;
    float   z;

    Station recvStation;
};

struct ShotLineTime
{
    ShotLineTime():staNum(0),shotTimes(0){}
    ~ShotLineTime(){

    }
    void clear(){
        if(shotTimes)
        {
            delete[] shotTimes;
            shotTimes = 0;
        }
        staNum = 0;
    }
    int       staNum;
    QDateTime *shotTimes;
};

struct ShotLineTimes
{
    ShotLineTimes()
        :lineNum(0),shotLineTimes(0){}
    ~ShotLineTimes(){

    }
    void clear()
    {
        for(int i =0;i<lineNum;i++)
        {
            shotLineTimes[i].clear();
        }
        if(shotLineTimes){
            delete[] shotLineTimes;
            shotLineTimes = 0;
        }
        lineNum = 0;
    }
    int              lineNum;
    ShotLineTime    *shotLineTimes;
};


class AbstractSegyProducer
{
public:
    AbstractSegyProducer();
    virtual ~AbstractSegyProducer();
    void    setParameter(const ExportSegyParameter &parameter){m_Parameter = parameter;}

    //初始化参数和环境
    bool init();
    //开始运行-------
    void start();

protected:
    /*执行步骤:预处理->运行->结束
     *后处理只有抽共炮数据才执行，继续将共检波点数据抽取
    */
    virtual bool   preProcesss() = 0;
    //数据处理程序
    virtual bool   run() = 0;
    bool           finalize();

    bool        initLog(const QString &projPath,const QString &jobName);
    static void myMsgHandler(QtMsgType qtMsgType,const QMessageLogContext &, const QString &);

    //读取SPS文件,获取工区信息
    bool readSpsFiles();

protected:
    ExportSegyParameter m_Parameter;
    QString             m_errString;
    static QFile       *m_logFile;

    AreaDataInfo       *m_areaDataInfo;
};



#endif // ABSTRACTSEGYPRODUCER_H
