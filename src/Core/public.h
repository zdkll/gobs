#ifndef PUBLIC_H
#define PUBLIC_H

#include <QtCore>
#include <QJsonDocument>

#include "macro.h"
#include "segy.h"

//qunit32 ,IP地址值范围
typedef QPair<uint,uint> IPScope;

//Host
//typedef  QPair<uint,bool> HostConnect;
//typedef  QVector<HostConnect> Hosts;

//节点显示状态
enum Node_Status
{
    Status      = 0,//链接状态
    Voltage     = 1,
    Temperature = 2,
    Pressure    = 3,
//    EleCurr     = 4,
//    ChargeVolt  = 5,
    Memory_Capacity  = 4,
    SelfTest    = 5,   //所有参数自检状态
    DownLoad    = 6
};

//typedef QPair<QVariant,QColor>  Limit;
//typedef QList<Limit>            Limits;

//struct LimitInfo
//{
//    QVariant Type;  //数据类型
//    QString  format;//显示类型
//    Limits   limits;//阀值-颜色信息
//};

struct TaskCount
{
    uint start;
    uint step;
    uint end;
};

//工区信息
struct ProjectInfo
{
    ProjectInfo(){}
    ProjectInfo(const QString &projPath,const QString &projName)
        :ProjectPath(projPath),ProjectName(projName){}
    QString ProjectPath;
    QString ProjectName;
};

//输出Segy 文件参数结构体
struct ExportSegyParameter
{
    int       segyType;//0：共接收点；1:共炮点
    QString   jobName; //相关文件保存在 work 目录下

    QString   projectPath;
    QString   projectName;
    QString   areaName;
    int       traceNs;
    int       ds;     //采样点时间间隔 ms

    QString   dataPath;
    QString   outputPath;

    //卷头3200 字节 text
    QString     textHeader;
    QStringList shotLineFiles;

    QPair<int,int> lineScope;
    QPair<int,int> staScope;
};

//文件句柄信息
struct SegyFileHandle
{
    SegyFileHandle()
        :openTrace(0)
        ,binary3200(0)
        ,binaryhead(0)
        ,segyInfo(0){}

    ~SegyFileHandle(){
        clear();
    }
    void clear(){
        if(binary3200){
            delete binary3200;
            binary3200 = 0;
        }
        if(binaryhead) {
            delete binaryhead;
            binaryhead = 0;
        }
        if(segyInfo) {
            delete segyInfo;
            segyInfo = 0;
        }
        openTrace = 0;
    }

    QString    fileName;
    OpenTrace  *openTrace;

    char       *binary3200;
    BinaryHead *binaryhead;
    SegyInfo   *segyInfo;
};

struct FileHandle
{
    FileHandle():openTrace(0){}
    QString     fileName;
    OpenTrace  *openTrace;
};

struct SegyFileHandles
{
    SegyFileHandles():
        binary3200(0)
      ,binaryhead(0)
      ,segyInfo(0){}

    ~SegyFileHandles(){
        clear();
    }
    void clear(){
        if(binary3200){
            delete[] binary3200;
            binary3200 = 0;
        }
        if(binaryhead) {
            delete binaryhead;
            binaryhead = 0;
        }
        if(segyInfo) {
            delete segyInfo;
            segyInfo = 0;
        }
    }
    FileHandle fileHandles[4];

    char       *binary3200;
    BinaryHead *binaryhead;
    SegyInfo   *segyInfo;
};

//Project，项目操作--------------------------------------------------------
namespace Project {
extern "C" QString projName(const ProjectInfo &);
//创建项目文件----
extern "C" int createProject(const ProjectInfo &);
//检查项目文件，Id是否正确
extern "C" int checkProject(const ProjectInfo &projInfo,QString *errorStr = 0);

//查询Project 信息
extern "C" QJsonValue getProjectInfo(const ProjectInfo &,const QString &key,int *ok = 0,QString *errorStr = 0);

//修改Project 信息
extern "C" int updateProjectInfo(const ProjectInfo &,const QString &key,const QJsonValue &value,QString *errorStr = 0);

//获取完成下载的GOBS设备列表
extern "C" QStringList finishedDownloadGOBS(const ProjectInfo &projInfo);

//某台GOBS下载完成，参数为GOBS编号
extern "C" int appendDownloadGOBS(const ProjectInfo &projInfo,const QString &gobs);

}

/*-----------------------------------------------------------
 *工区操作
 */
namespace Area {
//创建工区，工区Area文件夹(保存在pro同级目录)，包括拷贝sps文件，保存工区信息到area文件等
extern "C" int createArea(const ProjectInfo &,const QString &areaName,const QStringList &spsFiles);

extern "C" int updateArea(const ProjectInfo &,const QString &areaName
                          ,const QStringList &spsFiles,int option);

extern "C" int updateReaplceArea(const ProjectInfo &,const QString &areaName
                          ,const QStringList &spsFiles);
extern "C" int updateMergeArea(const ProjectInfo &,const QString &areaName
                          ,const QStringList &spsFiles);

extern "C" void calShotLineInfo(const QStringList &lineList,QMap<QString,int> &shotLineInfo);
//项目添加工区
extern "C" int addArea(const ProjectInfo &,const QString &areaName);
//获取项目工区 - area list
QStringList areasFromProject(const QString &projFile);

//删除工区
extern "C" int deleteArea(const ProjectInfo &,const QString &areaName);

//读取工区sps 文件 Stringlist
extern "C"  QStringList spsFiles(const ProjectInfo &,const QString &areaName);

extern "C"  QStringList spsFiles1(const QString &projPath,const QString &areaName);

}



#endif // PUBLIC_H

