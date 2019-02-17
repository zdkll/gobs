#include "datamgrcontrol.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataMgrControl w;

    QString icon = QApplication::applicationDirPath()
            + "/../icon/module_dmc.png";
    a.setWindowIcon(QIcon(icon));

    ModuleBuilder::buildModule(argc,argv,w);
    w.show();

    return a.exec();
}
