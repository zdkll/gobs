#include "nodequery.h"
#include <QHostAddress>
#include <QDebug>

#define Socket_WaitForConnect_Time 1
#define TCP_IP_PORT                5000

//最小查询延迟
#define Max_Query_Delay            30000

#include "rnitems.h"

//获取电压，工作电流，充电电压
static const char voltCmd[3] = {0x3a,0x01,0x62};
//获取仓温
static const char tempCmd[3] = {0x3a,0x01,0x60};
//获取仓压
static const char pressCmd[3] = {0x3a,0x01,0x61};

NodeQuery::NodeQuery(Node *owner)
    :QTcpSocket(owner),m_queryStep(NoQuery)
    ,m_running(false),m_count(0), m_stoped(true)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(slotReadData()));
    connect(this,SIGNAL(stateChanged(QAbstractSocket::SocketState))
            ,this,SLOT(slotStateChanged(QAbstractSocket::SocketState)));

    m_node = owner;
}

void NodeQuery::reConnect()
{
    flush();
    abort();
    connectToHost(QHostAddress(m_node->ip()),TCP_IP_PORT);
}

void NodeQuery::disConnect()
{
    this->disconnectFromHost();
    m_running = false;
}

void NodeQuery::start(const int &interval)
{
    m_interval = interval;
    m_stoped = false;
    connectToHost(QHostAddress(m_node->ip()),TCP_IP_PORT);
}

void NodeQuery::stop()
{
    m_count = 0;
    flush();
    m_stoped = true;
    this->abort();
    m_running = false;
}

void NodeQuery::refresh()
{
    //两种节点状态处理
    //1)节点状态为 false
    if(!m_node->state()){
        m_count++;
        //当前状态为false的节点,几个周期查询链接一次，降低频率
        if(m_count*m_interval>=Max_Query_Delay){
            reConnect();
            m_count = 0;
        }
        return;
    }
    else{
        //节点正在运行,可能是通讯中断，需要重新刷新链接启动
        if(m_running)
        {
            m_running = false;
            reConnect();
            return;
        }
    }

    //节点状态 ok，并且上次运行结束，直接查询刷新
    startQuery();
}

void NodeQuery::startQuery()
{
    if(this->state() != QAbstractSocket::ConnectedState)
        return;

    m_running = true;
    t.start();
    //启动查询,从电压-仓温-仓压顺序查询
    write(voltCmd,sizeof(voltCmd));
    m_queryStep = QueryVoltage;
}

void NodeQuery::slotReadData()
{
    quint16 iData;
    quint32 bdata;
    QByteArray byteArray = this->readAll();
    QString temp;

    if(m_queryStep == QueryVoltage)
    {
        //电压
        iData = (((byteArray[4]&0xff)<<8)|(byteArray[3]&0xff));
        temp = QString::number(iData);
        m_node->setVoltage(temp.toFloat()/1000.f);

        //工作电流
        iData=(((byteArray[6]&0xff)<<8)|(byteArray[5]&0xff));
        temp = QString::number(iData);
        m_node->setCurrent(temp.toFloat()/1000.f);

        //充电电压
        iData=(((byteArray[8]&0xff)<<8)|(byteArray[7]&0xff));
        temp = QString::number(iData);
        m_node->setChargeVolt(temp.toFloat()/1000.f);

        //查询仓温
        write(tempCmd,sizeof(tempCmd));
        m_queryStep = QueryTemp;
        return;
    }
    if(m_queryStep == QueryTemp)
    {
        //仓温
        iData = (((byteArray[4]&0xff)<<8)|(byteArray[3]&0xff));
        temp = QString::number(iData);
        m_node->setTemperature(temp.toFloat()/10.f);

        //查询仓压
        write(pressCmd,sizeof(pressCmd));
        m_queryStep = QueryPressure;
        return;
    }
    if( m_queryStep == QueryPressure)
    {
        //仓压
        bdata = ((byteArray[3]&0xff)|((byteArray[4]&0xff)<<8)
                |((byteArray[5]&0xff)<<16)|((byteArray[6]&0xff)<<24));

        temp = QString::number(bdata);
        m_node->setPressure(temp.toFloat()/101325.0f);

        //结束--------------------------------
        m_queryStep = NoQuery;
        m_running = false;

        //更新显示状态
        m_node->updateNode(State_Update);
    }
}

void NodeQuery::slotStateChanged(QAbstractSocket::SocketState state)
{
    if(state == QAbstractSocket::UnconnectedState)
    {
        if(!m_stoped){
            m_node->setState(false);
            m_node->updateNode();
        }
    }
    if(state == QAbstractSocket::ConnectedState)
    {
        //开始查询
        m_node->setState(true);
        startQuery();
    }
}



