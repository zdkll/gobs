#include "nodeftp.h"
#include "rnitems.h"
#include "gpublicfunctions.h"

#include <QHostAddress>
#include <QDebug>

#define  FTP_User_Name      "admin"
#define  FTP_User_PassWord  "admin"

//设置最大配置文件大小，判断是配置文件还是数据文件
#define  Max_Config_File_Size   10E3

FtpManager::FtpManager(Node *owner)
    :QObject(owner)
    ,m_node(owner)
{
    m_Thread = new QThread(this);
    m_nodeFtp = new NodeFtp;

    m_nodeFtp->moveToThread(m_Thread);

    connect(this,SIGNAL(signalStartWork(int,QVariant)),
            m_nodeFtp,SLOT(startWork(int,QVariant)),Qt::QueuedConnection);
    connect(this,SIGNAL(signalStopWork()),
            m_nodeFtp,SLOT(stopWork()),Qt::QueuedConnection);

    connect(m_nodeFtp,SIGNAL(memoryChanged(float))
            ,SLOT(slotMemoryChanged(float)),Qt::QueuedConnection);
    connect(m_nodeFtp,SIGNAL(frequencyChanged(int))
            ,SLOT(slotFrequencyChanged(int)),Qt::QueuedConnection);
    connect(m_nodeFtp,SIGNAL(downloadPercentChanged(int))
            ,SLOT(slotDownloadPercentChanged(int)),Qt::QueuedConnection);
    connect(m_nodeFtp,SIGNAL(finishedFtpWork(int,bool))
            ,SLOT(slotFinishedFtpWork(int,bool)),Qt::QueuedConnection);

    //Note
    connect(m_nodeFtp,SIGNAL(noteChanged(QString))
            ,SLOT(slotNoteChanged(QString)),Qt::QueuedConnection);

}

FtpManager::~FtpManager()
{
    if(m_Thread && m_Thread->isRunning()){
        m_Thread->quit();
        m_Thread->wait();
    }
    delete m_nodeFtp;
}


void FtpManager::startWork(const int &ftpWk,const QVariant &arg)
{
    if(m_Thread->isRunning())
        return;
    m_nodeFtp->setDevInfo(m_node->no(),m_node->ip());

    //启动子线程工作
    m_Thread->start();
    emit signalStartWork(ftpWk,arg);
}

void FtpManager::stopFtpWork()
{
    emit signalStopWork();
}

void FtpManager::slotMemoryChanged(float memory)
{
    qDebug()<<"slot memory changed:"<<QThread::currentThread();
    qDebug()<<"meory changed:"<<memory;
    m_node->setMemory(memory);
    m_node->updateNode(Ftp_Update_Memory);
}

void FtpManager::slotFrequencyChanged(int frequency)
{
    m_node->setSampleFrequency(frequency);
    m_node->updateNode(Ftp_Update_SampleFrequency);
}

void FtpManager::slotDownloadPercentChanged(int percent)
{
    m_node->setDownloadPercent(percent);
    m_node->updateNode(Ftp_Update_Progress);
}

void FtpManager::slotNoteChanged(const QString &note)
{
    m_node->setInfoString(note);
    m_node->updateNode(Ftp_Update_Note);
}
void FtpManager::slotFinishedFtpWork(int ftpWork,bool error)
{
    //结束线程
    if(m_Thread->isRunning())
    {
        m_Thread->quit();
        m_Thread->wait();
    }
    //完成工作
    m_node->finishedFtpWork(ftpWork,error);
}




//=======================================================
NodeFtp::NodeFtp(QObject *parent)
    :QObject(parent)
    ,m_Ftp(0),m_file(0)
    ,m_workStep(NoneWork)
{
    qRegisterMetaType<QUrlInfo>("QUrlInfo");
    m_stoped = false;
}

void NodeFtp::setDevInfo(uint devNo,uint ip)
{
    node_ip = QHostAddress(ip).toString();
    node_no = QString("G%1").arg(devNo,3,10,QChar('0'));
}



void NodeFtp::startWork(const int &ftpWk,const QVariant &arg)
{
    qDebug()<<"main thread:"<<qApp->thread()
           <<"current thread:"<<QThread::currentThread();
    m_workStep = ftpWk;
    emit noteChanged(workName(m_workStep));
    if(!m_Ftp){
        m_Ftp = new QFtp(this);
        connect(m_Ftp,SIGNAL(commandFinished(int,bool))
                ,this,SLOT(slotCommandFinished(int,bool)),Qt::DirectConnection);
        connect(m_Ftp,SIGNAL(listInfo(QUrlInfo))
                ,this,SLOT(slotListInfo(QUrlInfo)),Qt::DirectConnection);
    }
    //1 更新节点信息
    if(m_workStep == Updating)
    {
        m_workPath = arg.toString();
        //文件目录
        m_workPath += Dir_Separator + node_no;

        //查询本地文件目录，如果有配置文件读取采样率，没有在list之后需要下载
        m_FilesData.urlInfos.clear();
    }
    //2 下载文件
    else if(m_workStep == DownLoadFiles) {
        m_downloadType = arg.toInt();
    }
    //3 清除存储
    else if(m_workStep == ClearMemory){
    }
    //4 设置采样率
    else if(m_workStep == SetSampleFrequecy)
    {
        if(m_sampleFrequency == arg.toInt()){
            emit finishedFtpWork(m_workStep,true);
            emit noteChanged("Finished");
            m_workStep = NoneWork;
            return;
        }
        m_sampleFrequency = arg.toInt();
        QString filename = m_workPath + Dir_Separator + Node_NODAL_FILE;
        if(!changeLocalFrequency(m_sampleFrequency,filename)){

            emit finishedFtpWork(m_workStep,true);
            emit noteChanged("Finished");
            m_workStep = NoneWork;
            return;
        }
    }
    m_stoped = false;
    if(m_Ftp->state() == QFtp::Unconnected){
        qDebug()<<"connect:"<<node_ip;
        m_ftpCmds.ConToHost = m_Ftp->connectToHost(node_ip);
    }
}
void NodeFtp::stopWork()
{
    qDebug()<<"node stoped:"<<m_stoped;
    emit noteChanged("Stopping");
    m_stoped = true;
}
void NodeFtp::slotCommandFinished(int cmd, bool error)
{
    if(m_ftpCmds.ConToHost == cmd)
    {
        if(!error)
        {
            m_ftpCmds.LogIn = m_Ftp->login(FTP_User_Name,FTP_User_PassWord);
        }else{
            m_workStep = NoneWork;
        }
    }
    if(m_ftpCmds.LogIn == cmd)
    {
        if(!error)
        {
            //如果是查询
            if( m_workStep == Updating)
                m_ftpCmds.List = m_Ftp->list("/");
            //下载文件
            else if(m_workStep == DownLoadFiles)
            {
                startDownload();
            }
            //设置采样率
            else if(m_workStep == SetSampleFrequecy)
            {
                uploadNodalFile();
            }
            //清除容量
            else if(m_workStep == ClearMemory)
            {
                //判断是否有文件
                if(m_FilesData.urlInfos.size()<1)
                    ;//do nothing
                else
                    startClearMemory();
            }
        }
        else
        {
            //结束
            m_workStep = NoneWork;
            m_Ftp->close();
        }
    }
    if(m_ftpCmds.List == cmd)
    {
        if(!error)
        {
            //如果是查询
            if(m_workStep == Updating){
                updateInformation();
            }
        }else
        {
            //结束
            m_workStep = NoneWork;
            m_Ftp->close();
        }
    }
    else if(cmd == m_ftpCmds.get)
    {
        if(m_workStep == Updating)
        {
            //更新时候，下载完成配置文件，读取刷新采样率
            downloadedConfigureFile();

        }else if(m_workStep == DownLoadFiles)
        {
            finishDownload();
        }
    }
    else if(cmd == m_ftpCmds.put)
    {
        if(!error)
        {
            if(m_workStep == SetSampleFrequecy)
            {
                finishedSetFrequency();
            }
        }
    }
    //清除存储
    else if(cmd == m_ftpCmds.remove)
    {
        if(!error)
        {
            clearNextMemory();
        }
    }
    //close 结束任务
    else if(cmd == m_ftpCmds.close)
    {
        int temWork = m_workStep;
        m_workStep = NoneWork;
        emit finishedFtpWork(temWork);
        if(m_stoped)
            emit noteChanged("Stopped");
        else
            emit noteChanged("Finished");
    }
}

void NodeFtp::slotListInfo(QUrlInfo url)
{
    m_FilesData.urlInfos.append(url);
}

void NodeFtp::startDownload()
{
    qDebug()<<"start download:"<<m_downloadType;
    m_FilesData.workingFiles.clear();
    //初始化下载文件信息：1)全部下载
    if(m_downloadType == DownloadAll){
        //服务器所有文件列表，去掉已经下载的文件
        foreach (QUrlInfo url,  m_FilesData.urlInfos) {
            if(!m_FilesData.localFiles.contains(url.name()))  //已经完成文件
                m_FilesData.workingFiles.append(url.name());
        }
    }

    //2)下载时间段内数据文件
    else if(m_downloadType == DownloadInPeriod)
    {
        //判断是否在时间段内
        foreach (QUrlInfo url,  m_FilesData.urlInfos) {
            //本地包含就不下载
            if(m_FilesData.localFiles.contains(url.name()))
                continue;
            //如果是数据文件则计算时间
            if(!(url.name().endsWith(".log")||url.name().endsWith(".lst")
                 ||url.name().endsWith(".CFG"))){
                //计算文件事件判断是否需要下载
                if(isFileInPeriod(url.name()))
                    m_FilesData.workingFiles.append(url.name());
                continue;
            }
            //其他文件
            m_FilesData.workingFiles.append(url.name());
        }
    }
    //    qDebug()<<"working:"<<m_FilesData.workingFiles
    //           <<"fninshed:"<<m_FilesData.finishedFiles
    //          <<"unfinished:"<<m_FilesData.unfinishedFiles;
    //更新下载进度
    int progress = calDownloadProgress();
    emit downloadPercentChanged(progress);
    //如果没有目录则创建目录
    QDir dir(m_workPath);
    if(!dir.exists())
        dir.mkdir(m_workPath);

    if(!m_file)
        m_file = new QFile(this);

    //开始下载第一个文件----
    downloadNext();
}

//初始化信息
void NodeFtp::updateInformation()
{
    qDebug()<<"update information thread:"<<QThread::currentThread();
    //1 计算节点剩余存储,GB为单位保存
    float gMem = countFilesSize(m_FilesData.urlInfos);
    emit memoryChanged(32.0f - gMem);

    //2 初始化本地文件目录，计算下载进度
    QDir dir(m_workPath);
    if(!dir.exists())
        dir.mkdir(m_workPath);

    m_FilesData.localFiles = dir.entryList(QDir::Files);
    //服务器所有文件列表，去掉已经下载的文件
    int percent = 100*m_FilesData.localFiles.size()/m_FilesData.urlInfos.size();
    percent = percent>100?100:percent;
    emit downloadPercentChanged(percent);

    //3 采样率查询
    QString filename = m_workPath + Dir_Separator + Node_NODAL_FILE;
    QFile  file(filename);
    //本地配置文件存在
    if(file.exists())
    {
        int frequency = getFrequencyFromFile(filename);
        if(frequency>0)
        {
            m_sampleFrequency = frequency;
            emit frequencyChanged(m_sampleFrequency);

            //关闭链接，结束任务
            m_ftpCmds.close = m_Ftp->close();
            return;
        }
    }

    //配置文件不存在，下载配置文件
    //开始下载
    if(!m_file)
        m_file = new QFile(this);
    m_FilesData.currentFile = Node_NODAL_FILE;
    filename = m_workPath + Dir_Separator + Node_NODAL_FILE;
    m_file->setFileName(filename);
    m_file->open(QIODevice::WriteOnly);

    m_ftpCmds.get = m_Ftp->get(Node_NODAL_FILE,m_file);
}

void NodeFtp::startClearMemory()
{
    m_FilesData.workingFiles.clear();
    //去掉Node_NODAL_FILE File
    foreach (QUrlInfo url, m_FilesData.urlInfos) {
        if(url.name() != Node_NODAL_FILE)
            m_FilesData.workingFiles.append(url.name());
    }
    //从第一个开始清除
    m_ftpCmds.remove = m_Ftp->remove(m_FilesData.workingFiles.takeFirst());
}

float NodeFtp::countFilesSize(const QList<QUrlInfo> &files)
{
    long long memSize = 0;
    foreach (QUrlInfo url, files) {
        memSize += url.size();
    }
    //GB为单位保存
    float gMem = float((double)memSize/(double)(1024*1024*1024));
    return gMem;
}

int NodeFtp::calDownloadProgress()
{
    int percent = 0;

    //更新下载文件进度
    if(m_FilesData.urlInfos.size()<1)
        percent = 0;
    else
    {
        percent = (100.f*float(m_FilesData.localFiles.size())
                   /float(m_FilesData.urlInfos.size()));
        percent = percent>100?100:percent;
    }
    return percent;
}

void NodeFtp::finishDownload()
{
    //关闭文件
    m_file->close();
    //更新本地文件进度
    m_FilesData.localFiles.append(m_FilesData.currentFile);
    int progress = calDownloadProgress();
    emit downloadPercentChanged(progress);

    //已经停止，结束下载
    if(m_stoped)
    {
        qDebug()<<"download stoped";
        m_ftpCmds.close = m_Ftp->close();
    }else
        //继续下一个文件
        downloadNext();
}
int NodeFtp::getFrequencyFromFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return -1;
    QTextStream in(&file);
    QString context = in.readAll();
    QString sampleLine = context.split("\n",QString::SkipEmptyParts).at(1);
    QString sampleStr = sampleLine.mid(sampleLine.indexOf(":")+1,4);
    file.close();
    return sampleStr.toInt();
}

void NodeFtp::downloadedConfigureFile()
{
    m_file->close();
    //下载完成，更新采样率
    {
        //配置文件下载完成，更新采样频率
        QString filename = m_workPath + Dir_Separator + Node_NODAL_FILE;
        int frequency = getFrequencyFromFile(filename);
        m_sampleFrequency = frequency;

        emit frequencyChanged(m_sampleFrequency);
        //下载完成，结束
        m_ftpCmds.close = m_Ftp->close();
    }
}

void NodeFtp::downloadNext()
{
    if(!m_FilesData.workingFiles.isEmpty()){
        m_FilesData.currentFile = m_FilesData.workingFiles.takeFirst();
        QString fileName = m_workPath+Dir_Separator+m_FilesData.currentFile;
        m_file->setFileName(fileName);
        if(!m_file->open(QIODevice::WriteOnly))
            return;
        qDebug()<<"download "<<m_FilesData.currentFile<<" to "<<fileName;
        m_ftpCmds.get = m_Ftp->get(m_FilesData.currentFile,m_file);
    }else
    {
        //下载完成，结束
        m_ftpCmds.close = m_Ftp->close();
    }
}

bool NodeFtp::changeLocalFrequency(const int &frequency,const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream in(&file);
    QString context = in.readAll();
    file.close();
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QStringList list = context.split("\n",QString::SkipEmptyParts);

    int index = list.at(1).indexOf(":");
    QString prex = list.at(1).mid(0,index+1);
    QString last;
    if(frequency == 1000)
        last = "1000 //500,250";
    else if(frequency == 500)
        last = "500 //1000,250";
    else if(frequency == 250)
        last = "250 //1000,500";
    list[1] = prex+last;
    QTextStream out(&file);
    for(int i=0;i<list.size();i++)
        out<<list[i]<<endl;
    file.close();
    return true;
}

void NodeFtp::uploadNodalFile()
{
    //上传配置文件
    if(!m_file)
        m_file = new QFile(this);
    QString filename = m_workPath +Dir_Separator+Node_NODAL_FILE;
    m_file->setFileName(filename);
    qDebug()<<"upload:"<<filename;
    qDebug()<<"open:"<< m_file->open(QIODevice::ReadOnly);
    m_ftpCmds.put = m_Ftp->put(m_file,Node_NODAL_FILE);
}

void NodeFtp::finishedSetFrequency()
{
    m_file->close();
    qDebug()<<"finished put";
    emit frequencyChanged(m_sampleFrequency);
    //关闭链接，结束任务
    m_ftpCmds.close = m_Ftp->close();
}

void NodeFtp::clearNextMemory()
{
    //判断清除完成
    if(m_FilesData.workingFiles.isEmpty())
    {
        emit memoryChanged(32.0f);
        m_ftpCmds.close = m_Ftp->close();
    }
    else
    {
        //从第一个开始清除
        m_ftpCmds.remove = m_Ftp->remove(m_FilesData.workingFiles.takeFirst());
    }
}



