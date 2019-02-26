#include "recvorientedsegy.h"
#include "gsqlfunctions.h"
#include "toolsalgo.h"
#include "gpublicfunctions.h"

#define  DataFileList       "DataFile.lst"
#define  TimeErrFile        "TimeErr.log"
#define  LogFileFilter      "A*.log"
#define  DataFileFilter     "[0-9A-F]+\\.[0-9A-F]+"

#define First_Data_File_No   "0001"

#define Data_File_Size        125829120    //120*1024*1024 B

#define Data_File_Sample_Size 10485760     //10*1024*1024
//文件时间跨度大小,ms,以1000 为基准
#define Data_File_Msec_Size   10*1024*1024

//最大钟差，1s
#define Max_Time_Err           1.0


const  double TC0  = 3145728000;
const  double PCLK0= (TC0/256.0);
//const qint64 FileDataSecSize = 10*1024*1024*1000;

RecvOrientedSegy::RecvOrientedSegy()
{
    m_segyFileHandle = 0;
    m_traceHead = 0;
    m_data      = 0;
    m_shotLineTimes  = 0;
}
RecvOrientedSegy::~RecvOrientedSegy()
{
    if(m_segyFileHandle)
        delete m_segyFileHandle;
    if(m_shotLineTimes){
        m_shotLineTimes->clear();
        delete m_shotLineTimes;
    }
    if(m_traceHead)
        delete m_traceHead;
    if(m_data)
        delete[] m_data;
    qDeleteAll(m_depolyedDevices);
    m_depolyedDevices.clear();
}

bool RecvOrientedSegy::preProcesss()
{
    qDebug()<<"Preprocess...";//log
    //1 读取sps文件-------------
    bool ok = readSpsFiles();
    if(!ok)
        return ok;

    //2 查询本地设备数据目录，查找存在数据的节点设备
    ok = getDevicesInfo();
    if(!ok)
        return ok;

    //3 获取并初始化炮时信息
    ok = resvShotInform();
    if(!ok)
        return ok;

    //4 开辟初始化内存
    ok = allocInitMemory();
    if(!ok)
        return ok;

    return true;
}

bool RecvOrientedSegy::run()
{
    qDebug()<<QString("Start produce reciever oriented segy files,at %1")
              .arg(QDateTime::currentDateTime().toString(DateTime_Format_Sec));//log
    //找出所有设备
    QList<DepolyedDevice *> depolyedDevices = checkEffectiveDevice();
    if(depolyedDevices.size()<1)
    {
        m_errString = "No Effective Device.";
        return false;
    }
    qDebug()<<"Total GOBS Device:"<<depolyedDevices.size();//log
    bool ok = false;
    int finishedCount = 0;
    //任务遍历
    DepolyedDevice *depolyedDevice;
    for(int i =0;i<depolyedDevices.size();i++)//m_depolyedDevices.size()
    {
        depolyedDevice = depolyedDevices[i];
        qDebug()<<QString("%1% Process Line%2 GOBS%3 %4.")
                  .arg(i*100/depolyedDevices.size())
                  .arg(depolyedDevice->line)
                  .arg(depolyedDevice->station)
                  .arg(depolyedDevice->device);

        //1 读取设备数据信息
        DataFileInfo *dataFileInfo = readDataFileInfo(depolyedDevice);
        if(dataFileInfo == NULL){
            qDebug()<<m_errString;
            continue;
        }

        //接收点信息-------
        Station recvStation = depolyedDevice->recvStation;
        m_traceHead->gx     = recvStation.x;
        m_traceHead->gy     = recvStation.y;
        m_traceHead->gelev  = recvStation.elev;
        m_traceHead->tracf  = 1;
        m_traceHead->tracl  = 1;

        //保存接受点线号，保存在未使用地址段
        m_traceHead->unass[2] = recvStation.line;
        m_traceHead->unass[3] = recvStation.sp;

        //1 打开四分量输出文件
        ok = openSegyFiles(depolyedDevice);
        if(!ok){
            m_errString = QString("Process %1 Error,%2").arg(depolyedDevice->device)
                    .arg(m_errString);
            qDebug()<<m_errString;
            delete dataFileInfo;
            continue; //继续下一个
        }

        //2 循环炮线,炮时
        ok =  mainRecvProcess(dataFileInfo);
        {
            //关闭打开的文件
            foreach (FileInfo *fileInfo, dataFileInfo->fileInfos) {
                if(fileInfo->fp)
                    closeInputFile(fileInfo);
            }
            delete  dataFileInfo;
        }
        if(!ok){
            m_errString = QString("Process %1 Error,%2").arg(depolyedDevice->device).arg(m_errString);
            qDebug()<<m_errString;
            //关闭并删除文件
            closeSegyFiles(false);
            continue;
        }

        //3 关闭输出文件
        ok = closeSegyFiles();
        if(!ok){
            m_errString = QString("Process %1 Error,%2").arg(depolyedDevice->device).arg(m_errString);
            qDebug()<<m_errString;
            continue;
        }
        finishedCount ++;
    }
    qDebug()<<QString("100% Data Process Finished.%1").arg(QDateTime::currentDateTime().toString(DateTime_Format_));
    qDebug()<<QString("%1 GOBS completed , %2 Failed.").arg(finishedCount)
              .arg(depolyedDevices.size() - finishedCount);
    return true;
}

bool RecvOrientedSegy::getDevicesInfo()
{
    //1 数据库查询工区所有投放设备信息表-------
    qDeleteAll(m_depolyedDevices);
    m_depolyedDevices.clear();

    SqlFunctions *sqlFunc = SqlFunctions::instance();
    QList<Condition> conds;
    QStringList      fieldList;
    fieldList<<TABLE_FIELD_DEVICE
            <<TABLE_FIELD_IP
           <<TABLE_FIELD_LINE
          <<TABLE_FIELD_STATION
         <<TABLE_FIELD_ACTUALX
        <<TABLE_FIELD_ACTUALY
       <<TABLE_FIELD_ACTUALZ;
    bool ok = false;
    Condition  areaCond;
    areaCond.fieldName = TABLE_FIELD_AREA;
    areaCond.fieldValue = m_Parameter.areaName;
    conds.append(areaCond);
    QSqlQuery query = sqlFunc->query(m_Parameter.areaName,fieldList,conds,&ok);
    if(!ok)
    {
        m_errString = sqlFunc->errorString();
        return ok;
    }
    float   line;
    int     station;
    while(query.next())
    {
        line    = query.value(2).toFloat();
        station = query.value(3).toInt();
        if(line==0||station==0)
            continue;
        DepolyedDevice *depolyedDev = new DepolyedDevice;
        depolyedDev->device  = query.value(0).toString();
        depolyedDev->ip      = query.value(1).toString();
        depolyedDev->line    = line;
        depolyedDev->station = station;

        depolyedDev->x = query.value(4).toFloat();
        depolyedDev->y = query.value(5).toFloat();
        depolyedDev->z = query.value(6).toFloat();
        m_depolyedDevices.append(depolyedDev);
    }
    if(m_depolyedDevices.size()<1)
    {
        m_errString = "Database contains no deployed device.";
        return false;
    }

    //2 查询设置数据目录------------------------
    QString dataPath = m_Parameter.dataPath;
    QDir dir(dataPath);
    QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    // qDebug()<<"subDirs:"<<subDirs;

    //去除非"G"(或者“192”)开头的文件
    for(int i=subDirs.size()-1;i>=0;i--)
    {
        if(!subDirs.at(i).startsWith("G")&&!subDirs.at(i).startsWith("192"))
            subDirs.removeAt(i);
    }
    if(subDirs.size()<0){
        m_errString = "Data Path Contains No Data File";
        return false;
    }

    //判断文件名为G设备号还是192开头为IP地址
    int  gorIpFile = 0; //数据文件是 "G+设备号"：0，还是IP地址：1
    if(subDirs[0].startsWith("G"))
    {
        gorIpFile = 0;
    }
    else if(subDirs[0].startsWith("192"))
        gorIpFile = 1;

    //遍历每个投放的设备是否有本地数据，没有则将忽略该设备
    if(gorIpFile == 0){
        foreach (DepolyedDevice *depolyedDev, m_depolyedDevices) {
            if(subDirs.contains(depolyedDev->device))
                depolyedDev->dataFile = depolyedDev->device;
        }
    }
    else if(gorIpFile == 1)//IP地址为目录名
    {
        foreach (DepolyedDevice *depolyedDev, m_depolyedDevices) {
            //qDebug()<<depolyedDev->ip;
            if(subDirs.contains(depolyedDev->ip))
                depolyedDev->dataFile = depolyedDev->ip;
        }
    }

    return true;
}
bool RecvOrientedSegy::resvShotInform()
{
    //读取炮时所在年份-根据数据文件的日志文件日期判断
    int shot_year = -1;
    for(int i=0;i<m_depolyedDevices.size();i++)
    {
        if(!m_depolyedDevices[i]->dataFile.isEmpty()){
            QString dataPath = m_Parameter.dataPath+ Dir_Separator + m_depolyedDevices[i]->dataFile;
            QDir dir(dataPath);
            QStringList nameFilters;nameFilters<<LogFileFilter;

            QStringList logFiles = dir.entryList(nameFilters,
                                                 QDir::NoDotAndDotDot|QDir::Files
                                                 ,QDir::Name);
            if(logFiles.size()<1)
                continue;
            shot_year = logFiles.last().mid(1,4).toInt();

            break;
        }
    }
    if(shot_year<0){
        m_errString = "Can not confirm the year of shot time.";
        return false;
    }
    //读取炮线坐标文件
    m_shotLineTimes = new ShotLineTimes;
    StationInfo *shotStationInfo   = m_areaDataInfo->shotStationInfo;
    m_shotLineTimes->lineNum       = shotStationInfo->lineNum;
    //qDebug()<<"line num:"<<m_shotLineTimes->lineNum;
    //每条线一个文件
    m_shotLineTimes->shotLineTimes = new ShotLineTime[shotStationInfo->lineNum];

    //读取每个文件，解析出炮时，坐标值从sps 读取，并且顺序对应
    int   line    = 0;
    foreach (QString shotLineFile, m_Parameter.shotLineFiles)
    {
        QFile shotFile(shotLineFile);

        if(!shotFile.open(QIODevice::ReadOnly))
        {
            m_errString= QString("Open shotLine file %1 failed.").arg(shotLineFile);
            return false;
        }

        //每条线的炮点数
        int stations = shotStationInfo->staLines[line].staNum;
        m_shotLineTimes->shotLineTimes[line].staNum    = stations;
        m_shotLineTimes->shotLineTimes[line].shotTimes = new QDateTime[stations];

        //根据实际存储的列修改
        QTextStream in(&shotFile);

        QString   dateTimeStr;
        QDate     date0(shot_year,1,1);
        QDate     date;
        QTime     time;
        int shot = 0;
        while(!in.atEnd())
        {
            dateTimeStr = in.readLine().split(QRegExp("\\s+"))[4];

            date        = date0.addDays(dateTimeStr.left(3).toInt()-1);
            time        = QTime::fromString(dateTimeStr.mid(3),"HHmmss.zzz");
            m_shotLineTimes->shotLineTimes[line].shotTimes[shot] = QDateTime(date,time);

            shot++;
        }
        shotFile.close();

        line ++;
    }
    return true;
}


//申请道内存-------------
bool RecvOrientedSegy::allocInitMemory()
{
    //文件句柄
    if(m_segyFileHandle)
    {
        delete m_segyFileHandle;
        m_segyFileHandle = 0;
    }

    m_segyFileHandle = new SegyFileHandles;
    //读取卷头信息 0~3200-----------------------------
    QString textheader = m_Parameter.textHeader;

    //截取3200字节
    textheader = textheader.left(3200);
    char       *binary3200 = new char[3200];
    memset(binary3200,0,3200);
    memcpy(binary3200,textheader.toLatin1().data(),textheader.size());


    //卷头信息-400 字节
    BinaryHead *binaryHead = new BinaryHead;
    binaryHead->nt   = m_Parameter.traceNs;//样点数
    //采样间隔
    binaryHead->mudt = 1000*m_Parameter.ds;
    binaryHead->samptype = 1;

    //SegyInfo
    SegyInfo   *segyInfo     = new SegyInfo;
    segyInfo->ns             = m_Parameter.traceNs;
    segyInfo->sampleInterval = 1000*m_Parameter.ds;
    segyInfo->traces       = 0;

    //handles,四分量文件
    m_segyFileHandle->segyInfo   = segyInfo;
    m_segyFileHandle->binary3200 = binary3200;
    m_segyFileHandle->binaryhead = binaryHead;

    //道头-----------------------------------------------
    m_traceHead = new TraceHead;
    memset(m_traceHead,0,sizeof(TraceHead));
    //根据采样点数和数据类型计算数据空间--------------
    int ns    = m_segyFileHandle->binaryhead->nt;
    int dType = m_segyFileHandle->binaryhead->samptype;
    int byteNum = 4;
    if(dType == 3)
        byteNum = 2;
    //四分量
    m_data      = new float[ns*4];

    return true;
}

QList<DepolyedDevice *> RecvOrientedSegy::checkEffectiveDevice()
{
    QList<DepolyedDevice *> effectiveDevices;
    //线和站点序号范围
    int startLine = m_Parameter.lineScope.first;
    int lastLine  = m_Parameter.lineScope.second;
    int startStation = m_Parameter.staScope.first;
    int lastStation  = m_Parameter.staScope.second;

    int lineNo,staNo;//从0开始
    DepolyedDevice *depolyedDevice;
    qDebug()<<"Check GOBS---------------------------------";
    for(int i =0;i<m_depolyedDevices.size();i++)//m_depolyedDevices.size()
    {
        depolyedDevice = m_depolyedDevices[i];
        //是否投放到站点
        if(depolyedDevice->station<1){
            qDebug()<<QString("%1 not depolyed").arg(depolyedDevice->device);//log
            continue ;
        }
        //判断是否有数据文件
        if(depolyedDevice->dataFile.isEmpty()){
            qDebug()<<QString("%1 no data").arg(depolyedDevice->device);//log
            continue;
        }
        //查找对应的站点
        if(findStation(depolyedDevice,&lineNo,&staNo)){
            //判断范围
            if((startLine<=lineNo && lineNo<=lastLine)
                    && (startStation<=staNo && staNo<=lastStation))
                effectiveDevices.append(depolyedDevice);
            else
                qDebug()<<QString("%1 out of scope.").arg(depolyedDevice->device);
        }else{
            qDebug()<<QString("%1 not found station.").arg(depolyedDevice->device);//log
        }
    }
    qDebug()<<QString("Check finished,total devices: %1, working devices:%2.\n\n")
              .arg(m_depolyedDevices.size())
              .arg(effectiveDevices.size());
    return effectiveDevices;
}

DataFileInfo *RecvOrientedSegy::readDataFileInfo(DepolyedDevice *depolyedDevice)
{
    bool ok = false;
    QString dataPath = m_Parameter.dataPath+ Dir_Separator + depolyedDevice->dataFile;
    DataFileInfo *dataFileInfo = new DataFileInfo;
    dataFileInfo->dataPath = dataPath;

    //1 钟差文件判断,读取-------------------
    {
        //读取Err 值
        ok = readTimeErrFile(QString("%1/%2").arg(dataPath,TimeErrFile)
                             ,dataFileInfo);
        if(!ok){
            m_errString = "Read "+depolyedDevice->dataFile+" TimeErr.log Failed.";
            delete dataFileInfo;
            return 0;
        }
    }

    //2 从DataFile.lst 截取有效数据文件段列表
    QString errStr;
    ok = getDataFiles(dataPath,dataFileInfo,&errStr);
    if(!ok){
        m_errString = depolyedDevice->dataFile+" check data files error,Error: "+errStr;
        delete dataFileInfo;
        return 0;
    }

    //2 读取log 文件获取钟差值
    {
        //读取log文件 TC值-----
        ok = readLogFile(dataFileInfo);
        //qDebug()<<"Tc:"<<QString::number(dataFileInfo->TC,'f',0);
        if(!ok){
            m_errString ="Read "+depolyedDevice->dataFile+" log file failed："+m_errString;
            delete dataFileInfo;
            return 0;
        }
    }

    return  dataFileInfo;
}

bool RecvOrientedSegy::readTimeErrFile(const QString &filename,DataFileInfo *dataFileInfo)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&file);
    QString     infoStr = in.readLine();//第一行
    QStringList strList = infoStr.split(QRegExp("\\s+"));

    //总的钟差
    dataFileInfo->timeErr =strList[3].remove("Err:").toDouble();//"Err:XXXX "

    file.close();

    return true;
}

bool RecvOrientedSegy::readLogFile(DataFileInfo *dataFileInfo)
{
    //找到采集起始文件时间对应的Log文件
    QString startDateStr = dataFileInfo->startTime.date().toString("yyyyMM");

    QString logFile = dataFileInfo->dataPath +
            QString("/A%1.log").arg(startDateStr);
    QFile file(logFile);
    if(!file.exists()){
        m_errString = "Log file not existed.";
        return false;
    }
    if(!file.open(QIODevice::ReadOnly)){
        m_errString = "Open log file failed.";
        return false;
    }
    QTextStream in(&file);
    //找到对应起始采样时间时的记录
    QString startStr = "!"+dataFileInfo->startTime.toString("yyyy-MM-dd HH:mm:ss")
            +" Enter Sampling Mode";
    while(!in.atEnd()){
        //找到起始行
        if(startStr == in.readLine())
        {
            QStringList strList = in.readLine().split(QRegExp("\\s+"));
            //TC 值
            dataFileInfo->TC = strList.last().remove("TC=").toDouble();
            file.close();
            return true;
        }
    }
    m_errString = "Log File Not find right TC value at start time.";
    return false;
}

bool RecvOrientedSegy::getDataFiles(const QString &dataPath,DataFileInfo *dataFileInfo,QString *errStr)
{
    //本地所有文件列表
    QDir        dir(dataPath);
    QStringList totalfiles = dir.entryList(QDir::NoDotAndDotDot|QDir::Files
                                           ,QDir::Name);
    //数据文件列表DataFile.lst
    QStringList exist_datafiles = totalfiles.filter(QRegExp(DataFileFilter));
    if(exist_datafiles.isEmpty())
    {
        *errStr = "Not found any dataFile.";
        return false;
    }
    QFile file(dataPath+Dir_Separator+DataFileList);
    if(!file.exists())
    {
        //判断是否有数据文件列表 文件
        *errStr = "Not found DataFile.lst file";
        return false;
    }
    if(!file.open(QIODevice::ReadOnly)){
        *errStr = "Open DataFile.lst failed.";
        return false;
    }
    QTextStream in(&file);
    //查找最后一个0001行，开始
    QStringList fileList = in.readAll().split("\n",QString::SkipEmptyParts);
    int idx0 = fileList.lastIndexOf(QRegularExpression("0001 .*"));
    if(idx0<0) //没有找到满足条件的文件系列
    {
        *errStr = "Not found start  data file.";
        file.close();
        return false;
    }

    //起始时间,正确的值，校正基准值，计算以后每个文件的时间
    QStringList strList = fileList[idx0].split(QRegExp("\\s+"));
    QDateTime fileTime  = QDateTime::fromString(QString("%1 %2")
                                                .arg(strList[1],strList[2].left(12))
            ,DateTime_Format_);
    //采样率
    dataFileInfo->frequency = strList[4].remove("sps").toInt();
    dataFileInfo->startTime = fileTime;
    QList<FileInfo*>        fileInfos;
    for(int i = idx0;i<fileList.size();i++){
        strList = fileList[i].split(QRegExp("\\s+"));
        //5栏信息
        if(strList.size()<5){
            *errStr = QString("DataFile.lst line %1 is Error.").arg(i);
            file.close();
            return false;
        }
        FileInfo *fileInfo = new FileInfo;
        fileInfo->fileName = strList[3];
        //真实的时间
        fileInfo->dateTime = fileTime;
        fileInfo->path     = dataPath;

        fileInfo->exist=QFile(dataPath + Dir_Separator +fileInfo->fileName).exists();
        fileInfos.append(fileInfo);
        //每次时间增加 120M 采样时间间隔
        fileTime = fileTime.addMSecs(Data_File_Sample_Size*(1000/dataFileInfo->frequency));
    }
    dataFileInfo->fileInfos = fileInfos;

    //计算文件总的样点数
    dataFileInfo->NTPS_all  = calTotalNs(dataPath,fileInfos);

    file.close();
    return true;
}

long long RecvOrientedSegy::calTotalNs(const QString &dataPath,const QList<FileInfo*> &fileInfos)
{
    //判断最后一个文件是否存在
    QFileInfo fileInf(dataPath+Dir_Separator+fileInfos.last()->fileName);
    if(fileInf.exists())
    {
        //判断最后一个文件大小，因为一般情况下最后个文件<120MB
        return Data_File_Msec_Size*(fileInfos.size()-1)+ fileInf.size()/12;//12B 为一次采样
    }else{
        //最后一个文件不存在默认总的采样点数 为文件数-1
        return Data_File_Msec_Size*(fileInfos.size()-1);
    }
}

//打开输出文件，初始化卷头信息
bool RecvOrientedSegy::openSegyFiles(DepolyedDevice *depolyedDevice)
{
    //确定文件名:线号_点号_设备编号_分量.segy
    QString prefixName = QString("%1/RecvLine%2_%3_%4")
            .arg(m_Parameter.outputPath)
            .arg(int(depolyedDevice->line),2,10,QChar('0'))
            .arg(depolyedDevice->station,3,10,QChar('0'))
            .arg(depolyedDevice->device);
    QStringList components;
    components<<"bhx"
             <<"bhy"
            <<"bhz"
           <<"hyd";
    //打开四个分量文件
    QString fileName;
    for(int i=0;i<4;i++)
    {
        //每个文件名字和目录，每台设备存放一个目录
        fileName = QString("%1_%2.%3").arg(prefixName,components[i],"segy");
        //qDebug()<<QString("open %1/%2.").arg(path,fileName);
        m_segyFileHandle->fileHandles[i].fileName = fileName;

        //打开文件
        int            ok = -1;
        int          mode = Write_Only;
        OpenTrace *openTr = gobs_open_segy_func(fileName.toLatin1().data()
                                                ,&mode,m_segyFileHandle->binary3200
                                                ,m_segyFileHandle->binaryhead
                                                ,m_segyFileHandle->segyInfo,&ok);
        if(ok!=0)
        {
            m_errString = QString("Open outputFile %1 failed,ret =%3.")
                    .arg(fileName)
                    .arg(ok);
            return false;
        }
        //保存文件fd
        m_segyFileHandle->fileHandles[i].openTrace = openTr;
    }

    return true;
}

bool RecvOrientedSegy::closeSegyFiles(bool success)
{
    int ok = -1;
    for(int i=0;i<4;i++){
        if(m_segyFileHandle->fileHandles[i].openTrace){
            gobs_close_segy_func(m_segyFileHandle->fileHandles[i].openTrace, &ok);
            if(ok!=0)
            {
                m_errString = QString("Close file %1 Failed.ok=%3.")
                        .arg(m_segyFileHandle->fileHandles[i].fileName)
                        .arg(ok);
                return false;
            }
        }
    }
    //如果处理失败，则删除生成的文件
    if(!success){
        for(int i=0;i<4;i++){
            QFile file(m_segyFileHandle->fileHandles[i].fileName);
            file.remove();
        }
    }
    return true;
}
//打开输入文件
bool RecvOrientedSegy::openInputFile(FileInfo*  fileInfo)
{
    fileInfo->fp = fopen(QString(fileInfo->path+Dir_Separator+fileInfo->fileName).toLatin1().data(),"rb");
    if(fileInfo->fp == NULL){
        return false;
    }
    //读取数据
    fileInfo->data = new unsigned char[Data_File_Size];
    fread(fileInfo->data,1,Data_File_Size,fileInfo->fp);
    return true;
}

void RecvOrientedSegy::closeInputFile(FileInfo* fileInfo)
{
    fclose(fileInfo->fp);
    fileInfo->fp = NULL;
    delete[] fileInfo->data;
    fileInfo->data = 0;
}

bool RecvOrientedSegy::mainRecvProcess(DataFileInfo *dataFileInfo)
{
    int isOk = -1;
    bool ok = false;
    QDateTime startDateTime,finalDateTime;//每个数据文件起始结束时间
    StationInfo   *shotStationInfo = m_areaDataInfo->shotStationInfo;
    QList<FileInfo*>  fileInfos    = dataFileInfo->fileInfos;

    //计算输入输出数据信息
    //输出数据ds(ms)-----------
    int out_ds = m_Parameter.ds;
    int out_ns = m_Parameter.traceNs;
    //一道数据长度,时间(msec)
    int msec   = out_ns*out_ds;
    int sps    = dataFileInfo->frequency;

    //输入数据长度---------
    int in_ds  = 1000/sps; //ds:ms
    int in_ns  = float(msec)*float(sps)/1000.0;
    float *in  = new float[in_ns*4]; //四个分量

    double  output_err;
    //遍历炮线,默认按照炮线方向，炮时递增的，所以文件向递增方向找
    int file_index = 0;m_traceHead->ep = 1;
    for(int line = 0;line<m_shotLineTimes->lineNum;line++)//m_shotLineTimes->lineNum
    {
        ShotLineTime shotLineTime = m_shotLineTimes->shotLineTimes[line];
        StaLine staLine = shotStationInfo->staLines[line];

        //遍历炮点,默认炮时是递增的
        for(int shot= 0;shot<shotLineTime.staNum;shot++){
            //使用以下两个备用字段用作抽取共炮文件使用
            //表示炮线序号和该条线炮点序号
            m_traceHead->unass[0] = line+1;
            m_traceHead->unass[1] = shot+1;
            //=============================
            //循环到最后一个文件，不再继续
            if(file_index == fileInfos.size()){
                if(fileInfos[file_index-1]->fp){
                    closeInputFile(fileInfos[file_index-1]);
                }
                delete[] in;
                in  = 0;
                return true;}
            for(;file_index<fileInfos.size();file_index++)
            {
                //判断文件是否存在
                if(!fileInfos[file_index]->exist)
                    continue;
                startDateTime = fileInfos[file_index]->dateTime;
                //结束时间，默认为文件是连续的
                if(file_index<fileInfos.size()-1 && fileInfos[file_index+1]->exist)
                    finalDateTime = fileInfos[file_index+1]->dateTime;
                else //最后一个有效文件
                    finalDateTime = startDateTime.addMSecs(Data_File_Sample_Size*(1000/dataFileInfo->frequency));

                if(startDateTime<=shotLineTime.shotTimes[shot] && shotLineTime.shotTimes[shot]<finalDateTime)
                {
                    //1 道头信息-炮点
                    Station shotStation = staLine.stations[shot];
                    {
                        m_traceHead->fldr   = shotStation.sp;

                        m_traceHead->sdepth = shotStation.sdepth;
                        m_traceHead->sdel   = shotStation.del;
                        m_traceHead->swdep  = shotStation.swdep;
                        m_traceHead->sx     = shotStation.x;
                        m_traceHead->sy     = shotStation.y;
                        //高程，相当于z值
                        m_traceHead->selev  = shotStation.elev;

                        //保存该道对应的炮时
                        m_traceHead->year = shotLineTime.shotTimes[shot].date().year();
                        m_traceHead->day  = shotLineTime.shotTimes[shot].date().dayOfYear();

                        m_traceHead->hour   = shotLineTime.shotTimes[shot].time().hour();
                        m_traceHead->minute = shotLineTime.shotTimes[shot].time().minute();
                        m_traceHead->sec    = shotLineTime.shotTimes[shot].time().second();
                    }

                    //2 根据炮时和文件信息计算钟差
                    double  total_err = calTimeErr(dataFileInfo,file_index,shotLineTime.shotTimes[shot]);
                    output_err = total_err;
                    if(fabs(total_err)>Max_Time_Err)
                    {
                        m_errString = QString("Clock error is too big,value=%1 sec.")
                                .arg(total_err);
                        delete[] in;
                        in  = 0;
                        return false;
                    }
                    //3 读取数据，插值，写入
                    {
                        memset(in,0,in_ns*4*4);

                        //对应数据的时间,精度到Msec
                        QDateTime  Ti_time = shotLineTime.shotTimes[shot].addMSecs(qRound(total_err*1000));

                        //读取输入数据
                        float xi0;//起始输入数据的坐标偏移(相对炮时)，单位ms
                        {
                            int  ns = in_ns;
                            //计算样点时间偏移(相对当前文件)
                            long long time_offset = fileInfos[file_index]->dateTime.msecsTo(Ti_time);
                            int sample_offset     = qRound(double(time_offset)*(double(sps)/1000.0));
                            //实际取数据的时间
                            QDateTime revisedTime = fileInfos[file_index]->dateTime.addMSecs(sample_offset*(1000/sps));
                            //数据和炮时偏差坐标
                            xi0 = revisedTime.time().toString("ss.zzz").toFloat()
                                    - Ti_time.time().toString("ss.zzz").toFloat();
                            //可能覆盖的文件采样点
                            int ns1=0,ns2=0,ns3=0;
                            if(sample_offset<0)
                            {
                                ns1 = abs(sample_offset);
                                ns1 = ns1>ns?ns:ns1;
                                if(file_index>0 && fileInfos[file_index-1]->exist){
                                    ok = readDataFromFile(fileInfos[file_index-1],Data_File_Sample_Size-ns1,ns1,in,in_ns);
                                    if(!ok){
                                        delete[] in;
                                        in  = 0;
                                        return false;
                                    }
                                }
                                ns -= ns1;
                            }
                            if(ns>0){
                                //当前文件数据范围，确定起点
                                sample_offset = sample_offset<0?0:sample_offset;
                                if(sample_offset<Data_File_Sample_Size){
                                    //最大长度
                                    ns2 = (Data_File_Sample_Size-sample_offset)>=ns?ns:(Data_File_Sample_Size-sample_offset);
                                    ok = readDataFromFile(fileInfos[file_index],sample_offset,ns2,in+ns1,in_ns);
                                    if(!ok){
                                        delete[] in;
                                        in  = 0;
                                        return false;
                                    }
                                    ns -= ns2;
                                }
                            }
                            if(ns>0){
                                //判断起始时间是否跳到下一个文件
                                sample_offset += ns2;
                                if(sample_offset >= Data_File_Sample_Size)
                                {
                                    //判断下一个文件
                                    if((file_index+1)<fileInfos.size() && fileInfos[file_index+1]->exist){
                                        sample_offset = sample_offset-Data_File_Sample_Size;
                                        ns3 = ns;
                                        ok = readDataFromFile(fileInfos[file_index+1],sample_offset,ns3,in+(ns1+ns2),in_ns);
                                        if(!ok){
                                            delete[] in;
                                            in  = 0;
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                        //插值
                        for(int i=0;i<4;i++){
                            gobs_1D_interpolation_regular(in+i*in_ns,in_ns,xi0,in_ds,
                                                          m_data+i*out_ns,out_ns,0,out_ds,
                                                          0,0,LINEARINTER,&isOk);
                            if(isOk!=0)
                            {
                                delete[] in;
                                in  = 0;
                                m_errString = "Interpolation failed.";
                                return false;
                            }
                        }
                        //数据写入
                        for(int i=0;i<4;i++){
                            gobs_write_segy_func(m_segyFileHandle->fileHandles[i].openTrace,m_traceHead,(char*)(m_data+i*out_ns),&isOk);
                            if(isOk !=0 )
                            {
                                delete[] in;
                                in  = 0;
                                m_errString = "Write trace data failed.";
                                return false;
                            }
                        }

                        m_traceHead->tracf ++;
                        m_traceHead->tracl ++;

                    }
                    break;
                }

                else//炮时超过当前文件，则需要将上一个文件关闭清除数据
                {
                    if(file_index-2>=0 && fileInfos[file_index-2]->fp){
                        closeInputFile(fileInfos[file_index-2]);
                    }
                    //炮时低于数据文件时间
                    if(shotLineTime.shotTimes[shot]<startDateTime)
                        break;
                }
            }

            m_traceHead->ep ++;
        }

    }
    qDebug()<<"Clock error is "<<output_err<<" sec.";

    delete[] in;
    in  = 0;
    return true;
}

double RecvOrientedSegy::calTimeErr(DataFileInfo *dataFileInfo,const int &file_index,const QDateTime &shotTime)
{
    QList<FileInfo*> fileInfos    =  dataFileInfo->fileInfos;

    int            sps = dataFileInfo->frequency;
    double         TC  = dataFileInfo->TC;
    long long NTPS_all = dataFileInfo->NTPS_all;
    double         Err = dataFileInfo->timeErr;

    //计算钟差----------------------------------
    //起始文件的样点大小
    long long offset_sample0 = file_index*Data_File_Sample_Size;
    //计算炮时 所有对应的数据文件位置偏移
    FileInfo *start_fileInf   = fileInfos[file_index];

    //文件中偏移的msec,粗略精确到msec
    long long offset_msec = start_fileInf->dateTime.msecsTo(shotTime);
    long long NTPS_ti     = offset_sample0+double(offset_msec)/1000.0*double(dataFileInfo->frequency);

    //TC Err,晶振主频 偏离引起的累计误差
    double TC_err = ((TC/256.0-PCLK0)/PCLK0)*double(NTPS_ti)/double(sps);

    //总钟差线性分布在总事件段内//sec
    double Ti_err     = Err*(double(NTPS_ti)/double(NTPS_all));
    double total_err  = Ti_err + TC_err + 18.0/double(sps);

    return total_err;
}

bool RecvOrientedSegy::readDataFromFile(FileInfo *fileInfo,int sample_offset,int samples,float *in,int ins)
{
    //判断文件是否打开，打开文件读取数据
    if(!fileInfo->fp){
        if(!openInputFile(fileInfo))
        {
            m_errString = QString("Open %1 failed.").arg(fileInfo->fileName);
            return false;
        }
    }
    //samples 实际读取数据量,ins 总的需要读取数据量
    unsigned char buf[3];
    int offset = 12*sample_offset;
    unsigned char *data = fileInfo->data+offset;
    for(int i=0;i<samples;i++){
        memcpy(buf,data+12*i,3);
        in[i] = swap3(buf);

        memcpy(buf,data+12*i+3,3);
        in[ins*1+i] = swap3(buf);

        memcpy(buf,data+12*i+6,3);
        in[ins*2+i] = swap3(buf);

        memcpy(buf,data+12*i+9,3);
        in[ins*3+i] = swap3(buf);
    }
    return true;
}

Station RecvOrientedSegy::findStation(const float &line,const int &station) const
{
    StationInfo *recvStationInfo = m_areaDataInfo->recvStationInfo;
    for(int i = 0;i<recvStationInfo->lineNum;i++)
        if(line == recvStationInfo->staLines[i].line){
            StaLine staLine = recvStationInfo->staLines[i];

            for(int j=0;j<staLine.staNum;j++)
                if(staLine.stations[j].sp == station)return staLine.stations[j];
        }
    return Station();
}
bool RecvOrientedSegy::findStation(DepolyedDevice *depolyedDevice,int *lineNo, int *staNo)
{
    StationInfo *recvStationInfo = m_areaDataInfo->recvStationInfo;
    for(int i = 0;i<recvStationInfo->lineNum;i++)
        if(depolyedDevice->line == recvStationInfo->staLines[i].line){
            StaLine staLine = recvStationInfo->staLines[i];
            for(int j=0;j<staLine.staNum;j++)
                if(staLine.stations[j].sp == depolyedDevice->station){
                    *lineNo = i+1;
                    *staNo = j+1;
                    depolyedDevice->recvStation = staLine.stations[j];
                    return true;
                }
        }
    return false;
}

