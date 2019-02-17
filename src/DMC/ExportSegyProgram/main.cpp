#include <QtCore>
#include <QCoreApplication>
#include <QDebug>

#include "gpublic.h"
#include "recvorientedsegy.h"
#include "shotorientedsegy.h"

bool readParameters(int argc,char **argv,ExportSegyParameter &parameter);
//抽取道集主程序
int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);
    //解析参数
    ExportSegyParameter parameter;
    if(!readParameters(argc,argv,parameter))
    {
        throw "Read job parameters error.";
        exit(-1);
    }
    AbstractSegyProducer *producer;

    //任务类型-------------------
    if(parameter.segyType == 0){
        producer = new RecvOrientedSegy;
    }
    else if(parameter.segyType == 1)
        producer = new ShotOrientedSegy;

    //设置参数
    producer->setParameter(parameter);
    //启动
    producer->start();

    delete producer;
    //return a.exec();
    return 0;
}

bool readParameters(int argc,char **argv,ExportSegyParameter &parameter)
{
    if(argc<2)
        return false;
    // 作业文件，解析参数
    QString jobFile = QString(argv[1]);
    QFile file(jobFile);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if(doc.isNull()){
        return false;
    }
    //初始化参数
    QJsonObject jsobj = doc.object();
    parameter.segyType    = jsobj.value(Json_Key_JobType).toInt();
    parameter.jobName     = jsobj.value(Json_Key_JobName).toString();
    parameter.projectPath = jsobj.value(Json_Key_ProjectPath).toString();
    parameter.projectName = jsobj.value(Json_Key_ProjectName).toString();

    parameter.areaName    = jsobj.value(Json_key_AreaName).toString();
    parameter.dataPath    = jsobj.value(Json_key_DataPath).toString();
    parameter.outputPath  = jsobj.value(Json_key_OutputPath).toString();

    QString lineScopeStr      = jsobj.value(Json_key_LineScope).toString();
    parameter.lineScope.first = lineScopeStr.split("|").at(0).toInt();
    parameter.lineScope.second= lineScopeStr.split("|").at(1).toInt();
    QString staScopeStr       = jsobj.value(Json_key_StationScope).toString();
    parameter.staScope.first = staScopeStr.split("|").at(0).toInt();
    parameter.staScope.second= staScopeStr.split("|").at(1).toInt();

    //共检波点
    if(parameter.segyType == 0){
        parameter.traceNs     = jsobj.value(Json_key_NS).toInt();
        parameter.ds          = jsobj.value(Json_key_DS).toInt();

        parameter.textHeader  = jsobj.value(Json_key_SegyTextHeader).toString();

        QJsonArray  shotLineArray = jsobj.value(Json_key_ShotLineFiles).toArray();
        for(int i =0;i<shotLineArray.size();i++)
            parameter.shotLineFiles<<shotLineArray[i].toString();
    }

    file.close();

    return true;
}

