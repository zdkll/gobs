#include "rnmgraphview.h"

#define  GroupBox_Item_VSpace   20
#define  GroupBox_Item_HSpace   10

#define  Default_Refresh_Interval       5000

#include <QTimer>
#include <QMouseEvent>
#include <QDialog>
#include <QTableWidget>
#include <QMessageBox>
#include <QBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QHeaderView>

RNMGraphView::RNMGraphView(QWidget *parent)
    :QGraphicsView(parent)
    ,m_showAllConnected(false)
    ,m_timer(new QTimer(this))
{
    m_scene = new QGraphicsScene(this);
    this->setScene(m_scene);


    this->setAlignment(Qt::AlignLeft|Qt::AlignTop);

    m_updateInterval = Default_Refresh_Interval;
    m_timer->setInterval(Default_Refresh_Interval);
    m_NodeInfoDlg = new NodeInfoDlg(this);

    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotRefresh()));
}

RNMGraphView::~RNMGraphView()
{
    //正在刷新，停止
    if(m_timer->isActive()){
        startOrStopUpdateNode(false);
    }
}

//void RNMGraphView::mouseMoveEvent(QMouseEvent *e)
//{
//    qDebug()<<"mouse move";
//    QGraphicsView::mouseMoveEvent(e);
//}

void RNMGraphView::setNodes(const QVector<Node*> &nodes)
{
    m_Nodes = nodes;

    //初始化布局显示
    initNodeDisplay(nodes);
}

void RNMGraphView::setStatisticsData(StatisticsData *data)
{
    m_statisticsData = data;
}

void RNMGraphView::startOrStopUpdateNode(const bool &start)
{
    //启动
    if(start){
        foreach (Node *node, m_Nodes) {
            node->startRefresh(m_updateInterval);
        }
        m_timer->start();
    }
    //停止
    else
    {
        m_timer->stop();
        foreach (Node *node, m_Nodes) {
            node->stopRefresh();
        }
    }
}

void RNMGraphView::setUpdateInterval(const int &interval)
{
    m_updateInterval = interval;
    m_timer->setInterval(m_updateInterval);
}

bool RNMGraphView::isUpdating() const
{
    return m_timer->isActive();
}

//
void RNMGraphView::setShowAllOrConnected(const bool &show)
{
    m_showAllConnected = show;

    //先停止刷新
    bool is_updating = isUpdating();
    if(is_updating)
        startOrStopUpdateNode(false);

    initNodeDisplay(m_Nodes);

    //启动刷新
    if(is_updating)
        startOrStopUpdateNode(true);
}

void RNMGraphView::searchNode(const uint &node_no)
{
    foreach(NodeItem *item, m_nodeItems) {
        if(item->node()->no() == node_no){
            item->setSelected(true);
            this->ensureVisible(item);
            return;
        }
    }

    QMessageBox::information(this,"Information"
                             ,QString("Not Found Node G%1.").arg(node_no,3,10,QChar('0')));
}

void RNMGraphView::mouseDoubleClickEvent(QMouseEvent *e)
{
    QGraphicsItem *item = m_scene->itemAt(mapToScene(e->pos()),this->transform());
    if(item){
        NodeItem      *nodeItem = dynamic_cast<NodeItem *>(item);
        if(nodeItem)
        {
            m_NodeInfoDlg->showNodeInfo(nodeItem->node());
        }
    }
    QGraphicsView::mouseDoubleClickEvent(e);
}

void RNMGraphView::mousePressEvent(QMouseEvent *e)
{

    QGraphicsView::mousePressEvent(e);
}

void RNMGraphView::slotRefresh()
{
    foreach(Node *node, m_Nodes)
        node->refreshState();
    emit updateNodes();
}


void RNMGraphView::initNodeDisplay(const QVector<Node*> &nodes)
{
    //按照每行15 台设备固定显示---
    m_scene->clear();

    m_nodeItems.clear();
    m_groupBoxItem.clear();

    //计算行数(组)
    int node_num = nodes.size();
    int max_rowCount = node_num/Group_Node_Num;
    max_rowCount = (node_num%Group_Node_Num==0)?max_rowCount:(max_rowCount+1);
    m_groupBoxItem.resize(max_rowCount);
    m_nodeItems.resize(node_num);

    QSize groupSize =  GroupBoxItem::size();
    int row = 0;
    int base_y = 0;
    int index  = 0;
    GroupBoxItem  *boxItem = 0;
    int avalibleNum = 0;
    for(int i = 0 ;i<nodes.size();i++)
    {
        //15个一组----------------------
        if(!boxItem || boxItem->childs() == 15)
        {
            base_y += GroupBox_Item_VSpace;

            //添加组--------------------
            boxItem = new GroupBoxItem;
            m_groupBoxItem[row] = boxItem;
            boxItem->setPos(GroupBox_Item_HSpace,base_y);
            boxItem->setGroupId(row+1);
            m_scene->addItem(boxItem);
            row ++;
            base_y += groupSize.height();

        }
        if(m_showAllConnected || nodes[i]->state()){
            if(nodes[i]->state())
                avalibleNum ++;
            //添加和保存节点
            NodeItem *nodeItem = boxItem->addNodeItem(nodes[i]);
            nodes[i]->setId(i);
            nodes[i]->setItem(nodeItem);

            m_nodeItems[index] = nodeItem;
            index ++;
        }else
            nodes[i]->setItem(0);//不显示需要将对应的项指针置为0,不再更新显示状态
    }

    m_groupBoxItem.resize(row);
    m_nodeItems.resize(index);

    //统计数据
    m_statisticsData->totalNodes     = node_num;
    m_statisticsData->avaliableNodes = avalibleNum;
    m_statisticsData->nodeBoxes      = row;

    m_scene->setSceneRect(0,0,groupSize.width()+10*2
                          ,(groupSize.height())*row+(row+1)*20);
}

void RNMGraphView::updateNodeDisplay()
{
    m_nodeItems.clear();
    m_groupBoxItem.clear();
    //...

}



NodeInfoDlg::NodeInfoDlg(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Node Information");
    initDlg();
}

void NodeInfoDlg::initDlg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_txtBrowser = new QTextBrowser;
    mainLayout->addWidget(m_txtBrowser);

    QPushButton *closeButton = new QPushButton(tr("&Close"),this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch(1);
    hLayout->addWidget(closeButton);
    mainLayout->addLayout(hLayout);

    this->setLayout(mainLayout);
    connect(closeButton,SIGNAL(clicked(bool)),this,SLOT(close()));
}

void NodeInfoDlg::showNodeInfo(const Node *node)
{
    m_txtBrowser->clear();

    m_txtBrowser->append(QString("Device.No: G%1").arg(node->no(),3,10,QChar('0')));
    QString stateStr = node->state()?"true":"false";
    m_txtBrowser->append(QString("Connected: %1").arg(stateStr));
    m_txtBrowser->append("");

    m_txtBrowser->append(QString("Voltage:                %1(V)").arg(node->voltage()));
    m_txtBrowser->append(QString("Temperature:       %1(℃)").arg(node->temperature()));
    m_txtBrowser->append(QString("Pressure:              %1(atm)").arg(node->pressure(),0,'g',3));
    m_txtBrowser->append(QString("Current:               %1(A)").arg(node->current()));
    m_txtBrowser->append(QString("Charge Voltage:    %1(V)").arg(node->chargeVolt()));
    m_txtBrowser->append(QString("Memory Capacity:%1(GB)").arg(node->memory()));

    this->show();
}


