#ifndef GPOSITIONSYSTEM_H
#define GPOSITIONSYSTEM_H

#include <QWidget>
#include "gpositionsystem_global.h"
#include "gpspublic.h"

namespace Ui {
class GPositionSystem;
}

class GSerialPort;
class QPushButton;
class DeckUnit;
class QTextBrowser;
class PositionChart;
class  GPOSTIONSYTEMSHARED_EXPORT GPositionSystem : public QWidget
{
    Q_OBJECT

public:
    explicit GPositionSystem(QWidget *parent = 0);
    ~GPositionSystem();

    void showText(const QString& text);
protected slots:
    void startPostion();
    void stopPostion();

    //接收到GPS 甲板定位
    void slotRecvGpsCord(const GpsCoord& cord);

private:
    void createWg();

    Ui::GPositionSystem *ui;
    GSerialPort   *m_gSerialPort;

    DeckUnit        *m_deckUnit;
    PositionChart *m_positionChart;

    //=============================
    QPushButton  *m_startBtn,*m_stopBtn;
    QTextBrowser *m_textBrowser;
//    ShowTextFunc m_showTxtFunc;
};

#endif // GPOSITIONSYSTEM_H
