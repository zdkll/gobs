#ifndef RECVORIENTEDSEGY_H
#define RECVORIENTEDSEGY_H

#include "abstractsegyproducer.h"

class RecvOrientedSegy : public AbstractSegyProducer
{
public:
    RecvOrientedSegy();
    ~RecvOrientedSegy();
protected:
    bool    preProcesss();
    bool    run();

private:
    //查询数据文件目录，所有数据设备,从数据库查找其对应接受站点
    bool getDevicesInfo();

    //申请道内存,数据道内存-----
    bool allocInitMemory();

    //解析炮时文件
    bool resvShotInform();

    //根据设备和接收站点，接受站点范围，检出有效设备
    QList<DepolyedDevice *> checkEffectiveDevice();

    //读取数据文件所有信息-----
    DataFileInfo *readDataFileInfo(DepolyedDevice *depolyedDevice);

    //找到的GOBS对应的站点
    Station findStation(const float &line,const int &station) const;

    bool findStation(DepolyedDevice *depolyedDevice,int *lineNo, int *staNo);

    //读取文件TimeErr
    bool readTimeErrFile(const QString &filename,DataFileInfo *dataFileInfo);
    //读取最新的log 文件
    bool readLogFile(DataFileInfo *dataFileInfo);
    //数据目录下的数据文件整理
    bool getDataFiles(const QString &dataPath,DataFileInfo *dataFileInfo,QString *errStr);

    //根据文件列表计算总的样点数
    long long calTotalNs(const QString &dataPath,const QList<FileInfo*> &fileInfos);

    //run 子函数-------------------------
    //打开输出文件,初始化卷头信息
    bool openSegyFiles(DepolyedDevice *);
    bool closeSegyFiles(bool ok = true);

    bool openInputFile(FileInfo*  fileInfo);
    void closeInputFile(FileInfo* fileInfo);

    //数据处理主函数
    bool mainRecvProcess(DataFileInfo *);
    //计算炮时
    double calTimeErr(DataFileInfo *dataFileInfo,const int &file_index,const QDateTime &shotTime);

    bool readDataFromFile(FileInfo *fileInfo,int sample_offset,int samples,float *in,int ins);

private:
    QList<DepolyedDevice *> m_depolyedDevices;
    ShotLineTimes      *m_shotLineTimes;

    //输出文件
    SegyFileHandles *m_segyFileHandle;
    TraceHead       *m_traceHead;//道头
    float           *m_data;     //数据
};

#endif // RECVORIENTEDSEGY_H
