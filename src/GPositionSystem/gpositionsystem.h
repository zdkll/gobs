#ifndef GPOSITIONSYSTEM_H
#define GPOSITIONSYSTEM_H

#include <QWidget>
#include "gpositionsystem_global.h"
#include "public.h"

namespace Ui {
class GPositionSystem;
}

class GSerialPort;
class QPushButton;
class  GPOSTIONSYTEMSHARED_EXPORT GPositionSystem : public QWidget
{
    Q_OBJECT

public:
    explicit GPositionSystem(QWidget *parent = 0);
    ~GPositionSystem();

protected slots:
    void startPostion();
    void stopPostion();

    void slotRecvGpsCord(const GpsCoord& cord);

private:
    void createWg();

    Ui::GPositionSystem *ui;
    GSerialPort  *m_gSerialPort;

private:
    QPushButton *m_startBtn,*m_stopBtn;
};

#endif // GPOSITIONSYSTEM_H