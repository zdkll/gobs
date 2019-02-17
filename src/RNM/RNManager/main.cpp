#include "rnmanager.h"
#include <QApplication>

#include <QTime>
#include <QHostAddress>
#include <qftp.h>

#include "nodequery.h"
#include "nodeftp.h"
#include "rnftpmanager.h"

#include <signal.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RNManager w;

    QString icon = QApplication::applicationDirPath()
            + "/../icon/module_rnm.png";
    a.setWindowIcon(QIcon(icon));
    //构建模块
    if(ModuleBuilder::buildModule(argc,argv,w)!=0)
        return -1;
    w.show();

    //qInstallMessageHandler(outputMessage);

    //test
    //LimitsDialog dlg;
    //dlg.show();

    //    StatusReportDlg dlg;
    //    dlg.show();

    //    Node     node;
    //    node.ip  = QHostAddress("192.168.4.100").toIPv4Address();

    //    NodeItem item(&node);
    //    NodeFtp  nodeFtp(&item);
    //    nodeFtp.queryFiles();

    //        RNFtpManager manager;
    //        manager.show();

//   qDebug()<< dataFile2Time("4246B925.175"
//                            "",3145728772);

    return a.exec();
}
