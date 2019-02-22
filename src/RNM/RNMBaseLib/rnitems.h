#ifndef RNITEMS_H
#define RNITEMS_H

#define Group_Node_Num         15
#define Node_Item_Width        48
#define Node_Item_Height       30

#define Frame_Item_Width       40
#define Frame_Item_Height      60

#define Interface_Item_Width   16
#define Interface_Item_Height  Interface_Item_Width

#define Device_Item_Width      (Interface_Item_Width*Group_Node_Num+(6+16)*10)
#define Device_Item_Height     (Frame_Item_Height+Interface_Item_Height+20)

#define GroupBox_Item_Width    6*20+Node_Item_Width*Group_Node_Num+16*10
#define GroupBox_Item_Height   Device_Item_Height+Node_Item_Height+30


#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QTcpSocket>
#include <QThread>

#include <QMetaType>//记得包含这个头文件

#include "gpublic.h"
#include "rnmpublic.h"
#include "rnmbaselib_global.h"

//节点项目,实现状态刷新线程任务
class GroupBoxItem;
class StatusControler;
class RNMBASELIBSHARED_EXPORT NodeItem : public QGraphicsItem
{
public:
    NodeItem(Node *node,QGraphicsItem *parent = 0);
    ~NodeItem();

    static const QSize &size(){return m_size;}
    void setSize(const QSize &size);

    //更新显示状态
    void updateNode(const QColor &color);

    QRectF   boundingRect() const;
    void     paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //groupId
    const int  &groupId(){return m_groupId;}
    void       setGroupId(const int &id){m_groupId = id;}

    Node       *node() {return m_node;}

private:
    static QSize  m_size;
    Node           *m_node;
    int                 m_id;
    int                 m_groupId;
    QColor          m_color;
};


class DeviceItem;
//默认按照15台节点设备摆放,每3台为一单元
class RNMBASELIBSHARED_EXPORT GroupBoxItem : public QGraphicsItem
{
public:
    GroupBoxItem(QGraphicsItem *parent = 0);
    ~GroupBoxItem();

    static const QSize size();

    QRectF boundingRect() const;

    void  paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem *addNodeItem( Node *node);

    void      addNodeItem();

    void setGroupId(const int &id){m_groupId = id;}

    void setInterfaceState(const int &id, const Node &node);

    int childs(){return m_childNodeItems.size();}

private:
    int          m_itemCount;
    int          base_x,base_y;

    DeviceItem  *m_deviceItem;
    int          m_groupId;
    QList<NodeItem *>   m_childNodeItems;

};


class RNMBASELIBSHARED_EXPORT DeviceItem : public QGraphicsItem
{
public:
    enum InterfaceStatus
    {
        InterNoConnect  = 0,
        InterDown       = 1,
        InterActive     = 2
    };
    DeviceItem(QGraphicsItem *parent = 0);
    ~DeviceItem();

    QRectF boundingRect() const;

    void  paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setInterfaceState(const int &id, InterfaceStatus state){
        m_interStatues.insert(id,state);
        update();
    }

private:
    //绘制函数
    void drawFrame(QPainter *painter);
    void drawInterfaces(QPainter *painter);

private:
    QMap<int,InterfaceStatus>  m_interStatues;
};



#endif // RNITEMS_H
