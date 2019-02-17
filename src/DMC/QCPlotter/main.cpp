#include "qcplotter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCPlotter w;
    w.show();

    return a.exec();
}
