#include "shotorientedsegy.h"

#define RecvSegy_BHX_FileFilter "RecvLine*_bhx.segy"
#define RecvSegy_BHY_FileFilter "RecvLine*_bhy.segy"
#define RecvSegy_BHZ_FileFilter "RecvLine*_bhz.segy"
#define RecvSegy_HYD_FileFilter "RecvLine*_hyd.segy"

ShotOrientedSegy::ShotOrientedSegy()
{
    m_recvFilesList = 0;
}
ShotOrientedSegy::~ShotOrientedSegy()
{
    if(m_recvFilesList){
        delete m_recvFilesList;
        m_recvFilesList = 0;
    }
    if(m_segyFileHandle)
        delete m_segyFileHandle;
    if(m_data)
        delete[] m_data;
    if(m_traceHead)
        delete m_traceHead;
}
bool ShotOrientedSegy::preProcesss()
{
    qDebug()<<"Preprocess ...";
    bool ok = false;
    //1 读取sps 文件
    ok = readSpsFiles();
    if(!ok)
        return ok;

    //2 读取共检波点数据文件信息
    ok = readRecvFiles();
    if(!ok)
        return ok;

    //初始化文件卷头信息
    ok = allocInitMemory();
    if(!ok)
        return ok;

    return true;
}

bool ShotOrientedSegy::run()
{
    qDebug()<<QString("Start produce shot oriented segy file,at %1")
              .arg(QDateTime::currentDateTime().toString(DateTime_Format_Sec));

    //抽取范围-----------------------------------
    int startLine   = m_Parameter.lineScope.first;
    int lastLine    = m_Parameter.lineScope.second;
    int startStation= m_Parameter.staScope.first;
    int lastStation = m_Parameter.staScope.second;
    int end;

    StationInfo *shotStationInfo = m_areaDataInfo->shotStationInfo;
    StaLine staLine;
    int     lineNum,shotNum;
    bool ok = false;
    //每条炮线单独显示进度
    lineNum = lastLine -startLine +1;
    qDebug()<<QString("Totol shotLine :%1.").arg(lineNum);
    int line = 0;
    for(int lineNo = startLine;lineNo<=lastLine;lineNo++){
        staLine = shotStationInfo->staLines[lineNo-1];
        shotNum = staLine.staNum;
        end = lastStation>shotNum?shotNum:lastStation;

        qDebug()<<QString("%1% Process ShotLine %2,Shot: %3-%4.").arg(100*line/lineNum)
                  .arg(staLine.line).arg(startStation).arg(end);

        //打开炮线分量文件
        if(!openLineSegyFiles(staLine,startStation,end))
            return false;

        //遍历炮号------------
        for(int shotNo = startStation;shotNo<=end;shotNo++){
            ok =  mainShotProcess(lineNo,shotNo);
            if(!ok) {
                break;//跳到下一条线
            }
        }
        //关闭炮线文件--------
        if(!closeSegyFiles())
            return false;
        line ++;
    }
    qDebug()<<QString("100% Data Process Finished , at %1.")
              .arg(QDateTime::currentDateTime().toString(DateTime_Format_Sec));
    return true;
}

//打开输出文件,初始化卷头信息
bool ShotOrientedSegy::openLineSegyFiles(const StaLine &staLine,const int &start,const int &end)
{
    QStringList suffixs;
    suffixs<<"bhx"
          <<"bhy"
         <<"bhz"
        <<"hyd";
    QString fileName;
    int mode = Write_Only;
    int ok   = -1;
    for(int i=0;i<4;i++){
        fileName = QString("ShotLine%1_%2-%3_%4.segy").arg(int(staLine.line),2,10,QChar('0'))
                .arg(start).arg(end).arg(suffixs[i]);
        fileName = m_Parameter.outputPath +Dir_Separator+fileName;
        //qDebug()<<"open "<<fileName;
        OpenTrace *openTrace = gobs_open_segy_func(fileName.toLatin1().data()
                                                   ,&mode,m_segyFileHandle->binary3200
                                                   ,m_segyFileHandle->binaryhead
                                                   ,m_segyFileHandle->segyInfo
                                                   ,&ok);
        if(ok!=0){
            m_errString = QString("Open Outputfile %1 failed.")
                    .arg(fileName);
            return false;
        }
        m_segyFileHandle->fileHandles[i].fileName = fileName;
        m_segyFileHandle->fileHandles[i].openTrace = openTrace;
    }
    return true;
}

bool ShotOrientedSegy::closeSegyFiles()
{
    int ok = -1;
    for(int i =0;i<4;i++){
        gobs_close_segy_func(m_segyFileHandle->fileHandles[i].openTrace,&ok);
        if(ok!=0){
            m_errString = QString("Close Ouputfile %1 failed.")
                    .arg(m_segyFileHandle->fileHandles[i].fileName);
            return false;
        }
    }
    return true;
}

bool ShotOrientedSegy::readRecvFiles()
{
    m_recvFilesList = new RecvFilesList;

    QDir dir(m_Parameter.dataPath);
    QStringList fileList;
    fileList= dir.entryList(QStringList()<<RecvSegy_BHX_FileFilter,QDir::Files,QDir::Name);
    if(fileList.size()<1){
        m_errString = "Read no reciever segy files.";
        return false;
    }
    //x 分量segy文件
    m_recvFilesList->bhxRecvFiles.resize(fileList.size());
    for(int i=0;i<fileList.size();i++)
    {
        RecvFile *recvFile = new RecvFile;
        recvFile->fileName = m_Parameter.dataPath+Dir_Separator+fileList[i];
        m_recvFilesList->bhxRecvFiles[i]= recvFile;
    }
    //y 分量segy文件
    fileList= dir.entryList(QStringList()<<RecvSegy_BHY_FileFilter,QDir::Files,QDir::Name);
    m_recvFilesList->bhyRecvFiles.resize(fileList.size());
    for(int i=0;i<fileList.size();i++)
    {
        RecvFile *recvFile = new RecvFile;
        recvFile->fileName = m_Parameter.dataPath+Dir_Separator+fileList[i];
        m_recvFilesList->bhyRecvFiles[i]= recvFile;
    }
    //z 分量segy文件
    fileList= dir.entryList(QStringList()<<RecvSegy_BHZ_FileFilter,QDir::Files,QDir::Name);
    m_recvFilesList->bhzRecvFiles.resize(fileList.size());
    for(int i=0;i<fileList.size();i++)
    {
        RecvFile *recvFile = new RecvFile;
        recvFile->fileName = m_Parameter.dataPath+Dir_Separator+fileList[i];
        m_recvFilesList->bhzRecvFiles[i]= recvFile;
    }
    //d 分量segy文件
    fileList= dir.entryList(QStringList()<<RecvSegy_HYD_FileFilter,QDir::Files,QDir::Name);
    m_recvFilesList->hydRecvFiles.resize(fileList.size());
    for(int i=0;i<fileList.size();i++)
    {
        RecvFile *recvFile = new RecvFile;
        recvFile->fileName = m_Parameter.dataPath+Dir_Separator+fileList[i];
        m_recvFilesList->hydRecvFiles[i]= recvFile;
    }

    qDebug()<<"readed recvfiles,"
              " bhx files:"<<m_recvFilesList->bhxRecvFiles.size()
           <<"bhy files:"<<m_recvFilesList->bhyRecvFiles.size()
          <<"bhz files:"<<m_recvFilesList->bhzRecvFiles.size()
         <<"hyd files:"<<m_recvFilesList->hydRecvFiles.size();

    return true;
}

bool ShotOrientedSegy::allocInitMemory()
{
    //预先打开一个共检波点文件，赋卷头信息
    char       *binary3200 = new char[3200];
    BinaryHead *binaryhead = new BinaryHead;
    SegyInfo   *segyInfo   = new SegyInfo;
    int mode = Read_Only;
    int ok = -1;
    OpenTrace *openTr = gobs_open_segy_func(m_recvFilesList->bhxRecvFiles[0]->fileName.toLatin1()
            ,&mode,binary3200,binaryhead,segyInfo,&ok);
    if(ok!=0){
        m_errString = "Open reciver segy file failed.";
        return false;
    }
    gobs_close_segy_func(openTr,&ok);
    m_segyFileHandle = new  SegyFileHandles;
    m_segyFileHandle->binary3200 = binary3200;
    m_segyFileHandle->binaryhead = binaryhead;
    m_segyFileHandle->segyInfo   = segyInfo;

    //根据采样点数开辟数据空间
    m_traceHead = new TraceHead;
    memset(m_traceHead,0,sizeof(TraceHead));

    m_ns = binaryhead->nt;
    //四分量数据
    m_data = new char[m_ns*4];

    return true;
}

bool ShotOrientedSegy::mainShotProcess(const int &line,const int& shot)
{
    int file_num = m_recvFilesList->bhxRecvFiles.size();
    QString fileName;
    int ok = -1;
    long long *currentTr;
    long long  traceNo;
    for(int i =0;i<file_num;i++)
    {
        //x分量---
        fileName = m_recvFilesList->bhxRecvFiles[i]->fileName;
        currentTr= &m_recvFilesList->bhxRecvFiles[i]->curTrace;
        ok = procTrace(fileName,line,shot,0,currentTr);
        if(ok<0)
            return false;
        else if(ok>0)
            continue;
        traceNo = *currentTr-1;
        traceNo = traceNo<0?0:traceNo;

        //y分量---
        fileName = m_recvFilesList->bhyRecvFiles[i]->fileName;
        m_recvFilesList->bhyRecvFiles[i]->curTrace = traceNo;
        currentTr= &m_recvFilesList->bhyRecvFiles[i]->curTrace;
        ok = procTrace(fileName,line,shot,1,currentTr);
        if(ok<0)
            return false;
        else if(ok>0)
            continue;

        //z分量---
        fileName = m_recvFilesList->bhzRecvFiles[i]->fileName;
        m_recvFilesList->bhzRecvFiles[i]->curTrace = traceNo;
        currentTr= &m_recvFilesList->bhzRecvFiles[i]->curTrace;
        ok = procTrace(fileName,line,shot,2,currentTr);
        if(ok<0)
            return false;
        else if(ok>0)
            continue;

        //d分量---
        fileName = m_recvFilesList->hydRecvFiles[i]->fileName;
        m_recvFilesList->hydRecvFiles[i]->curTrace = traceNo;
        currentTr= &m_recvFilesList->hydRecvFiles[i]->curTrace;
        ok = procTrace(fileName,line,shot,3,currentTr);
        if(ok<0)
            return false;
        else if(ok>0)
            continue;
    }
    return true;
}

//打开，读取，判断，关闭，写入,一道
int ShotOrientedSegy::procTrace(const QString &fileName,const int &line,const int& shot
                                ,const int &handle,long long *curTrace)
{
    int mode = Read_Only;
    int ok = -1;
    //1 打开输入文件文件
    OpenTrace *openTr = gobs_open_segy_func(fileName.toLatin1().data(),&mode
                                            ,m_segyFileHandle->binary3200,m_segyFileHandle->binaryhead
                                            ,m_segyFileHandle->segyInfo,&ok);
    if(ok!=0){
        m_errString = QString("Process ShotLine %1 Shot %2 error,Open Inputfile %3 failed.")
                .arg(line)
                .arg(shot)
                .arg(fileName);
        return -1;
    }

    //2 读取数据，写入----------------
    int isOk = readWriteData(openTr,line,shot,handle,curTrace);
    if(isOk<0) //错误
    {
        m_errString = QString("Process ShotLine %1 Shot %2 in file %3 error,ERR:%4")
                .arg(line)
                .arg(shot)
                .arg(fileName)
                .arg(m_errString);
    }

    //3 关闭文件---------------------
    gobs_close_segy_func(openTr,&ok);
    if(ok!=0){
        m_errString = QString("Process ShotLine %1 Shot %2 error,Close Inputfile %3 failed.")
                .arg(line)
                .arg(shot)
                .arg(fileName);
        return -1;
    }

    return isOk;
}

int ShotOrientedSegy::readWriteData(OpenTrace *openTr,const int &line,const int& shot
                                    ,const int &handle,long long *curTrace)
{
    int ok = -1;
    long long readedTr;
    while(1){
        gobs_read_segy_func(openTr,*curTrace
                            ,1,m_traceHead,m_data,&readedTr,&ok);
        //没有数据直接返回
        if(readedTr==0){
            return 1;
        }
        if(ok!=0) {
            m_errString = QString("Read Inputfile failed.");
            return -2;
        }
        //找到该道，写入
        if(m_traceHead->unass[0]==line && m_traceHead->unass[1]==shot)
        {
            ++*curTrace;
            gobs_write_segy_func(m_segyFileHandle->fileHandles[handle].openTrace
                                 ,m_traceHead,m_data,&ok);
            if(ok!=0){
                m_errString = QString("Write Outputfile failed.")
                        .arg(m_segyFileHandle->fileHandles[handle].fileName);
                return -3;
            }
            break;
        }
        //在此道之前，继续读取
        else if(m_traceHead->unass[0]<line || m_traceHead->unass[1]<shot)
            ++*curTrace;
        else//超过，返回
            return 2;
    }

    return 0;
}
