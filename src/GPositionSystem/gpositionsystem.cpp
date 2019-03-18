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
#include "positionchart.h"
#include "postioncontrols.h"

GPositionSystem::GPositionSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPositionSystem)
{
    ui->setupUi(this);
    //    m_showTxtFunc = std::bind(&GPositionSystem::showText,this,std::placeholders::_1);

    createWg();

    this->resize(1000,520);

    connect(m_gSerialPort,&GSerialPort::recvGpsCord,this,&GPositionSystem::slotRecvGpsCord);

    testData();
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
    mainLayout->setSpacing(0);
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

    leftWidget->setLayout(leftLayout);

    leftWidget->setMaximumWidth(280);

    m_textBrowser = new QTextBrowser(this);
    leftLayout->addWidget(m_textBrowser);

    mainLayout->addWidget(leftWidget,0);

    //右侧绘图布局
    QVBoxLayout *rightLayout = new QVBoxLayout;
    TitleBar *titleBar = new TitleBar(this);
    rightLayout->addWidget(titleBar);

    m_positionChart = new PositionChart(this);
    rightLayout->addWidget(m_positionChart,1);

    mainLayout->addLayout(rightLayout,1);

    this->setLayout(mainLayout);

    //    m_gSerialPort->setShowTxtFunc(m_showTxtFunc);
    //    m_deckUnit->setShowTxtFunc(m_showTxtFunc);

    connect(m_startBtn,&QPushButton::clicked,this,&GPositionSystem::startPostion);
    connect(m_stopBtn,&QPushButton::clicked,this,&GPositionSystem::stopPostion);

    connect(titleBar,&TitleBar::sideBarHide,leftWidget,&QWidget::setHidden);
}

void GPositionSystem::testData()
{
    //模拟数据传递给画图控件绘制
    int N = 100;
    QVector<GpsCoord>   gpsCords;
    QVector<GobsCoord> gobsCords;
    float x0 = 100000;
    float y0 =  2000000;
    float x,y;
    for(int i=0;i<N;i++){
        GpsCoord cord;
        cord.x = x0+i*100;
        cord.y = y0+rand()%10000;
        gpsCords.append(cord);

        if(i%4 ==0){
            GobsCoord gobs_cord;
            gobs_cord.x = x0+i*100;
            gobs_cord.y = y0+rand()%10000;
            gobsCords.append(gobs_cord);
        }
    }

    m_positionChart->setData(gpsCords,gobsCords);
}
