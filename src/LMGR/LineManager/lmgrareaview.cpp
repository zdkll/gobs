#include "lmgrareaview.h"

//视口大小
#define VIEW_WIDTH   viewport()->rect().width()
#define VIEW_HEIGHT  viewport()->rect().height()
#define VIEW_CENTER  viewport()->rect().center()

#include <QMouseEvent>
#include <QWheelEvent>
#include <QBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QDebug>

#include "gsqlfunctions.h"

LmgrAreaView::LmgrAreaView(QWidget *parent)
    :QGraphicsView(parent),m_bMouseTranslate(false)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setBackgroundBrush(QBrush(Qt::black));
    this->setScene(m_scene);

    m_stationInfoDlg = new StationInfoDlg(this);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    m_legendItem = 0;
    m_shotNum = m_recvNum = 0;

    //自定义显示样式
    m_preferenceDlg = new PreferenceDlg(this);
    //默认显示样式颜色
    m_colorMap = m_preferenceDlg->colorMap();

    //设置默认显示项----------
    setShowItemFlags(BaseItem::DeFaultItems);

    connect(m_preferenceDlg,SIGNAL(colorChanged(int,QColor)),this,SLOT(slotColorChanged(int,QColor)));
    connect(m_preferenceDlg,SIGNAL(colorMapChanged(QMap<int,QColor>)),this,SLOT(slotColorMapChanged(QMap<int,QColor>)));
}

LmgrAreaView::~LmgrAreaView()
{
    m_scene->clear();
    if(m_stationInfoDlg)
        delete m_stationInfoDlg;
}

void LmgrAreaView::setStatusBar(StationInfoBar *bar)
{
    m_statusInfoBar = bar;
}

void LmgrAreaView::setRNDepolyments(RNDeployments *rnDepolyments)
{
    m_RNDepolyments = rnDepolyments;
}

void LmgrAreaView::setShowItemFlag(BaseItem::ShowItemFlag flag, bool enabled)
{
    if(enabled)
        m_showItemFlags |= flag;
    else
        m_showItemFlags &= (~flag);
    updateItemsShow();
}

void LmgrAreaView::setShowItemFlags(BaseItem::ShowItemFlags flags)
{
    m_showItemFlags = flags;
    updateItemsShow();
}

void LmgrAreaView::updateItemsShow()
{
    setShowRecvs(m_showItemFlags&BaseItem::RecvItem);
    setShowShots(m_showItemFlags&BaseItem::ShotItem);
    setShowRecvLines(m_showItemFlags&BaseItem::RecvLineItem);
    setShowShotLines(m_showItemFlags&BaseItem::ShotLineItem);
    setShowShotText(m_showItemFlags&BaseItem::ShotTextItem);
    setShowRecvText(m_showItemFlags&BaseItem::RecvTextItem);

    this->scene()->update(scene()->sceneRect());
}
void LmgrAreaView::updateShotStatus()
{
    //接收点三种状态判断
    foreach (StationItem *item, m_shotItems){
        if(item->station().unass == 0)
            item->setColor(m_colorMap.value(Shot_Color));
        else{
            //qDebug()<<"Fired shot";
            item->setColor(m_colorMap.value(Fired_Shot_Color));
        }
    }
}

void LmgrAreaView::updateRecieverStatus()
{
    RemoteNode *rnode = 0;
    int line,station;
    //接收点三种状态判断
    foreach (StationItem *item, m_recvItems) {
        //条件判断
        line = item->station().line;
        station = item->station().sp;
        rnode = m_RNDepolyments->findRemoteNode(line,station);
        if(!rnode)
            item->setColor(m_colorMap.value(Recv_Color));
        else
        {
            //按照优先级
            if(rnode->downloaded){
                item->setColor(m_colorMap.value(Downloaded_Recv_Color));
            }
            else if(rnode->pickedup){
                item->setColor(m_colorMap.value(PickedUp_Recv_Color));
            }
            else if(rnode->depolyed)
            {
                item->setColor(m_colorMap.value(Deployed_Recv_Color));
            }else
                item->setColor(m_colorMap.value(Recv_Color));
        }
    }
}

void LmgrAreaView::searchRecvItem(const float &line,const int &station)
{
    zoomInFull();
    //qDebug()<<line<<station;
    foreach (StationItem *item, m_recvItems) {
        if(item->station().line == line && item->station().sp == station){
            item->setSelected(true);
            this->centerOn(item);
            break;
        }
    }

}

void  LmgrAreaView::createLengend()
{
    m_legendItem = new LegendItem;
    m_scene->addItem(m_legendItem);
    m_legendItem->setPos(mapToScene(5,VIEW_HEIGHT-40));

    QPointF p1 = mapToScene(0,0);
    QPointF p2 = mapToScene(VIEW_WIDTH,0);
    QLineF lineF(p1,p2);

    m_legendItem->setScreenWidth(lineF.length(),VIEW_WIDTH);
}

void LmgrAreaView::setAreaData(AreaDataInfo *data)
{
    this->resetTransform();
    m_DataInfo = data;
    m_scene->clear();//清除所有项
    m_legendItem = 0;
    if(m_DataInfo == 0){
        m_scene->setSceneRect(0,0,0,0);
        m_sceneRectF = QRectF(0,0,0,0);
        return;
    }
    //debug test--------

    //计算坐标范围
    QRectF rect;
    LmgrPublic::calAreaScope(m_DataInfo,rect);
    qDebug()<<"area scope:"<<rect;
    m_sceneRectF  = rect.adjusted(-Scene_Rect_Margin,-Scene_Rect_Margin
                                  ,Scene_Rect_Margin, Scene_Rect_Margin);
    m_scene->setSceneRect(m_sceneRectF);

    m_shotItems.clear();
    m_recvItems.clear();

    m_shotLineItems.clear();
    m_recvLineItems.clear();

    m_shotTextItems.clear();
    m_recvTextItems.clear();

    //添加炮站点-----------------
    m_shotNum = addStations(m_DataInfo->shotStationInfo
                            ,m_colorMap.value(Shot_Color),m_shotItems,m_shotTextItems);
    //添加接收站点-------------------
    m_recvNum = addStations(m_DataInfo->recvStationInfo
                            ,m_colorMap.value(Recv_Color),m_recvItems,m_recvTextItems);

    addLineNumber(m_DataInfo->recvStationInfo
                  ,m_colorMap.value(Recv_Line_Color),m_recvLineItems);

    addLineNumber(m_DataInfo->shotStationInfo
                  ,m_colorMap.value(Shot_Line_Color),m_shotLineItems);

    //this->scale(1.0,-1.0);

    createLengend();

    //初始默认显示比例1.0
    zoomInFull();

    //刷新显示隐藏
    updateItemsShow();

    //刷新接受点颜色状态
    updateRecieverStatus();
    //刷新炮点颜色状态
    updateShotStatus();
}

int LmgrAreaView::addStations(StationInfo *stationsInfo,const QColor &color,QList<StationItem *> &m_items
                              ,QList<QGraphicsSimpleTextItem *> &m_textItems)
{
    int totalNum = 0;
    StaLine  *staLines;
    Station  *stations;
    int lineNum;
    int staionNum;
    staLines = stationsInfo->staLines;
    lineNum  = stationsInfo->lineNum;

    int top = m_sceneRectF.top();
    int bottom = m_sceneRectF.bottom();
    int x,y;

    QColor textColor;
    int    step = 1;
    if(stationsInfo->type == "S"){
        textColor = m_colorMap.value(Shot_Text_Color);
        step  = 5;
    }
    else{
        textColor = m_colorMap.value(Recv_Text_Color);
        step  = 3;
    }

    for(int i= 0;i<lineNum;i++)
    {
        stations = staLines[i].stations;
        staionNum = staLines[i].staNum;
        totalNum += staionNum;
        for(int j = 0; j<staionNum;j++){
            StationItem *item = new StationItem(stations[j]);
            x = stations[j].x;
            y = stations[j].y;

            item->setColor(color);
            item->setRect(x,top+bottom-y,10,10);
            m_scene->addItem(item);
            m_items.append(item);

            if(j%step==0){
                QGraphicsSimpleTextItem *textItem
                        = new QGraphicsSimpleTextItem(QString::number(stations[j].sp));
                textItem->setPos(x,top+bottom-y);
                textItem->setPen(QPen(textColor));
                m_scene->addItem(textItem);
                m_textItems.append(textItem);
            }
        }
    }
    return totalNum;
}

int LmgrAreaView::addLineNumber(StationInfo *stationsInfo,const QColor &color,QList<LineItem *> &m_items)
{
    StaLine  *staLines;
    int lineNum;
    staLines = stationsInfo->staLines;
    lineNum  = stationsInfo->lineNum;
    QString text = stationsInfo->type;

    int top = m_sceneRectF.top();
    int bottom = m_sceneRectF.bottom();
    QPointF pt0;
    for(int i= 0;i<lineNum;i++)
    {
        LineItem *item = new LineItem(staLines[i]);
        item->setColor(color);
        pt0 = LineItem::calPos(staLines[i]);
        item->setPos(pt0.x(),top + bottom - pt0.y());
        m_scene->addItem(item);

        m_items.append(item);
    }
    return lineNum;
}

void LmgrAreaView::zoomIn()
{
    QTransform transf = transform().scale(1.0 + Scale_Delta,1.0 + Scale_Delta);
    if(transf.map(QLineF(0,0,1.0,0)).length()>Scene_Scale_Max)
        return;
    scale(1.0 + Scale_Delta,1.0 + Scale_Delta);
}

void LmgrAreaView::zoomOut()
{
    QTransform transf = transform().scale(1.0/(1.0 + Scale_Delta),1.0/(1.0 + Scale_Delta));
    if(transf.map(QLineF(0,0,1.0,0)).length()<Scene_Scale_Min)
        return;

    scale(1.0/(1.0 + Scale_Delta),1.0/(1.0 + Scale_Delta));
}



void LmgrAreaView::zoomInFull()
{
    //视口坐标和场景坐标比例为1
    float dx = (m_sceneRectF.width() - VIEW_WIDTH)/2.0f;
    float dy = (m_sceneRectF.height() - VIEW_HEIGHT)/2.0f;

    this->fitInView(m_sceneRectF.adjusted(dx,dy,-dx,-dy),Qt::KeepAspectRatioByExpanding);
}

void LmgrAreaView::zoomOutFull()
{
    this->fitInView(m_sceneRectF,Qt::KeepAspectRatio);
}

void LmgrAreaView::setShowRecvs(const bool &visible)
{
    qDebug()<<"set show rcvs:"<<visible;
    foreach (QGraphicsItem *item, m_recvItems) {
        item->setVisible(visible);
    }
}

void LmgrAreaView::setShowShots(const bool &visible)
{
    qDebug()<<"set show shots:"<<visible;
    foreach (QGraphicsItem *item, m_shotItems) {
        item->setVisible(visible);
    }
}

void LmgrAreaView::setShowRecvLines(const bool &visible)
{
    foreach (QGraphicsItem *item, m_recvLineItems) {
        item->setVisible(visible);
    }
}

void LmgrAreaView::setShowShotLines(const bool &visible)
{
    foreach (QGraphicsItem *item, m_shotLineItems) {
        item->setVisible(visible);
    }
}

void LmgrAreaView::setShowShotText(const bool &visible)
{
    foreach (QGraphicsSimpleTextItem *item, m_shotTextItems) {
        item->setVisible(visible);
    }
}
void LmgrAreaView::setShowRecvText(const bool &visible)
{
    foreach (QGraphicsSimpleTextItem *item, m_recvTextItems) {
        item->setVisible(visible);
    }
}

void LmgrAreaView::setPreference()
{
    m_preferenceDlg->exec();
}

void LmgrAreaView::slotColorChanged(const int &id,const QColor &color)
{
    m_colorMap.insert(id,color);
    int line,station;
    RemoteNode *rnode = 0;
    switch(id)
    {
    case Backdroud_Color:
        this->setBackgroundBrush(QBrush(color));
        break;
    case Shot_Color:
        foreach (StationItem *item, m_shotItems) {
            if(item->station().unass == 0)
                item->setColor(color);
        }
        break;
    case Fired_Shot_Color:
        foreach (StationItem *item, m_shotItems) {
            if(item->station().unass == 1)
                item->setColor(color);
        }
        break;
    case Recv_Color:
        foreach (StationItem *item, m_recvItems) {
            item->setColor(color);
        }
        break;
    case Shot_Line_Color:
        foreach (LineItem *item, m_shotLineItems) {
            item->setColor(color);
        }
        break;
    case Recv_Line_Color:
        foreach (LineItem *item, m_recvLineItems) {
            item->setColor(color);
        }
        break;
    case Legend_Color:
        if(m_legendItem)
            m_legendItem->setColor(color);
        break;
    case Deployed_Recv_Color:
        foreach (StationItem *item, m_recvItems) {
            //条件判断
            line = item->station().line;
            station = item->station().sp;
            rnode = m_RNDepolyments->findRemoteNode(line,station);
            if(rnode){
                if(rnode->depolyed&&(!rnode->pickedup))
                    item->setColor(color);
            }
        }
        break;
    case PickedUp_Recv_Color:
        foreach (StationItem *item, m_recvItems) {
            //条件判断
            line = item->station().line;
            station = item->station().sp;
            rnode = m_RNDepolyments->findRemoteNode(line,station);
            if(rnode){
                if(rnode->pickedup)
                    item->setColor(color);
            }
        }
        break;
    case Downloaded_Recv_Color:
        foreach (StationItem *item, m_recvItems) {
            //条件判断
            line = item->station().line;
            station = item->station().sp;
            rnode = m_RNDepolyments->findRemoteNode(line,station);
            if(rnode){
                if(rnode->downloaded)
                    item->setColor(color);
            }
        }
        break;
    case Recv_Text_Color:
        foreach (QGraphicsSimpleTextItem *item, m_recvTextItems) {
            item->setPen(QPen(color));
        }
        break;
    case Shot_Text_Color:
        foreach (QGraphicsSimpleTextItem *item, m_shotTextItems) {
            item->setPen(QPen(color));
        }
        break;
    }
}

void LmgrAreaView::slotColorMapChanged(const QMap<int,QColor> &map)
{
    m_colorMap = map;

    //Backdroud_Color
    this->setBackgroundBrush(QBrush(m_colorMap.value(Backdroud_Color)));

    //Shot_Color
    updateShotStatus();

    // Shot_Line_Color:
    foreach (LineItem *item, m_shotLineItems) {
        item->setColor(m_colorMap.value(Shot_Line_Color));
    }

    // Recv_Line_Color:
    foreach (LineItem *item, m_recvLineItems) {
        item->setColor(m_colorMap.value(Recv_Line_Color));
    }

    // Legend_Color:
    if(m_legendItem)
        m_legendItem->setColor(m_colorMap.value(Legend_Color));

    //shot text
    foreach (QGraphicsSimpleTextItem *item, m_shotTextItems) {
        item->setPen(QPen(m_colorMap.value(Shot_Text_Color)));
    }
    //recv text
    foreach (QGraphicsSimpleTextItem *item, m_recvTextItems) {
        item->setPen(QPen(m_colorMap.value(Recv_Text_Color)));
    }
    //Recv_Color: 更新接收点状态
    updateRecieverStatus();

}


void LmgrAreaView::wheelEvent(QWheelEvent *e)
{
    QPoint delta = e->angleDelta();
    delta.y()>0?zoomIn():zoomOut();
}

void LmgrAreaView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_bMouseTranslate = true;
    }else if(e->button() == Qt::RightButton)
    {
        //查看信息
        QGraphicsItem *item = itemAt(e->pos());
        if(item != NULL)
        {
            StationItem *staItem = dynamic_cast<StationItem *>(item);
            if(staItem){
                if(staItem->stationId() == 'R')
                {
                    //查找对应的 remote node
                    const RemoteNode *rnode = m_RNDepolyments->findRemoteNode(
                                staItem->station().line
                                ,staItem->station().sp);
                    m_stationInfoDlg->showStationInfo(staItem->station(),rnode);
                }
                else if(staItem->stationId() == 'S'){
                    m_stationInfoDlg->showStationInfo(staItem->station());
                }

            }
        }
    }
    m_lastMousePos = e->pos();
    QGraphicsView::mousePressEvent(e);
}

void LmgrAreaView::mouseDoubleClickEvent(QMouseEvent *e)
{
    //查看信息
    QGraphicsItem *item = itemAt(e->pos());
    if(item != NULL)
    {
        StationItem *staItem = dynamic_cast<StationItem *>(item);
        if(staItem){
            if(staItem->stationId() == 'R')
            {
                //查找对应的 remote node
                const RemoteNode *rnode =m_RNDepolyments->findRemoteNode(
                            staItem->station().line
                            ,staItem->station().sp);
                m_stationInfoDlg->showStationInfo(staItem->station(),rnode);
            }
            else if(staItem->stationId() == 'S'){
                m_stationInfoDlg->showStationInfo(staItem->station());
            }
        }
    }
}


void LmgrAreaView::paintEvent(QPaintEvent *e)
{
    QGraphicsView::paintEvent(e);
    if(m_legendItem)
    {
        m_legendItem->setPos(mapToScene(10,VIEW_HEIGHT-20));

        QPointF p1 = mapToScene(0,0);
        QPointF p2 = mapToScene(VIEW_WIDTH,0);
        QLineF lineF(p1,p2);

        m_legendItem->setScreenWidth(lineF.length(),VIEW_WIDTH);
    }
}

void LmgrAreaView::mouseMoveEvent(QMouseEvent *e)
{
    //显示当前站点信息-----------
    QGraphicsItem *item = itemAt(e->pos());
    if(item != NULL)
    {
        StationItem *staItem = dynamic_cast<StationItem *>(item);
        if(staItem)
        {
            m_statusInfoBar->setStation(staItem->station());

            //根据线号和站点号查询
            int station = staItem->station().sp;
            int line    = staItem->station().line;
            const RemoteNode *remoteNode = m_RNDepolyments->findRemoteNode(line,station);
            if(remoteNode)
            {
                m_statusInfoBar->setRemoteNode(remoteNode->device);
            }
        }
        //        else m_statusInfoBar->clear();
    }

    //-----------------------
    QPointF mouseDelta = mapToScene(e->pos()) - mapToScene(m_lastMousePos);
    //translate
    if(m_bMouseTranslate){
        //计算比例
        float m_scale = transform().map(QLineF(0,0,1.0,0)).length();
        mouseDelta *= m_scale;

        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        QPoint newCenter(VIEW_WIDTH/2.0f - mouseDelta.x(),  VIEW_HEIGHT/2.0f - mouseDelta.y());
        centerOn(mapToScene(newCenter));

        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
    m_lastMousePos = e->pos();

    QGraphicsView::mouseMoveEvent(e);
}

void LmgrAreaView::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_bMouseTranslate = false;
    }
    QGraphicsView::mouseReleaseEvent(e);
}


//------------------------------------
StationInfoBar::StationInfoBar(QStatusBar *bar)
    :m_statusBar(bar)
{
    setUpBar();
}

void StationInfoBar::setUpBar()
{
    //ru
    QFont font;
    int w = 15*font.pointSize();
    QLabel *ruNameLbl = new QLabel("RU:");
    m_ruLbl = new QLabel;
    m_ruLbl->setMinimumWidth(w);
    m_statusBar->addWidget(ruNameLbl);
    m_statusBar->addWidget(m_ruLbl);

    //Line
    int w1 = 9*font.pointSize();
    QLabel *lineNameLbl = new QLabel("Line:");
    m_lineLbl = new QLabel;
    m_lineLbl->setMinimumWidth(w1);
    m_statusBar->addWidget(lineNameLbl);
    m_statusBar->addWidget(m_lineLbl);

    //sta
    QLabel *staNameLbl = new QLabel("Station:");
    m_staLbl = new QLabel;
    m_staLbl->setMinimumWidth(w1);
    m_statusBar->addWidget(staNameLbl);
    m_statusBar->addWidget(m_staLbl);

    //north
    QLabel *northNameLbl = new QLabel("Northing:");
    m_northLbl = new QLabel;
    m_northLbl->setMinimumWidth(w1);
    m_statusBar->addWidget(northNameLbl);
    m_statusBar->addWidget(m_northLbl);

    //east
    QLabel *eastNameLbl = new QLabel("Easting:");
    m_eastLbl = new QLabel;
    m_eastLbl->setMinimumWidth(w1);
    m_statusBar->addWidget(eastNameLbl);
    m_statusBar->addWidget(m_eastLbl);
}

void StationInfoBar::setStation(const Station &staion)
{
    m_ruLbl->setText(QString("%1").arg(staion.unit));
    m_lineLbl->setText(QString("%1%2").arg(staion.Id).arg(staion.line));
    m_staLbl->setText(QString::number(staion.sp));
    m_northLbl->setText(QString::number(staion.y));
    m_eastLbl->setText(QString::number(staion.x));
}

void StationInfoBar::setRemoteNode(const QString &device)
{
    m_ruLbl->setText(device);
}

void StationInfoBar::clear()
{
    m_ruLbl->clear();
    m_lineLbl->clear();
    m_staLbl->clear();
    m_northLbl->clear();
    m_eastLbl->clear();
}


//-----------------------------------------
StationInfoDlg::StationInfoDlg(QWidget *parent)
    :QDialog(parent)
{
    initDlg();
}

void StationInfoDlg::showStationInfo(const Station &station,const RemoteNode *rnode)
{
    //显示炮点信息
    if(station.Id == 'S')
    {
        this->setWindowTitle("Shot");
        m_staLabel->setText(QString("Shot Line:%1, Station: %2\n"
                                    "Easting:%3, Northing:%4")
                            .arg(station.line)
                            .arg(station.sp)
                            .arg(station.x)
                            .arg(station.y));
        m_table->hide();
        this->resize(274,40);
    }
    //显示接受点信息
    if(station.Id == 'R')
    {
        this->setWindowTitle("Reciever");

        m_staLabel->setText(QString("Reciever Line:%1, Station: %2\n"
                                    "PrePlan Easting:%3, Northing:%4")
                            .arg(station.line)
                            .arg(station.sp)
                            .arg(station.x)
                            .arg(station.y));
        m_table->show();
        if(rnode){
            m_table->setRowCount(1);
            m_table->setItem(0,0,new QTableWidgetItem(rnode->device));
            m_table->setItem(0,1,new QTableWidgetItem(rnode->depolyedTime));
            m_table->setItem(0,2,new QTableWidgetItem(rnode->pickedupTime));
            m_table->setItem(0,3,new QTableWidgetItem(QString::number(rnode->actualX)));
            m_table->setItem(0,4,new QTableWidgetItem(QString::number(rnode->actualY)));
            m_table->setItem(0,5,new QTableWidgetItem(QString::number(rnode->actualZ)));
        }else{
            m_table->removeRow(0);
        }
        this->resize(835,147);
    }
    this->show();
}

//#include <QResizeEvent>
//void StationInfoDlg::resizeEvent(QResizeEvent *e)
//{
//    qDebug()<<e->size();
//}

void StationInfoDlg::initDlg()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    m_staLabel = new QLabel;

    vLayout->addWidget(m_staLabel);

    m_table = new QTableWidget(this);
    initTable();

    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setShowGrid(false);
    vLayout->addWidget(m_table);

    this->setLayout(vLayout);
}

void StationInfoDlg::initTable()
{
    QStringList headLbls;
    headLbls<<"Remote Node"
           <<"Deploy Time"
          <<"Pickup Time"
         <<"Actual X"
        <<"Actual Y"
       <<"Actual Z";

    m_table->setColumnCount(6);
    m_table->setColumnWidth(0,100);
    m_table->setColumnWidth(1,200);
    m_table->setColumnWidth(2,200);
    m_table->setRowCount(1);
    m_table->setHorizontalHeaderLabels(headLbls);
}
