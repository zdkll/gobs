#ifndef STATIONITEM_H
#define STATIONITEM_H

#define Legend_Bouding_Height  20

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QWidget>

#include "lmgrpublic.h"

//显示项的基类---------
class BaseItem
{
public:
    enum ShowItemFlag
    {
        NoneItem = 0x0,
        ShotItem = 0x01,
        RecvItem = 0x02,
        ShotLineItem = 0x04,
        RecvLineItem = 0x08,
        ShotTextItem = 0x10,
        RecvTextItem = 0x20,
        DeFaultItems = ShotItem|RecvItem,
        AllItems     = ShotItem|RecvItem|ShotLineItem|RecvLineItem|ShotTextItem|RecvTextItem
    };
    Q_DECLARE_FLAGS(ShowItemFlags, ShowItemFlag)

    BaseItem();

    virtual void setColor(const QColor &color) = 0;
};

//站点项，继承RectItem-----------------------
class StationItem : public QGraphicsRectItem , public BaseItem
{
public:
    explicit StationItem(const Station &station,QGraphicsItem *parent = 0);

    const Station &station(){return m_station;}

    char stationId(){return m_station.Id;}

    void setColor(const QColor &color);

protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

private:
    Station     m_station;
};

//线号文本项
class LineItem : public QGraphicsTextItem , public BaseItem
{
public:
    LineItem(const StaLine &staLine,QGraphicsItem *parent = 0);

    void setColor(const QColor &color);

    //计算文本坐标位置
    static QPointF calPos(const StaLine &staLine);

    StaLine    m_staLine;
};

//图例
class LegendItem : public QGraphicsItem
{
public :
    LegendItem(QGraphicsItem *parent = 0);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setScreenWidth(const float &phyWidth,const int &screenWidth);

    void setColor(const QColor &color){
        m_color = color;
        this->update(boundingRect());
    }
private:
    float calUnitWidth(const float &phyWidth) const;

    float m_phyWidth;
    int   m_screenWidth;
    QColor m_color;
};


Q_DECLARE_OPERATORS_FOR_FLAGS(BaseItem::ShowItemFlags)

#endif // STATIONITEM_H
