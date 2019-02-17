#include "commmodule.h"
#include <QMessageBox>

CommModule::CommModule()
{

}

bool CommModule::initArgs(int argc, char **argv)
{
    //测试用，参数少的情况
    if(argc<3)
    {
        //        QMessageBox::warning(NULL,"error","Args are too less!");
        return false;
    }

    m_projPath = QString(argv[1]);
    m_projName = QString(argv[2]);
    qDebug()<<"path:"<<m_projPath<<" name:"<<m_projName;
    m_projInfo = ProjectInfo(m_projPath,m_projName);
    return true;
}

bool CommModule::CheckProject(QString *errorStr)
{
    //1 判断gpro文件是否存在
 // QString project_file = m_projPath+Dir_Separator+m_projName+Project_File_Suffix;
    if(Project::checkProject(m_projInfo,errorStr) != 0){
        return false;
    }
    return true;
}



//ModuleBuilder---------------------------------------
int ModuleBuilder::buildModule(int argc,char **argv,CommModule &module)
{
    try{
        //初始化参数
        if(!module.initArgs(argc,argv))
            throw("Init args error.");

        //检查项目
        if(!module.CheckProject())
            throw("Check project error.");

        //模块初始化
        if(!module.init())
            throw("Init module error.");
        //--------------------
    }
    catch(const char *err)
    {
        QMessageBox::critical(NULL,"error",err);
        return -1;
    }

    return 0;
}
