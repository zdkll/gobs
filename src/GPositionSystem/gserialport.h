#ifndef GSERIALPORT_H
#define GSERIALPORT_H

#include <QWidget>
#include <QSerialPort>

#include "gpspublic.h"

namespace Ui {
class GSerialPort;
}

class GSerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit GSerialPort(QWidget *parent = 0);
    ~GSerialPort();
    bool open();
    void close();

    bool isOpen(){return m_serialPort->isOpen();}
    QSerialPort *serialPort(){return m_serialPort;}

    void setShowTxtFunc(ShowTextFunc func);
signals:
    void recvGpsCord(const GpsCoord& );
    void gpsErrorOccurs();

protected slots:
    void slotReadyRead();

private:
    void initWg();
    GpsCoord GPGGAStr2Cord(const QString& in_str);

    Ui::GSerialPort *ui;
    QSerialPort *m_serialPort;
    QByteArray  m_byteArray;
    QTimer   *m_timer = Q_NULLPTR;
    ShowTextFunc m_showTxtFunc;

    static QMap<int,QSerialPort::BaudRate>   mpBaudRate;
    static QMap<int,QSerialPort::DataBits>     mpDatabit;
    static QMap<int,QSerialPort::Parity>         mpParity;
    static QMap<int,QSerialPort::StopBits>     mpStopBit;
    static QMap<int,QSerialPort::FlowControl>   mpFlowControl;

};

#endif // GSERIALPORT_H
