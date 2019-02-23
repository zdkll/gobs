#include "gpositionsystem.h"
#include "ui_gpositionsystem.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "gserialport.h"

GPositionSystem::GPositionSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPositionSystem)
{
    ui->setupUi(this);

    createWg();

    connect(m_gSerialPort,&GSerialPort::recvGpsCord,this,&GPositionSystem::slotRecvGpsCord);
}

GPositionSystem::~GPositionSystem()
{
    delete ui;
}


void GPositionSystem::startPostion()
{
    //打开串口
    if(!m_gSerialPort->open()){
        QMessageBox::critical(this,QStringLiteral("打开串口"),QStringLiteral("打开串口%1失败！")
                              .arg( m_gSerialPort->serialPort()->portName()));
    }
}

void GPositionSystem::stopPostion()
{
    m_gSerialPort->close();
}

void GPositionSystem::slotRecvGpsCord(const GpsCoord& cord)
{
//    qDebug()<<"recv gps cord:"<<cord.x<<cord.y;

}

void GPositionSystem::createWg()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;

    //左侧边栏--------------------------------
    QVBoxLayout *leftLayout = new QVBoxLayout;
    //serialPort
    m_gSerialPort = new  GSerialPort(this);
    leftLayout->addWidget(m_gSerialPort);

    //button
    m_startBtn = new QPushButton(QStringLiteral("开始GPS定位"),this);
    m_stopBtn = new QPushButton(QStringLiteral("停止"),this);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(m_stopBtn);
    leftLayout->addLayout(btnLayout);
    leftLayout->addStretch(1);

    mainLayout->addLayout(leftLayout);

    //右侧绘图区域
    mainLayout->addWidget(new QWidget(this),1);

    this->setLayout(mainLayout);

    connect(m_startBtn,&QPushButton::clicked,this,GPositionSystem::startPostion);
    connect(m_stopBtn,&QPushButton::clicked,this,GPositionSystem::stopPostion);
}