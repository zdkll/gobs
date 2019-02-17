#include "rnitems.h"
#include <QPainter>
#include <QPainterPath>
#include <QHostAddress>

#include <QGraphicsSceneMouseEvent>

//NodeQuery::NodeQuery()
//{

//}

QSize NodeItem::m_size = QSize(Node_Item_Width,Node_Item_Height);

NodeItem::NodeItem(Node *node,QGraphicsItem *parent)
    :QGraphicsItem(parent) ,m_node(node)
    ,m_color(Node_Status_Disable_Color)
{
    this->setFlags(QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    this->setAcceptHoverEvents(true);
}

NodeItem::~NodeItem()
{

}

void NodeItem::setSize(const QSize &size)
{
    m_size = size;

}

void NodeItem::updateNode(const QColor &color)
{
    m_color = color;

    //更新接口链接状态-------
    if(this->parentItem())
    {
        GroupBoxItem *parentItem = static_cast<GroupBoxItem *>(this->parentItem());
        parentItem->setInterfaceState(m_groupId,*m_node);
    }

    //更新状态-----
    update();
}

QRectF NodeItem::boundingRect() const
{
    return QRectF(0,0,m_size.width(),m_size.height());
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing,true);

    QPen pen;
    if(this->isSelected()){
        pen.setStyle(Qt::DashLine);
    }
    painter->setPen(pen);
    int w = m_size.width();
    int h = m_size.height();

    //    w = 64;
    //    h = 40;
    //    w = 96;
    //    h = 60;
    //    w = 80;
    //    h = 50;

    w = Node_Item_Width;
    h = Node_Item_Height;

    QString text = QString("%1%2").arg(Node_Id).arg(m_node->no(),3,10,QChar('0'));

    //正面------
    QPainterPath path;
    painter->setBrush(m_color);
    path.moveTo(0,h/5);
    path.arcMoveTo(0,0,w,h*2/5,0);

    path.lineTo(w,h*3/5);

    path.arcTo(0,h*3/5,w,h*2/5,0,-180);
    path.lineTo(0,h/5);
    painter->drawPath(path);

    //顶部-------
    painter->setBrush(QBrush(Qt::lightGray));
    painter->drawEllipse(0,0,w,h*2/5);

    //编号---
    //if(m_node.state)
    painter->drawText(QRectF(0,h*2/5,w,h*3/5),Qt::AlignCenter,QString(text));
}



/*
*
*节点设备组
*/
GroupBoxItem::GroupBoxItem(QGraphicsItem *parent)
    :QGraphicsItem(parent),m_itemCount(0)
{
    base_x = 0;
    //计算需要的空间大小ceil(m_itemCount/3)+1,itemCount =15
    int w = GroupBox_Item_Width;

    m_deviceItem = new DeviceItem(this);
    m_deviceItem->setPos((w-Device_Item_Width)/2,10);

    this->setFlag(QGraphicsItem::ItemIsSelectable);
}

GroupBoxItem::~GroupBoxItem()
{

}

const QSize GroupBoxItem::size()
{
    return QSize(GroupBox_Item_Width,GroupBox_Item_Height);
}

QRectF GroupBoxItem::boundingRect() const
{
    //增加文字突出部分
    return QRectF(0,-10,GroupBox_Item_Width,GroupBox_Item_Height+10);
}

void GroupBoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    //计算需要的空间大小ceil(m_itemCount/3)+1,itemCount =15
    int w = GroupBox_Item_Width;
    int h = GroupBox_Item_Height;

    //边框----------
    painter->drawRect(0,0,w,h);

    //组标签文字-----
    painter->fillRect(QRectF(10,-10,80,20),Qt::white);
    painter->drawText(10,-10,80,20,Qt::AlignCenter,QString("Group %1").arg(m_groupId));
}

NodeItem *GroupBoxItem::addNodeItem(Node *node)
{
    NodeItem *item = new NodeItem(node,this);
    m_childNodeItems.append(item);
    item->setGroupId(m_itemCount);
    setInterfaceState(m_itemCount,*node);

    //单元间适当分开,布局显示
    {
        if(m_itemCount%3 == 0)
            base_x += 20;
        base_x += 10;
        item->setPos(base_x,GroupBox_Item_Height-Node_Item_Height-10);

        base_x += Node_Item_Width;
        m_itemCount ++;
    }

    return item;
}

void GroupBoxItem::addNodeItem()
{

}

void GroupBoxItem::setInterfaceState(const int &id, const Node &node)
{
    //通信设备接口通信状态设置
    if(node.state())
        m_deviceItem->setInterfaceState(id,DeviceItem::InterActive);
    else
        m_deviceItem->setInterfaceState(id,DeviceItem::InterDown);
}


//---------------------------------------
DeviceItem::DeviceItem(QGraphicsItem *parent)
    :QGraphicsItem(parent)
{
    //初始化状态
    for(int i = 0 ; i<15;i++)
        m_interStatues.insert(i,InterNoConnect);
}

DeviceItem::~DeviceItem()
{

}

QRectF DeviceItem::boundingRect() const
{
    return QRectF(0,0,Device_Item_Width,Device_Item_Height);
}

void DeviceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->drawRect(0,0,Device_Item_Width,Device_Item_Height);

    painter->save();
    //Frame--------
    drawFrame(painter);

    //绘制接口------
    painter->restore();
    drawInterfaces(painter);
}

void DeviceItem::drawFrame(QPainter *painter)
{
    //绘制Frame
    painter->setBrush(Qt::black);
    painter->translate((Device_Item_Width-Frame_Item_Width)/2,5);
    painter->drawRect(0,0,Frame_Item_Width,Frame_Item_Height);

    painter->setBrush(Qt::green);
    //3列 x 5行设备
    //上下左右Margin = 5
    painter->translate(0,5);
    painter->translate(5,0);
    for(int i=0;i<5;i++){
        painter->save();
        for(int j =0;j<3;j++)
        {
            painter->drawEllipse(QPoint(5,5),2,2);
            painter->translate(Frame_Item_Width/4,0);
        }
        painter->restore();
        painter->translate(0,Frame_Item_Height/6);
    }
}

void DeviceItem::drawInterfaces(QPainter *painter)
{
    painter->translate(10,15+Frame_Item_Height);
    for(int i =0;i<m_interStatues.size();i++){
        if(i%3 == 0)
            painter->translate(10,0);

        if(m_interStatues.value(i) == InterNoConnect)
            painter->setBrush(Qt::darkGray);
        else if(m_interStatues.value(i) == InterDown)
            painter->setBrush(Qt::gray);
        else
            painter->setBrush(Qt::green);

        painter->drawEllipse(0,0,Interface_Item_Width,Interface_Item_Height);

        painter->translate(Interface_Item_Width+10,0);
    }
}

