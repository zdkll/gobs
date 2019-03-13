#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>

#include "logindlg.h"
#include "gpositionsystem.h"

using namespace Core;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //1 创建程序数据存放目录
    if(!Core::createDocDir()){
        QMessageBox::warning(NULL,"Error","Create Documents File Error.");
        exit(1);
    }
    //    QString icon = QApplication::applicationDirPath()
    //            + "/../icon/gobs.png";
    //    a.setWindowIcon(QIcon(icon));

    //登陆界对话框
    //    LoginDlg dlg;

    //    int ret;
    //    if(dlg.exec()){
    //        MainWindow w;
    //        w.show();
    //      int  ret = a.exec();

    GPositionSystem w;

    w.show();

    //    }

    return a.exec();
}
