#include "gpositionsystem.h"
#include "ui_gpositionsystem.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTextBrowser>

#include <QDebug>

#include "gserialport.h"
#include "datamanager.h"
#include "deckunit.h"

GPositionSystem::GPositionSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPositionSystem)
{
    ui->setupUi(this);
    m_showTxtFunc = std::bind(&GPositionSystem::showText,this,std::placeholders::_1);

    createWg();

    connect(m_gSerialPort,&GSerialPort::recvGpsCord,this,&GPositionSystem::slotRecvGpsCord);
}

GPositionSystem::~GPositionSystem()
{
    delete ui;
}

void GPositionSystem::showText(const QString &text)
{
    if(m_textBrowser->isBackwardAvailable()){
        m_textBrowser->clear();
    }
    m_textBrowser->append(text);
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
    if(cord.utc_sec == 0){

        return;
    }
    qDebug()<<"recv gps cord:"<<cord.utc_sec<<cord.x<<cord.y;

    //坐标传递给数据中心
    DataManager::instance()->addGpsCoord(cord);
}

void GPositionSystem::createWg()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;

    //左侧边栏--------------------------------
    QWidget  *leftWidget = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout;
    //serialPort
    m_gSerialPort = new  GSerialPort(this);
    leftLayout->addWidget(m_gSerialPort);

    //start stop  button
    m_startBtn = new QPushButton(QStringLiteral("开始GPS定位"),this);
    m_stopBtn = new QPushButton(QStringLiteral("停止"),this);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(m_stopBtn);
    leftLayout->addLayout(btnLayout);

    //Deck Unit
    m_deckUnit = new DeckUnit(this);
    leftLayout->addWidget(m_deckUnit);

    leftLayout->addStretch(1);
    leftWidget->setLayout(leftLayout);

    leftWidget->setMaximumWidth(280);

    m_textBrowser = new QTextBrowser(this);
    leftLayout->addWidget(m_textBrowser);

    mainLayout->addWidget(leftWidget,0);

    //右侧绘图区域
    mainLayout->addWidget(new QWidget(this),1);

    this->setLayout(mainLayout);

    m_gSerialPort->setShowTxtFunc(m_showTxtFunc);
    m_deckUnit->setShowTxtFunc(m_showTxtFunc);

    connect(m_startBtn,&QPushButton::clicked,this,&GPositionSystem::startPostion);
    connect(m_stopBtn,&QPushButton::clicked,this,&GPositionSystem::stopPostion);
}
