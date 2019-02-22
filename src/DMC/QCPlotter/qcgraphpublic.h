#ifndef QCGRAPHPUBLIC_H
#define QCGRAPHPUBLIC_H

#include <QWidget>
#include <QScrollBar>
#include <QDialog>

#include "graphbase_2d.h"

//图像缩放显示数据范围信息
class GraphDataInfo
{
public:
    GraphDataInfo()
        :horiDataNum(0)
        ,vertDataNum(0)
        ,m_zoomX(1.0)
        ,m_zoomY(1.0)
        ,currentHorizIdxBeg(0)
        ,currentVertIdxBeg(0){}

private:
    int    horiDataNum;  //总的道数
    int    vertDataNum;  //总的采样点数

    //缩放倍数
    float  m_zoomX;
    float  m_zoomY;
    int currentHorizIdxBeg; //当前图像x轴第一道数据下标（原始图像第一道下标是0，一般不超过x数据总数-1）
    int currentVertIdxBeg;//当前图像y轴第一道数据下标（原始图像第一道下标是0，一般不超过y轴取样总数-1）

    //当前显示道间隔(逻辑坐标)
    int   traceStep;

    friend class QCGraphBase;
public:
    //计算公式
    int getZoomedVertNum(){return (m_zoomY * float(vertDataNum));} //实际当前可视的横向坐标取样数
    int getZoomedHorizNum(){return (m_zoomX * float(horiDataNum));} //实际当前可视的纵向坐标取样数

    float getLimitZoomX(){return LIMIT_SCALE_DATA_IN_MAP/float(horiDataNum);} //极限倍数，不会允许将放大倍数设置到比最大倍数更大，目前是一个方向上至少1个数据
    float getLimitZoomY(){return LIMIT_SCALE_DATA_IN_MAP/float(vertDataNum);} //极限倍数，不会允许将放大倍数设置到比最大倍数更大，目前Y方向上至少1个数据

    //rectWidth/rectHeight 画面宽度,cutX_1/cutX_2,放大时截取的两个x坐标
    //horizNum/vertNum     原始数据横向取样数
    void zoomX(const int &rectWidth, const int &cutX_1, const int &cutX_2 );
    void zoomY(const int &rectHeight, const int &cutY_1, const int &cutY_2 );
    void zoomOut();
    void zoomIn(QRect graphRect,QRect rect);

    //根据屏幕宽度和设置的显示道间隔计算合适的插值显示间隔
    int calTraceStep(int width, int step)
    {
        traceStep = ImageBaseFunction::getGoodStep(width,step,getZoomedHorizNum());
        return traceStep;
    }
    //返回总的显示道数
    int disTraceNum(){
        int disTraces = getZoomedHorizNum()/traceStep;
        return disTraces>0?disTraces:1;}
};

//鼠标功能拖动,包括缩放和分析范围道数选择
class Mouse_Rect_Drawer
{
public:
    Mouse_Rect_Drawer(QColor color = Qt::red);

    void setPointBegin(QPoint pt);
    void setPointEnd(QPoint pt);
    void setGraphRect(const QRect &rect);

    QRect drawRect(){return m_grapRect.intersected(QRect(m_beginPt,m_endPt));}

    //绘制------------------
    void show(QPainter &p);
    //是否可见，颜色
    void setVisible(bool);
    void setColor(QColor color);

    friend class QCGraphBase;
private:
    bool    m_visible;   //设置是否可见
    QPoint  m_beginPt;//mouse press point
    QPoint  m_endPt;  //mouse move point
    QRect   m_grapRect;
    //边框颜色
    QColor  m_color;
};

namespace Ui {
class TraceHeadDlg;
}
//显示道头信息对话框
class TraceHeadDlg : public QDialog
{
    Q_OBJECT
public:
    TraceHeadDlg(QWidget *parent = 0);
    void setHeaders(TraceHead *headers);
    void showTrace(const int &idx);
    void setCurrentTrace(const int &idx){m_currentTrace = idx;}
    int  currentTrace() const {return m_currentTrace;}

    //组装道头信息，名称-描述
    static void getTraceHeadInfo(QStringList &headInfos);

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void on_pushButton_clicked();

private:
    void initDlg();

private:
    Ui::TraceHeadDlg *ui;
    TraceHead        *m_headers;
    int               m_currentTrace;
};

#endif // QCGRAPHPUBLIC_H
