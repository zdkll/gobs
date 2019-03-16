#include "deckunit.h"
#include  "ui_deckunit.h"

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QCheckBox>
#include <QComboBox>

#include "publicwidgets.h"


DeckUnit::DeckUnit(QWidget *parent)
    :QWidget(parent),ui(new Ui::DeckUnit)
    ,m_socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    createWg();

    connect(m_socket,&QTcpSocket::stateChanged,this,&DeckUnit::socketStateChanged);
    connect(m_socket,&QTcpSocket::readyRead,this,&DeckUnit::slotReadyRead);
}

//void DeckUnit::setShowTxtFunc(ShowTextFunc func)
//{
//    m_showTxtFunc = func;
//}

void DeckUnit::slotConnect(bool connect)
{
    if(connect){
        if(m_ipAddrEdit->text().isEmpty()){
            //            m_showTxtFunc(QStringLiteral("请输入正确的IP地址!"));
            return;
        }
        if(m_portEdit->text().isEmpty() || m_portEdit->text().trimmed().toInt()<1){
            //            m_showTxtFunc(QStringLiteral("请输入正确的Port!"));
            return;
        }
        m_socket->connectToHost(m_ipAddrEdit->text(),m_portEdit->text().trimmed().toShort());
    }else
        m_socket->disconnect();
}

void DeckUnit::startTx()
{
    //发送命令
    char cmdTx[] = {0x3A,0x01,0x05};
    m_socket->write(cmdTx,3);

    //EPC编号
//  m_epcCbx
}

void DeckUnit::stopTx()
{
    //发送命令
    char cmdTx[] = {0x3A,0x01,0x06};
    m_socket->write(cmdTx,3);
}

void DeckUnit::socketStateChanged(QAbstractSocket::SocketState state)
{
    if(state == QAbstractSocket::ConnectedState){
        //        m_showTxtFunc(QStringLiteral("连接到甲板机!"));
        m_optWidget->setEnabled(true);

        //        m_showTxtFunc(QStringLiteral("设置甲板机参数..."));
        //链接上之后设置参数
        //Repeat Interval 30seconds
        char cmdInterval[] = {0x3A,0x02,0x01,0x1E};
        m_socket->write(cmdInterval,3);

        //Repeat Times 30Times
        char cmdTimes[] = {0x3A,0x02,0x02,0x1E};
        m_socket->write(cmdTimes,3);
    }else if(state == QAbstractSocket::UnconnectedState)
    {
        //        m_showTxtFunc(QStringLiteral("甲板机断开链接!"));
        m_optWidget->setEnabled(false);
    }
}

void DeckUnit::slotReadyRead()
{
    QByteArray  ret = m_socket->readAll();
    //判断Command  0x07-延时
    if(int(ret.at(2)) == 0x07){
        int delayTime = int(ret[3])*(pow(2,8)) + (ret[4]);
    }
    else if(int(ret.at(2)) == 0x01){//设置重复间隔
        //        m_showTxtFunc(QStringLiteral("设置重复间隔完成!"));
    }else if(int(ret.at(2)) == 0x02){//设置重复次数
        //        m_showTxtFunc(QStringLiteral("设置重复次数完成!"));
    }
}

void DeckUnit::createWg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("甲板机通信"));
    QGridLayout *gridLayout = new QGridLayout();

    QLabel *ipLabel = new QLabel(QStringLiteral("IP:"));
    m_ipAddrEdit = new IpAddrEdit(this);
    m_ipAddrEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    m_ipAddrEdit->setText("10.10.100.254");

    gridLayout->addWidget(ipLabel,0,0);
    gridLayout->addWidget(m_ipAddrEdit,0,1);

    QLabel *portLabel = new QLabel(QStringLiteral("Port:"));
    m_portEdit = new QLineEdit(this);
    m_portEdit->setValidator(new QIntValidator(0,65537));//2^16 -1
    m_portEdit->setText("8899");
    gridLayout->addWidget(portLabel,1,0);
    gridLayout->addWidget(m_portEdit,1,1);

    //EPC编号-区分GOBS
    QLabel *epcLabel = new QLabel(QStringLiteral("EPC编号："));
    m_epcCbx = new QComboBox(this);
    m_epcCbx->setEditable(true);
    gridLayout->addWidget(epcLabel,2,0);
    gridLayout->addWidget(m_epcCbx,2,1);

    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->addLayout(gridLayout);

    QCheckBox   *connectCbx = new QCheckBox(QStringLiteral("链接甲板机"));
    QPushButton *startBtn = new QPushButton(QStringLiteral("开始TX"));
    QPushButton *stopBtn = new QPushButton(QStringLiteral("停止TX"));

    m_optWidget = new QWidget();
    QHBoxLayout *optLayout = new QHBoxLayout(m_optWidget);
    optLayout->addWidget(startBtn);
    optLayout->addWidget(stopBtn);
    m_optWidget->setEnabled(false);

    vLayout->addWidget(connectCbx,0,Qt::AlignLeft|Qt::AlignVCenter);
    vLayout->addWidget(m_optWidget);


    connect(connectCbx,&QCheckBox::toggled,this,&DeckUnit::slotConnect);
    connect(startBtn,&QPushButton::clicked,this,&DeckUnit::startTx);
    connect(stopBtn,&QPushButton::clicked,this,&DeckUnit::stopTx);

    groupBox->setLayout(vLayout);
    mainLayout->addWidget(groupBox);

    this->setLayout(mainLayout);
}
