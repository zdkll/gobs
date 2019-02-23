#include "gserialport.h"
#include "ui_gserialport.h"

#include "geo2xy_utm.h"

#define GPGGA_STRING  "$GPGGA"

#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>

QMap<int,QSerialPort::BaudRate>   GSerialPort::mpBaudRate =
        QMap<int,QSerialPort::BaudRate>({{0,QSerialPort::Baud9600},{1,QSerialPort::Baud115200}});
QMap<int,QSerialPort::DataBits>     GSerialPort::mpDatabit =
        QMap<int,QSerialPort::DataBits>({{0,QSerialPort::Data5},{1,QSerialPort::Data6},{2,QSerialPort::Data7},{3,QSerialPort::Data8}});
QMap<int,QSerialPort::Parity>         GSerialPort::mpParity =
        QMap<int,QSerialPort::Parity>({{0,QSerialPort::NoParity},{1,QSerialPort::OddParity},{2,QSerialPort::EvenParity},{3,QSerialPort::MarkParity},{4,QSerialPort::SpaceParity}});
QMap<int,QSerialPort::StopBits>     GSerialPort::mpStopBit =
        QMap<int,QSerialPort::StopBits>({{0,QSerialPort::OneStop},{1,QSerialPort::OneAndHalfStop},{2,QSerialPort::TwoStop}});
QMap<int,QSerialPort::FlowControl>   GSerialPort::mpFlowControl =
        QMap<int,QSerialPort::FlowControl>({{0,QSerialPort::NoFlowControl},{1,QSerialPort::HardwareControl},{2,QSerialPort::SoftwareControl}});

GSerialPort::GSerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSerialPort)
  ,m_serialPort(new QSerialPort(this))
{
    ui->setupUi(this);
    initWg();

    connect(m_serialPort,&QSerialPort::readyRead,this,&GSerialPort::slotReadyRead);
}

GSerialPort::~GSerialPort()
{
    delete ui;
}

bool GSerialPort::open()
{
    m_serialPort->setPortName(ui->SerialNumCbx->currentText());
    m_serialPort->setBaudRate(mpBaudRate.value(ui->baudRateCbx->currentIndex()));
    m_serialPort->setDataBits(mpDatabit.value(ui->dataBitCbx->currentIndex()));
    m_serialPort->setStopBits(mpStopBit.value(ui->stopBitCbx->currentIndex()));
    m_serialPort->setParity(mpParity.value(ui->parityCbx->currentIndex()));
    m_serialPort->setFlowControl(mpFlowControl.value(ui->flowControlCbx->currentIndex()));

    //    return  m_serialPort->open(QIODevice::ReadWrite);

    //模拟---begin-----
    if(!m_timer){
        m_timer = new QTimer(this);
        connect(m_timer,&QTimer::timeout,this,&GSerialPort::slotReadyRead);
    }
    m_timer->start(1000);
    return true;
    //模拟---end-----
}
void GSerialPort::close()
{
    m_serialPort->flush();
    m_serialPort->close();
}

void GSerialPort::slotReadyRead()
{
    //    m_byteArray += m_serialPort->readAll();

    //模拟---begin-----
    // 080944.00
    m_byteArray = QString("$GPGGA,%1.00,3117.70409,N,12131.91747,E,1,02,2.49,48.3,M,9.9,M,,*5D\r\n")
            .arg(QDateTime::currentDateTimeUtc().time().toString("HH:mm:ss")).toLocal8Bit();
    //模拟---end-----

    QString text = QString(m_byteArray);

    int index = text.indexOf(GPGGA_STRING);
    if(index>=0){
        //找到“$GPGGA”，去头
        if(index>0)
            m_byteArray = m_byteArray.mid(index);

        //是否包含时间和坐标信息
        text = QString(m_byteArray);
        //回车换行符-消息
        index = text.indexOf("\r\n");
        if(index >= 0){
            QString gpgga_string = text.left(index+1);

            //获取地球坐标
            GpsCoord cord =  GPGGAStr2Cord(gpgga_string);
            //计算物理坐标
            emit recvGpsCord(cord.toXy());

            //移除该段
            m_byteArray.mid(index+1);
        }
    }
}

GpsCoord GSerialPort::GPGGAStr2Cord(const QString& in_str)
{
    qDebug()<<"recv gpgga string:"<<in_str;
    GpsCoord gpsCord;
    QString ret_str = in_str.mid(7);
    int idx = 0;
    do{
        int index = ret_str.indexOf(",");
        if(index<1){
            return  GpsCoord();
            break;
        }
        QString text = ret_str.left(index);
        ret_str = ret_str.mid(index+1);

        //utc
        if(idx == 0){
            QDateTime dateTime =QDateTime::currentDateTimeUtc();
            dateTime.setTimeSpec(Qt::UTC);
            QTime utcTime= QTime::fromString(text.left(text.size()-3),"hh:mm:ss");
            dateTime.setTime(utcTime);
            gpsCord.utc_msec = dateTime.toTime_t();
        }

        //纬度
        if(idx == 1)
            gpsCord.x  = text.toDouble();
        //经度
        if(idx == 3)
            gpsCord.y  = text.toDouble();

        idx++;
    }while(idx<4);

    return gpsCord;
}

void GSerialPort::initWg()
{
    //串口号列表
    QList<QSerialPortInfo> portInfos =  QSerialPortInfo::availablePorts();
    int defaultIdx = -1;
    if(portInfos.size()>0) defaultIdx = 0;
    for(int i=0;i<portInfos.size();i++){
        ui->SerialNumCbx->addItem(portInfos[i].portName());
        if("USB Serial Port" == portInfos[i].description())
            defaultIdx = i;
    }

    //设置参数默认值
    ui->SerialNumCbx->setCurrentIndex(defaultIdx);
    ui->dataBitCbx->setCurrentIndex(3);
}



