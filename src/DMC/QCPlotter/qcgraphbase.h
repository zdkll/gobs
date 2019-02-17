#ifndef QCGRAPHBASE_H
#define QCGRAPHBASE_H

#include <QWidget>
#include <QStatusBar>
#include <QPixmap>

#include "qcgraphpublic.h"

class QCGraphBase : public QWidget
{
    Q_OBJECT
public:
    explicit QCGraphBase(QWidget *parent = 0);
    ~QCGraphBase();

    void Initial(QStatusBar *statusBar,QScrollBar *horiBar,
                 QScrollBar *vertzBar,GatherInfo *gatherInfo
                 ,GraphAttribute *graphAttribute);

    void setData(TraceHead *headers,char *data);
    void zoomOut();
    void zoomIn(QRect rect);
    void scrollValueChanged(Qt::Orientation,int value);

    void setFixedTimeScope(QPair<int,int> timeScope);

    //几种绘制模式命令----
    void draw();
    void objDraw();
    void reDraw();

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

    //鼠标事件
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool doInterpolationWG();
    bool doDrawPixmap();//绘制画布
    void doDrawWG(int i, QPainter &p,int fBase, int height
                  , float traceWidth, QPointF *,float valMax);
    void doDrawVA(int i, QPainter &p,int fBase, int height,
                  float traceWidth, float valMax,eBrushMode brushMode);
    void doDrawWGVA(int i, QPainter &p,int fBase, int height,
                    float traceWidth,QPointF *points,float valMax,eBrushMode brushMode);
    //绘制刻度--------------------------
    void drawScale(QPainter *painter);
    void drawHScale(QPainter *painter);
    void drawVScale(QPainter *painter);
    //绘制水平刻度标签和title-------------
    void drawHLabels(QPainter *painter);
    void drawVLabels(QPainter *painter);
    void drawBottomLabels(QPainter *painter);
    //鼠标位置横纵向序号------------------
    int traceAt(const QPoint &pt);//包括没有显示的道
    int disTraceAt(const QPoint &pt);//忽略隐藏没有显示的道
    int nsAt(const QPoint &pt);
    void showCoords(const QPoint &pt);
    //绘制分析道
    void doDrawAnaTrace(QPainter *p);

private:
    GatherInfo     *m_gatherInfo;
    QStatusBar     *m_statusBar;
    GraphAttribute *m_graphAttribute;

    //显示范围信息
    GraphDataInfo   m_graphDataInfo;

    //原始数据
    float       *m_data;
    TraceHead   *m_headers;

    float  m_ValMaxG,m_ValMinG; //全局最大值最小值
    float *m_traceMaxValue,*m_traceMinValue;//各道最大值最小值数组，数组长度为插值之后的道数

    //插值后数据
    float       *m_DataInterWG;
    float       *m_AnalyInterWG;

    //图像区
    QRect        m_GraphRect;
    bool         m_mousePressed;

    ePaintAction m_paintAction;

    //双缓冲，保存在内存中的画布
    QPixmap      m_pixCanvas;

    Mouse_Rect_Drawer m_mouseRectDrawer;
    QScrollBar       *m_horiBar,*m_vertBar;
    TraceHeadDlg     *m_traceHeadDlg;
};

#endif // QCGRAPHBASE_H
