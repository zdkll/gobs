#include "gpositionsystem.h"
#include <QApplication>

#include "gserialport.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GPositionSystem w;
    w.show();

    return a.exec();
}
