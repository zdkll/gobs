/********************************************************************
created:
author: dk.zhang
purpose:项目 工区操作类
*********************************************************************/
#ifndef PROJECTAREA_H
#define PROJECTAREA_H

#include <QtCore>
#include "public.h"
#include "core_global.h"

//Project，项目操作--------------------------------------------------------
class  CORESHARED_EXPORT Project
{
public:
    Project(){}

   static QString projName(const ProjectInfo &);

    //创建项目文件----
    static int createProject(const ProjectInfo &);
    //检查项目文件，Id是否正确
    static int checkProject(const ProjectInfo &projInfo,QString *errorStr = 0);

    //查询Project 信息
    static QJsonValue getProjectInfo(const ProjectInfo &,const QString &key,int *ok = 0,QString *errorStr = 0);

    //修改Project 信息
    static int updateProjectInfo(const ProjectInfo &,const QString &key,const QJsonValue &value,QString *errorStr = 0);

    //获取完成下载的GOBS设备列表
    static QStringList finishedDownloadGOBS(const ProjectInfo &projInfo);

    //某台GOBS下载完成，参数为GOBS编号
    static int appendDownloadGOBS(const ProjectInfo &projInfo,const QString &gobs);
};

/*-----------------------------------------------------------
 *工区操作
 */
class CORESHARED_EXPORT Area
{
public:
    Area(){}
    //创建工区，工区Area文件夹(保存在pro同级目录)，包括拷贝sps文件，保存工区信息到area文件等
    static int createArea(const ProjectInfo &,const QString &areaName,const QStringList &spsFiles);

    static int updateArea(const ProjectInfo &,const QString &areaName
                          ,const QStringList &spsFiles,int option);

    static int updateReaplceArea(const ProjectInfo &,const QString &areaName
                                 ,const QStringList &spsFiles);
    static int updateMergeArea(const ProjectInfo &,const QString &areaName
                               ,const QStringList &spsFiles);

    static void calShotLineInfo(const QStringList &lineList,QMap<QString,int> &shotLineInfo);
    //项目添加工区
    static int addArea(const ProjectInfo &,const QString &areaName);
    //获取项目工区 - area list
    static  QStringList areasFromProject(const QString &projFile);

    //删除工区
    static int deleteArea(const ProjectInfo &,const QString &areaName);

    //读取工区sps 文件 Stringlist
    static  QStringList spsFiles(const ProjectInfo &,const QString &areaName);

    static  QStringList spsFiles1(const QString &projPath,const QString &areaName);
};

#endif // PROJECTAREA_H
