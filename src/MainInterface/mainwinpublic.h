#ifndef MAINWINPUBLIC_H
#define MAINWINPUBLIC_H

#define WindowTile_Project "GOBS|Project:"

#define Icon_Module_LMGR   "/icon/module_lmgr"
#define Icon_Module_RNM    "/icon/module_rnm"
#define Icon_Module_DMC    "/icon/module_dmc"

#define Icon_New_Project   "/icon/new_project"
#define Icon_Open_Project  "/icon/open_project"

//历史项目记录,存放在根目录下projects文件
#define History_Projects_File "/projects"

//当前默认的工作目录，即打开和创建工区目录
#define Current_WorkPath "/workpath"



#include <QtCore>
#include <QApplication>

#include "gcore.h"

using namespace Core;


class MainWinPublic
{
public:
    MainWinPublic();

    //root
    static const QString rootPath();
    //bin 目录
    static const QString appPath()
    {
        return QApplication::applicationDirPath();
    }

    //工作目录
    static const QString workPath();

    //保存工作目录
    static void saveWorkPath(const QString &Path);
};


#endif // MAINWINPUBLIC_H
