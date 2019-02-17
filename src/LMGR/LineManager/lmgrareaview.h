#ifndef LMGRAREAVIEW_H
#define LMGRAREAVIEW_H

#define Scene_Rect_Margin      100

//每次缩放系数增量
#define Scale_Delta            0.1

//场景缩放比例
#define Scene_Scale_Min       0.1
#define Scene_Scale_Max       10

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStatusBar>
#include <QLabel>
#include <QDialog>

#include "stationitem.h"
#include "lmgrunit.h"

//设备投放回收信息管理-------
#include "rndeployments.h"

class StationInfoBar;
class QTableWidget;
class StationInfoDlg;
//图像显示类-----------------------------
class LmgrAreaView : public QGraphicsView
{
    Q_OBJECT
public:
    LmgrAreaView(QWidget *parent = 0);
    ~LmgrAreaView();

    void setStatusBar(StationInfoBar *bar);

    void setRNDepolyments(RNDeployments *rnDepolyments);

    void setAreaData(AreaDataInfo *data);


    void setShowItemFlag(BaseItem::ShowItemFlag flag, bool enabled = true);
    void setShowItemFlags(BaseItem::ShowItemFlags flags);

    //以10% 速度缩放
    void zoomIn();
    void zoomOut();

    //缩放比列调到1
    void zoomInFull();
    void zoomOutFull();//恢复显示全部

    //显示隐藏控制
    void setShowRecvs(const bool&);
    void setShowShots(const bool&);
    void setShowRecvLines(const bool&);
    void setShowShotLines(const bool&);
    void setShowShotText(const bool&);
    void setShowRecvText(const bool&);

    const int &shotNum(){return   m_shotNum;}
    const int &recvNum(){return   m_recvNum;}

    void setPreference();

    //更新接受点状态
    void updateRecieverStatus();

    //更新炮点状态
    void updateShotStatus();

    void searchRecvItem(const float &line,const int &station);

protected slots:
    //color settings
    void slotColorChanged(const int &id,const QColor &color);
    void slotColorMapChanged(const QMap<int,QColor> &map);

protected:
    void wheelEvent(QWheelEvent *e);//缩放
    void mousePressEvent(QMouseEvent *e);//选择，移动
    void mouseMoveEvent(QMouseEvent *e); //移动
    void mouseReleaseEvent(QMouseEvent *e);

    void mouseDoubleClickEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *e);

private:
    //添加站点，返回添加的站点数
    int addStations(StationInfo *stationsInfo,const QColor &color,QList<StationItem *> &m_items
                    ,QList<QGraphicsSimpleTextItem *> &m_textItems);

    int addLineNumber(StationInfo *stationsInfo,const QColor &color,QList<LineItem *> &m_items);

    void updateItemsShow();

    void  createLengend();

private:
    BaseItem::ShowItemFlags   m_showItemFlags;
    AreaDataInfo   *m_DataInfo;

    QGraphicsScene *m_scene;
    QRectF          m_sceneRectF;//场景项全部的区域范围
    QPoint          m_lastMousePos;
    bool            m_bMouseTranslate;

    StationInfoBar *m_statusInfoBar;//状态栏

    StationInfoDlg *m_stationInfoDlg;//信息对话框

    QList<StationItem *> m_shotItems,m_recvItems;
    QList<LineItem *>    m_shotLineItems,m_recvLineItems;
    QList<QGraphicsSimpleTextItem *> m_shotTextItems,m_recvTextItems;
    LegendItem     *m_legendItem;

    int             m_shotNum,m_recvNum;

    PreferenceDlg  *m_preferenceDlg;
    //颜色样式设置
    QMap<int,QColor>  m_colorMap;

    //设备投放和回收信息
    RNDeployments    *m_RNDepolyments;
};


class StationInfoBar
{
public:
    StationInfoBar(QStatusBar *bar);

    void setUpBar();

    void setStation(const Station &staion);

    void setRemoteNode(const QString &device);

    void clear();

private:
    QStatusBar *m_statusBar;

    QLabel     *m_ruLbl,*m_lineLbl,*m_staLbl,*m_northLbl,*m_eastLbl;
};

class StationInfoDlg : public QDialog
{
public:
    StationInfoDlg(QWidget *parent = 0);

    void showStationInfo(const Station &sta,const RemoteNode *rnode = 0);
protected:
    //void resizeEvent(QResizeEvent *e);

private:
    void initDlg();
    void initTable();

    QLabel       *m_staLabel;
    QTableWidget *m_table;
};

#endif // LMGRAREAVIEW_H
