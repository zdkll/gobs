#include "stationitem.h"

#include <QPainter>

BaseItem::BaseItem()
{

}


//-----------------------------------------------------------------------
StationItem::StationItem(const Station &station,QGraphicsItem *parent)
    :QGraphicsRectItem(parent),BaseItem(),m_station(station)
{
    //可选和聚焦
    this->setFlags(ItemIsSelectable|ItemIsFocusable);

    //    //颜色
    //    if(m_station.Id == Station_Rcv_Id)
    //        this->setBrush(QBrush(Qt::white));
    //    else if(m_station.Id == Station_Shot_Id)
    //        this->setBrush(QBrush(Qt::red));

    //位置
//    this->setRect(m_station.x,m_station.y,10,10);

}

void StationItem::setColor(const QColor &color)
{
    this->setBrush(QBrush(color));
}

void StationItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    //    //接受点---------
    //    if(m_station.Id == Station_Rcv_Id ){
    //        if(m_shotItemFlags & RecvItem){
    //            this->setVisible(true);
    //        }else{
    //            this->setVisible(false);
    //        }
    //    }

    //    //炮点-------------------------------
    //    if(m_station.Id == Station_Shot_Id){
    //        if(m_shotItemFlags & ShotItem)
    //            this->setVisible(true);
    //        else{
    //            this->setVisible(false);
    //        }
    //    }

    QGraphicsRectItem::paint(painter,option,widget);
}


LineItem::LineItem(const StaLine &staLine,QGraphicsItem *parent)
    :QGraphicsTextItem(parent),BaseItem(),m_staLine(staLine)
{
    //计算设置文本位置
    this->setPos(calPos(m_staLine));

    QFont font;
    this->setPlainText(QString("%1%2").arg(m_staLine.type)
                       .arg(m_staLine.line));

    this->setFont(font);

    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

void LineItem::setColor(const QColor &color)
{
    this->setDefaultTextColor(color);
}

QPointF LineItem::calPos(const StaLine &staLine)
{
    //计算角度
    int staNum        = staLine.staNum;
    Station  *staions = staLine.stations;
    QString text      = QString("%1%2").arg(staLine.type).arg(staLine.line);
    QLineF lineF;
    lineF.setP1(QPointF(staions[0].x,staions[0].y));
    lineF.setP2(QPointF(staions[staNum-1].x,staions[staNum-1].y));
    float angle = lineF.angle();
    //qDebug()<<angle;
    //根据角度正负确定绘制位置偏移 以x或y轴偏移----
    QPointF pt = QPointF(staions[staNum-1].x,staions[staNum-1].y);
    float sin_val = sin(angle*PI/180.f);
    float cos_val = cos(angle*PI/180.f);
    //x 方向偏移
    float dx = 0.0;
    float dy = 0.0;
    QFont ft;
    float t_w = ft.pointSize() * text.size();
    float t_h = ft.pointSize();
    //qDebug()<<"text width:"<<t_w<<"text height:"<<t_h;
    if(fabs(sin_val)<(sqrt(2.0)/2.0))
    {
        //qDebug()<<text<<":x midfy"<< cos_val;
        if(cos_val>0)
            dx += 0.0;
        else
            dx -= t_w;
        dy += t_h;
    }
    //y 方向偏移
    else
    {
        //qDebug()<<text<<":y midfy"<< sin_val;
        if(sin_val >0)
        {
            dy -= 4.0;
        }
        else
            dy += (10.0+t_h+9);
    }
    pt += QPointF(dx,dy);
    return pt;
}


LegendItem::LegendItem(QGraphicsItem *parent)
    :QGraphicsItem(parent),m_color(Qt::white)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

QRectF LegendItem::boundingRect() const
{
    QPointF endF = QPointF(m_screenWidth,Legend_Bouding_Height);
    return QRectF(QPointF(0,0),endF);
}


void LegendItem::setScreenWidth(const float &phyWidth,const int &screenWidth)
{
    m_phyWidth = phyWidth;
    m_screenWidth = screenWidth;
}

float LegendItem::calUnitWidth(const float &phyWidth)  const
{
    float i = log10(phyWidth/2.0f);
    int p = floor(i);
    float ret = pow(10,p);
    return ret;
}

void LegendItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option
                       , QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    painter->setPen(m_color);
    QFont font;
    font.setPointSize(9);
    painter->setFont(font);

    float unit_w = calUnitWidth(m_phyWidth);
    int   width  = unit_w*m_screenWidth/m_phyWidth;

    //-------------------------------------------
    QString m_text;

    if(unit_w>=1000)
    {
        m_text += QString::number(unit_w/1000.f,'g',2) + "km";
    }
    else
    {
        m_text += QString("%1m").arg(unit_w);
    }
    m_text += " , screen width ";

    //-----------
    if(m_phyWidth >= 1000)
    {
        m_text += QString::number(m_phyWidth/1000.f,'g',2)+"km";
    }
    else
    {
        m_text += QString("%1m").arg(m_phyWidth);
    }

    painter->drawRect(0,5,width,10);

    painter->drawText(width+1,15,m_text);
}



