#include "abstractsegyproducer.h"

#include "gsqlfunctions.h"


QFile *AbstractSegyProducer::m_logFile  = 0;

AbstractSegyProducer::AbstractSegyProducer()
{
    m_areaDataInfo = 0;
}

AbstractSegyProducer::~AbstractSegyProducer()
{
    if(m_logFile)
        delete m_logFile;

    if(m_areaDataInfo){
        m_areaDataInfo->clearData();
        delete m_areaDataInfo;
        m_areaDataInfo = 0;
    }
}

//初始化参数和环境
bool AbstractSegyProducer::init()
{
    //1 初始化日志
    initLog(m_Parameter.projectPath,m_Parameter.jobName);
    qDebug()<<"init args...";//log

    //2 打开数据库
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    bool ok = sqlFunc->openDataBase(m_Parameter.projectPath
                                    ,m_Parameter.projectName);
    if(!ok){
        m_errString = "Open Database Error.";
        return false;
    }

    return true;
}

//开始运行
void AbstractSegyProducer::start()
{
    try{
        if(!init())
            throw "Init ERROR";

        if(!preProcesss())
            throw "PreProcess ERROR";

        if(!run())
            throw "Run ERROR";

    }
    catch(const char *exp)
    {
        qDebug()<<QString(exp)+",errString:"<<m_errString;
        finalize();
        exit(1);
    }

    //结束工作，关闭文件等---
    finalize();
    qDebug()<<"Finished success.";
}
bool AbstractSegyProducer::finalize()
{
    //关闭数据库
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    sqlFunc->closeDataBase();

    //2 日志记录完成
    qDebug()<<"finalize----"; //log
    return true;
}

bool AbstractSegyProducer::initLog(const QString &projPath,const QString &jobName)
{
    QString logPath = projPath+Dir_Separator+"work/";
    QString logFile = logPath+jobName+".log";
    //日志文件
    qDebug()<<"log file:"<<logFile;
    m_logFile = new QFile(logFile);

    qInstallMessageHandler(AbstractSegyProducer::myMsgHandler);
    return true;
}

void AbstractSegyProducer::myMsgHandler(QtMsgType /*qtMsgType*/
                                        ,const QMessageLogContext &
                                        , const QString &context)
{
    m_logFile->open(QIODevice::WriteOnly|QIODevice::Append);

    m_logFile->write(context.toLatin1().data(),context.size());
    m_logFile->write("\n",1);

    m_logFile->close();
}

bool AbstractSegyProducer::readSpsFiles()
{
    //1 根据项目和工区 读取sps，炮点，检波点数据以及炮检点关系文件
    //关系文件以检波点站点为索引

    //获取sps文件
    QStringList spsFiles = Area::spsFiles1(m_Parameter.projectPath,m_Parameter.areaName);
    if(spsFiles.size()<1){
        m_errString = "SPS Files are not right";
        return false;
    }

    //读取所有的sps文件
    if(m_areaDataInfo)
        m_areaDataInfo->clearData();
    else
        m_areaDataInfo = new AreaDataInfo;

    QString errString;
    int ok = gobs_sps_files_read(spsFiles,m_areaDataInfo,&errString);
    if(ok != 0){
        m_errString = "read sps files failed";
        return false;
    }
    //结果查看
    //    RelationInfo *relationInfo = m_areaDataInfo->relationInfo;
    //    Relation *realtions = relationInfo->relations;

    //qDebug test
    //    qDebug()<<"relation num:"<<relationInfo->relationNum;
    //    for(int i=0;i<relationInfo->relationNum;i++)
    //    {
    //        qDebug()<<"shot line:"<<realtions[i].shotLine
    //               <<"shot no:"<<realtions[i].ep
    //              <<"first trace:"<<realtions[i].firstTrace
    //             <<"last trace:"<<realtions[i].lastTrace
    //            <<"recv line:"<<realtions[i].recvLine
    //           <<"first station:"<<realtions[i].firstRecv
    //          <<"last station:"<<realtions[i].lastRecv;
    //    }

    return true;
}

