#include "linemanager.h"
#include <QApplication>

#include "deploysystem.h"
#include "assignnodedlg.h"
#include "rndeployments.h"

#include <QTime>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LineManager w;

    QString icon = QApplication::applicationDirPath()
            + "/../icon/module_lmgr.png";
    a.setWindowIcon(QIcon(icon));
    //构建模块
    if(ModuleBuilder::buildModule(argc,argv,w)!=0)
        return -1;

    w.show();


    //test
    //    DeploySystem deploySys;
    //    deploySys.show();

    //test
    //    IpAddrEdit ipEdit;
    //    ipEdit.show();

    //    ProjectInfo projInfo;
    //    projInfo.ProjectPath = QString(argv[1]);
    //    projInfo.ProjectName = QString(argv[2]);

    //    SqlFunctions *sqlFunc = SqlFunctions::instance();
    //    sqlFunc->openDataBase(projInfo.ProjectPath,projInfo.ProjectName);

    //        QList<ResultInfo> retInfos;
    //        ResultInfo retInfo;
    //        retInfo.fieldName = TABLE_FIELD_DEVICE;
    //        retInfos.append(retInfo);

    //        retInfo.fieldName = TABLE_FIELD_AREA;
    //        retInfo.fieldValue = "area1";
    //        retInfos.append(retInfo);

    //        retInfo.fieldName = TABLE_FIELD_IP;
    //        retInfo.fieldValue = "192.168.1.1x";
    //        retInfos.append(retInfo);
    //        for(int i = 1; i<30 ;i++)
    //        {
    //            retInfos[0].fieldValue = QString("G%1").arg(i,3,10,QChar('0'));
    //            sqlFunc->insert("area1",retInfos);
    //        }

    //    AssignNodeDlg w;
    //    w.setProjInfo(projInfo);
    //    w.initDepolyments();
    //    w.show();

    //    EditCoordDlg dlg;
    //    dlg.show();

    return a.exec();
}
