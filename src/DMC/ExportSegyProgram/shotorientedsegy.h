#ifndef SHOTORIENTEDSEGY_H
#define SHOTORIENTEDSEGY_H

#include "abstractsegyproducer.h"

struct RecvFile
{
    RecvFile():nTraces(0),curTrace(0){}
    QString   fileName;
    long long nTraces;//总道数
    long long curTrace;//当前道数
};

struct RecvFilesList
{
    ~RecvFilesList(){
        qDeleteAll(bhxRecvFiles);
        bhxRecvFiles.clear();
        qDeleteAll(bhyRecvFiles);
        bhyRecvFiles.clear();
        qDeleteAll(bhzRecvFiles);
        bhzRecvFiles.clear();
        qDeleteAll(hydRecvFiles);
        hydRecvFiles.clear();
    }

    QVector<RecvFile*> bhxRecvFiles;
    QVector<RecvFile*> bhyRecvFiles;
    QVector<RecvFile*> bhzRecvFiles;
    QVector<RecvFile*> hydRecvFiles;
};

class ShotOrientedSegy : public AbstractSegyProducer
{
public:
    ShotOrientedSegy();
    ~ShotOrientedSegy();

protected:
    bool    preProcesss();
    bool    run();

    //打开输出文件,初始化卷头信息
    bool openLineSegyFiles(const StaLine &staLine,const int &start,const int &end);
    bool closeSegyFiles();

    bool readRecvFiles();
    bool allocInitMemory();
    //读取每一炮
    bool mainShotProcess(const int &line,const int& shot);
    //炮对应一道
    int  procTrace(const QString &fileName,const int &line,const int& shot
                   ,const int &handle,long long *curTrace);
    //读取和写入数据
    int readWriteData(OpenTrace *openTr,const int &line,const int& shot,const int &handle,long long *curTrace);

private:
    //-------
    RecvFilesList *m_recvFilesList;
    //输出文件
    SegyFileHandles *m_segyFileHandle;
    TraceHead *m_traceHead;
    char      *m_data;
    int        m_ns;
};

#endif // SHOTORIENTEDSEGY_H
