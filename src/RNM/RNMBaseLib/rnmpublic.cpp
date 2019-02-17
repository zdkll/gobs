#include "rnmpublic.h"

#include <QHostAddress>

#include "nodequery.h"
#include "nodeftp.h"
#include "rnitems.h"
#include "rnftpmanager.h"
#include "statusreport.h"

const QColor state_color[6] = {
    Node_Status_Disable_Color,
    Node_Status_Normal_Color,
    Node_Status_Warning_Color,
    Node_Status_Error_Color,
    Node_SelfTest_Warning_Color,
    Node_Downloaded_Color
};


//起始地址
static const uint startIp = QHostAddress("192.168.1.0").toIPv4Address();

bool isNodeIP(const uint &ipV4)
{
    uint val  = ipV4 - startIp;
    uint temp = val%256;
    //每个网段的100-200为节点的IP地址
    return (100 <= temp && temp<200);
}

uint ip2DeviceNo(const uint &ipV4)
{
    uint temp = ipV4 - startIp;
    return (temp/256)*100 + ((temp%256)-100);
}

QString workName(const int &work)
{
    if(work == Updating)
        return "Updating";
    else if(work == DownLoadFiles)
        return "Downloading Files";
    else if(work == ClearMemory)
        return "Clearing Files";
    else if(work == SetSampleFrequecy)
        return "Setting Sample Frequency";
    else //None Work
        return "";
}

//Node----------------------------------------
Node::Node(QObject *parent)
    :QObject(parent),m_nodeQuery(0)
{
    //6种状态当前对应的颜色
    m_colors = new QColor[7];
    //根据节点链接状态判断是否创建查询
    m_nodeQuery = new NodeQuery(this);

    //参数状态
    for(int i = 0;i<7;i++)
        m_state[i] = Normal;

    m_ftpManager = new FtpManager(this);

    m_downloadState = NoDownload;
    m_ftpError      = NoError;
}

Node::~Node()
{
    if(m_nodeQuery)
        m_nodeQuery->stop();

    delete[] m_colors;
}

void Node::startRefresh(const int &interval)
{
    //参数更新开始
    m_nodeQuery->start(interval);
}

void Node::stopRefresh()
{
    m_nodeQuery->stop();
}

void Node::refreshState()
{
    m_nodeQuery->refresh();
}
bool Node::isRunning() const
{
    return m_nodeQuery&&m_nodeQuery->isRunning();
}

void Node::updateNode(const UpdateCmd &updateCmd)
{
    if(updateCmd & State_Update)
    {
        //调用状态控制器计算每个参数状态
        m_stCtrler->calNodeState(this);
        int filter = state()?m_stCtrler->displayFilter():0;
        State st = m_state[filter];
        //项显示更新----------------
        if(nodeItem){
            nodeItem->updateNode(state_color[st]);
        }

        //向中介传递更新信息
        m_nodeMediator->nodeToStatusReport(updateCmd,this);
    }
    if(updateCmd & Ftp_Update){
        //向中介传递更新信息
        m_nodeMediator->nodeToFtpManagerMsg(this,updateCmd);
    }
}

void Node::startFtpWork(const FtpWork &ftpWk,const QVariant &arg)
{
    m_ftpManager->startWork(ftpWk,arg);
}

void Node::stopFtpWork()
{
    m_ftpManager->stopFtpWork();
}

void Node::finishedFtpWork(const int &ftpWk,const bool &err)
{
    m_nodeMediator->finishedFtpWork(this,ftpWk,err);
}


Node::NodeData::NodeData()
    :no(0),ip(0)
    ,state(false)
    ,sVoltage(0.0)
    ,sTemper(0.0)
    ,sPress(0.0)
    ,sEleCurr(0.0)
    ,sChargeVolt(0.0)
    ,sMemory(0.0)
    ,downloadPercent(0)
    ,sampleFrequency(1000)
    ,downloaded(false)
{

}


//状态控制--------------------------------------------
void StatusControler::setFilters(const int &status)
{
    m_filters = status;

    //更新---------------------------
    foreach (Node *node, m_nodes){
        node->updateNode(State_Update);
    }
}

void StatusControler::setLimitData(const QVector<LimitData> &limitData)
{
    m_LimitData = limitData;
    //更新
    qDebug()<<"LimitData:"<<limitData.size();
    foreach (Node *node, m_nodes){
        node->updateNode(State_Update);
    }
}

void StatusControler::setNodes(QVector<Node *> nodes)
{
    m_nodes = nodes;
}

void StatusControler::calNodeState(Node *node)
{
    //QString nodeName = QString("G%1").arg(node->no(),3,10,QChar('0'));
    //节点状态
    Node::State  *stateVale = node->stateValue();
    if(!node->state()){
        stateVale[0] = Node::Disable;
        return;
    }
    stateVale[0] = Node::Normal;
    //自检状态
    stateVale[5] = Node::SelfTest_Ok;

    //voltage
    stateVale[1] = calState(node->voltage(),m_LimitData[0]);
    if(stateVale[1] != Node::Normal)
        stateVale[5] = Node::SelfTest_Warning;
    //    if(stateVale[1] == Node::State_Error)
    //        qWarning(QString(nodeName+"'s voltage is too low").toLatin1().data());

    //temperature
    stateVale[2] = calState(node->temperature(),m_LimitData[1]);
    if(stateVale[2] != Node::Normal)
        stateVale[5] = Node::SelfTest_Warning;
    //if(stateVale[2] == Node::State_Error)
    //    qWarning(QString(nodeName+"'s temperature is too high").toLatin1().data());

    //pressure
    stateVale[3] = calState(node->pressure(),m_LimitData[2]);
    if(stateVale[3] != Node::Normal)
        stateVale[5] = Node::SelfTest_Warning;
    //    if(stateVale[3] == Node::State_Error)
    //        qWarning(QString(nodeName+"'s pressure is too low").toLatin1().data());

    //current
//    stateVale[4] = calState(node->current(),m_LimitData[3]);
//    if(stateVale[4] != Node::Normal)
//        stateVale[7] = Node::SelfTest_Warning;
    //    if(stateVale[4] == Node::State_Error)
    //        qWarning(QString(nodeName+"'s current is too low").toLatin1().data());

    //charge Volt
//    stateVale[5] = calState(node->chargeVolt(),m_LimitData[4]);
//    if(stateVale[5] != Node::Normal)
//        stateVale[7] = Node::SelfTest_Warning;
    //    if(stateVale[5] == Node::State_Error)
    //        qWarning(QString(nodeName+"'s charge voltage is too low").toLatin1().data());

    //memory
    stateVale[4] = calState(node->memory(),m_LimitData[5]);
    if(stateVale[4] != Node::Normal)
        stateVale[5] = Node::SelfTest_Warning;
    //if(stateVale[6] == Node::State_Error)
    //     qWarning(QString(nodeName+"'s storage is too less").toLatin1().data());

    //self test

    //download
    stateVale[6] = node->isDownloaded()?Node::Downloaded:Node::Normal;
}

Node::State StatusControler::calState(const float &value,const LimitData &limit)
{
    //计算颜色,红黄绿--------------------------
    if(limit.operate == Big)
    {
        if(value<=limit.first)
            return Node::Normal;
        else if (value>limit.second)
            return Node::State_Error;
        else
            return Node::State_Warning;
    }
    else
    {
        if(value<limit.second)
            return Node::State_Error;
        else if (value>=limit.first)
            return Node::Normal;
        else
            return Node::State_Warning;
    }
}




//NodeMediator 节点中介----
NodeMediator::NodeMediator(QObject *parent)
    :QObject(parent),m_ftpManager(0)
{

}

void NodeMediator::setRNFtpManager(RNFtpManager  *ftpManager)
{
    m_ftpManager = ftpManager;
}

void NodeMediator::setStatusReport(StatusReportDlg *statusReport)
{
    m_statusReport = statusReport;
}

void NodeMediator::setNodes(const QVector<Node*> &nodes)
{
    m_nodes = nodes;
}

void NodeMediator::nodeToStatusReport(const UpdateCmd &msg,Node *node)
{
    if(m_statusReport)
        m_statusReport->getMessage(msg,node);
}

void NodeMediator::nodeToFtpManagerMsg(Node *node,const UpdateCmd &msg)
{
    if(m_ftpManager)
        m_ftpManager->getMessage(node,msg);
}



void NodeMediator::finishedFtpWork(Node *node,const int &ftpWk,const bool &err)
{
    if(m_ftpManager)
        m_ftpManager->finishedFtpWork(node,ftpWk,err);
}

