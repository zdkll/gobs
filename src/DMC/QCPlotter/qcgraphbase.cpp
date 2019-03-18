#include "qcgraphbase.h"

#include <QPainter>
#include <QMouseEvent>
#include "toolsalgo.h"
#include "graphfunctions.h"

QCGraphBase::QCGraphBase(QWidget *parent)
    : QWidget(parent),m_gatherInfo(0)
    ,m_paintAction(NODRAW)
{
    m_DataInterWG   = 0;
    m_AnalyInterWG  = 0;
    m_traceMaxValue = 0;
    m_traceMinValue = 0;
    m_headers       = 0;
    m_data          = 0;

    m_gatherInfo    = 0;
    m_mousePressed  = false;
    this->setAttribute(Qt::WA_StaticContents);

    m_traceHeadDlg  = new TraceHeadDlg(this);
    this->setMouseTracking(true);
}

QCGraphBase::~QCGraphBase()
{
    if(m_DataInterWG)
        delete[] m_DataInterWG;
    if(m_AnalyInterWG)
        delete[] m_AnalyInterWG;
    if(m_traceMaxValue)
        delete[] m_traceMaxValue;
    if(m_traceMinValue)
        delete[] m_traceMinValue;
}

void QCGraphBase::Initial(QStatusBar *statusBar,QScrollBar *horiBar,
                          QScrollBar *vertzBar,GatherInfo *gatherInfo
                          ,GraphAttribute *graphAttribute)
{
    m_statusBar  = statusBar;
    m_gatherInfo = gatherInfo;
    m_graphAttribute = graphAttribute;

    m_horiBar = horiBar;
    m_vertBar = vertzBar;
}

void QCGraphBase::setData(TraceHead *headers,char *data)
{
    m_data    = (float*)data;
    m_headers = headers;
    //m_gatherInfo = gatherInfo;

    m_traceHeadDlg->setHeaders(m_headers);

    //新的道集，显示数据信息初始化
    m_graphDataInfo.horiDataNum = m_gatherInfo->NTraces;
    m_graphDataInfo.vertDataNum = m_gatherInfo->Ns;
    qDebug()<<"ntraces:"<< m_gatherInfo->NTraces
           <<"ns:"<<m_gatherInfo->Ns;
    //统计数据极值
    if(m_traceMaxValue)
        delete[] m_traceMaxValue;
    if(m_traceMinValue)
        delete[] m_traceMinValue;
    m_traceMaxValue = new float[m_gatherInfo->NTraces];
    m_traceMinValue = new float[m_gatherInfo->NTraces];
    ImageBaseFunction::getTraceExtremumFromData(m_data,m_gatherInfo->NTraces
                                                ,m_gatherInfo->Ns
                                                ,m_traceMinValue,m_traceMaxValue
                                                ,&m_ValMinG,&m_ValMaxG);
    //    qDebug()<<"global max:"<<m_ValMaxG
    //           <<"global min:"<<m_ValMinG;
    //恢复到原始范围
    zoomOut();
}

void QCGraphBase::zoomOut()
{
    m_graphDataInfo.zoomOut();
    this->draw();

    //更新滚动条--------
    m_horiBar->setRange(0,0);
    m_horiBar->setValue(0);
    m_vertBar->setRange(0,0);
    m_vertBar->setValue(0);
}

void QCGraphBase::zoomIn(QRect rect)
{
    if(!rect.isValid())
        return;

    QPoint pt =  rect.bottomRight() - rect.topLeft();
    if(pt.manhattanLength()<Zoom_MIN_LENGTH)
    {
        return this->zoomOut();
    }

    //数据信息范围，执行缩放
    m_graphDataInfo.zoomIn(m_GraphRect
                           ,rect.translated(-Left_Scale_Margin,-Top_Scale_Margin));
    this->draw();

    //更新滚动条--------
    m_horiBar->setRange(0,
                        m_graphDataInfo.horiDataNum-m_graphDataInfo.getZoomedHorizNum());
    m_horiBar->setPageStep(m_graphDataInfo.getZoomedHorizNum());
    m_horiBar->setValue(m_graphDataInfo.currentHorizIdxBeg);

    m_vertBar->setRange(0,
                        m_graphDataInfo.vertDataNum - m_graphDataInfo.getZoomedVertNum());
    m_vertBar->setPageStep(m_graphDataInfo.getZoomedVertNum());
    m_vertBar->setValue(m_graphDataInfo.currentVertIdxBeg);
}

void QCGraphBase::scrollValueChanged(Qt::Orientation orientation,int value)
{
    if(orientation == Qt::Horizontal)
    {
        m_graphDataInfo.currentHorizIdxBeg = value;
    }
    if(orientation == Qt::Vertical)
    {
        m_graphDataInfo.currentVertIdxBeg = value;
    }
    this->draw();
}

void QCGraphBase::setFixedTimeScope(QPair<int,int> timeScope)
{
    //设置Y方向显示范围
    if(m_gatherInfo){
        //起始坐标
        int startTime = timeScope.first;
        int beginIdx  = startTime*1000/m_gatherInfo->sampleInterval;
        if(beginIdx>=m_gatherInfo->Ns)
            return;
        m_graphDataInfo.currentVertIdxBeg = beginIdx;

        //缩放系数
        int endTime = timeScope.second;
        int endIdx  = endTime*1000/m_gatherInfo->sampleInterval;
        endIdx = endIdx>= m_gatherInfo->Ns?(m_gatherInfo->Ns-1):endIdx;
        m_graphDataInfo.m_zoomY = float(endIdx-beginIdx+1)/float(m_graphDataInfo.vertDataNum);

        m_vertBar->setRange(0,
                            m_graphDataInfo.vertDataNum - m_graphDataInfo.getZoomedVertNum());
        m_vertBar->setPageStep(m_graphDataInfo.getZoomedVertNum());
        m_vertBar->setValue(m_graphDataInfo.currentVertIdxBeg);

        draw();
    }
}

void QCGraphBase::draw()
{
    m_paintAction = DRAW;
    this->update();
}

void QCGraphBase::objDraw()
{
    m_paintAction = OBJDRAW;
    this->update();
}

void QCGraphBase::reDraw()
{
    m_paintAction = REDRAW;
    this->update();
}

void QCGraphBase::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    m_paintAction = DRAW;

    //更新图像区范围------------------
    m_GraphRect = QRect(Left_Scale_Margin,Top_Scale_Margin
                        ,width()-Left_Scale_Margin-Right_Scale_Margin
                        ,height()-Top_Scale_Margin-Bottom_Scale_Margin);
}

void QCGraphBase::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    //绘制绘图区和刻度区
    QPainter painter(this);
    painter.fillRect(m_GraphRect,Qt::white);
    painter.end();

    //是否有数据
    if(m_gatherInfo->NTraces<1 || !m_data)
        return;

    //1 插值
    if(m_paintAction == DRAW)
    {
        //qDebug()<<"draw----------------------";
        // WG插值
        if(!doInterpolationWG())
        {
            throw "Interpolation Failed.";
            exit(EXIT_FAILURE);
        }

        // 绘图
        doDrawPixmap();

    }

    if(m_paintAction == OBJDRAW)
    {
        // 绘图
        doDrawPixmap();
    }

    //绘制画布
    painter.begin(this);
    if(m_paintAction != NODRAW)
    {
        painter.drawPixmap(Left_Scale_Margin,Top_Scale_Margin
                           ,m_pixCanvas);
    }

    //绘制刻度painter
    drawScale(&painter);

    //拖动框绘制------
    m_mouseRectDrawer.show(painter);

    m_paintAction = REDRAW;
    painter.end();
}

void QCGraphBase::mouseMoveEvent(QMouseEvent *e)
{
    //没有数据
    if(!m_data)
        return QWidget::mouseMoveEvent(e);

    //显示图像坐标
    {
        showCoords(e->pos());
    }

    if(!m_mousePressed)
        return  QWidget::mouseMoveEvent(e);
    //缩放，刷新图像
    eMouseFunction mfucntion = m_graphAttribute->mouseFunction;
    if(mfucntion == MF_ZOOM)
    {
        m_mouseRectDrawer.setPointEnd(e->pos());
        this->reDraw();
    }

    return QWidget::mouseMoveEvent(e);
}

void QCGraphBase::mousePressEvent(QMouseEvent *e)
{
    //没有数据
    if(!m_data)
        return QWidget::mousePressEvent(e);
    m_mousePressed = true;

    //鼠标功能
    eMouseFunction mfucntion = m_graphAttribute->mouseFunction;
    if(mfucntion == MF_NOTHING)
        return QWidget::mousePressEvent(e);

    //缩放
    if(mfucntion == MF_ZOOM)
    {
        if(e->button() != Qt::LeftButton)
            return QWidget::mousePressEvent(e);
        //记录起始位置，开始显示橡皮框
        m_mouseRectDrawer.setVisible(true);
        m_mouseRectDrawer.setPointBegin(e->pos());
        m_mouseRectDrawer.setPointEnd(e->pos());
        m_mouseRectDrawer.setGraphRect(m_GraphRect);
        this->reDraw();
    }

    //分析----------------------
    if(mfucntion == MF_ANALYSE)
    {
        m_traceHeadDlg->show();
        int trIdx = disTraceAt(e->pos());
        //qDebug()<<"press analy trace:"<<trIdx;
        if(trIdx>=0)
        {
            m_traceHeadDlg->showTrace(trIdx);
            this->draw();
        }
    }

    return QWidget::mousePressEvent(e);
}

void QCGraphBase::mouseReleaseEvent(QMouseEvent *e)
{
    //没有数据
    if(!m_data)
        return QWidget::mouseReleaseEvent(e);

    m_mousePressed = false;
    eMouseFunction mfucntion = m_graphAttribute->mouseFunction;
    if(mfucntion == MF_NOTHING)
        return QWidget::mouseReleaseEvent(e);
    //缩放
    if(mfucntion == MF_ZOOM)
    {
        if(e->button() != Qt::LeftButton)
            return QWidget::mouseReleaseEvent(e);

        //记录起始位置，停止显示橡皮框
        m_mouseRectDrawer.setPointEnd(e->pos());
        m_mouseRectDrawer.setVisible(false);

        //计算缩放范围，执行缩放
        //        qDebug()<<"begin pt:"<<m_mouseRectDrawer.m_beginPt
        //               <<"end pt:"<<m_mouseRectDrawer.m_endPt;

        this->zoomIn(m_mouseRectDrawer.drawRect());
    }

    return QWidget::mouseReleaseEvent(e);
}

bool QCGraphBase::doInterpolationWG()
{
    int DataRectHeight = m_GraphRect.height();
    int vertDataNum    = m_graphDataInfo.vertDataNum;
    int currentHoriIdx = m_graphDataInfo.currentHorizIdxBeg;
    int currentVertIdx = m_graphDataInfo.currentVertIdxBeg;
    float zoom_Y       = m_graphDataInfo.m_zoomY;

    //计算合适的水平步长
    int traceStep = m_graphDataInfo.calTraceStep(m_GraphRect.width(),m_graphAttribute->dispStep);

    //总的显示道数
    int traceNum = m_graphDataInfo.disTraceNum();
    int interType= m_graphAttribute->interType;

    if(m_DataInterWG){
        delete[] m_DataInterWG;
        m_DataInterWG = 0;
    }
    if(m_AnalyInterWG){
        delete[] m_AnalyInterWG;
        m_AnalyInterWG = 0;
    }

    m_DataInterWG = new float[DataRectHeight*traceNum];

    //逐道执行一维插值
    float unitY = float(DataRectHeight)/float(vertDataNum);

    //水平方向起始偏移道
    int base =  traceStep/2;
    int k = base + currentHoriIdx;
    int isOk = -1;

    for(int i=0;i <traceNum;i++)
    {
        gobs_1D_interpolation_regular(m_data + k * vertDataNum, m_graphDataInfo.vertDataNum
                                      , 0.0f, unitY,
                                      m_DataInterWG +DataRectHeight*i, DataRectHeight
                                      ,currentVertIdx*unitY,zoom_Y,
                                      0.0f, 0.0f, interType, &isOk);
        k += traceStep;
    }

    //增加分析道插值------------------------------------------------------
    int analyTrace = m_traceHeadDlg->currentTrace();
    if(analyTrace>=0)
    {
        m_AnalyInterWG = new float[DataRectHeight];
        gobs_1D_interpolation_regular(m_data + analyTrace * vertDataNum, m_graphDataInfo.vertDataNum
                                      , 0.0f, unitY,
                                      m_AnalyInterWG , DataRectHeight
                                      ,currentVertIdx*unitY,zoom_Y,
                                      0.0f, 0.0f, (int)LINEARINTER, &isOk);
    }

    return true;
}

bool QCGraphBase::doDrawPixmap()
{
    eGraphicMode Mode     = m_graphAttribute->mode;
    eScale      scaleType = m_graphAttribute->scaleType;
    eBrushMode  brushMode = m_graphAttribute->brushMode;
    float       gainPercent = m_graphAttribute->gainPercent;

    //绘制画布
    m_pixCanvas = QPixmap(m_GraphRect.size());
    m_pixCanvas.fill(Qt::white);
    QPainter    p(&m_pixCanvas);

    //---------------------------------
    int traceStep    = m_graphDataInfo.traceStep;
    int DataRecWidth = m_GraphRect.width();
    int DataRecHeight= m_GraphRect.height();
    //int zoomedHorizNum = m_graphDataInfo.getZoomedHorizNum();
    int traceNum       = m_graphDataInfo.disTraceNum();

    //一道的占的像素宽度
    float traceWid = float(DataRecWidth)/ float(traceNum);

    //道偏移和像素偏移
    float fBase = traceWid/2.0f;
    int  offset = m_graphDataInfo.currentHorizIdxBeg;
    offset     += traceStep/2;

    float valMax;
    //使用全局极值
    if(scaleType == Global)
    {
        //最大绝对值
        valMax = (fabs(m_ValMaxG)>fabs(m_ValMinG)?fabs(m_ValMaxG):fabs(m_ValMinG));
        valMax*= gainPercent;
    }
    //绘制波形图 WG
    if(Mode == WG)
    {
        QPointF *points = new QPointF[DataRecHeight];
        //每一道极值
        for(int i=0; i < traceNum; i++)
        {
            if(scaleType == Trace)
            {
                valMax = fabs(m_traceMaxValue[offset])>fabs(m_traceMinValue[offset])?
                            fabs(m_traceMaxValue[offset]):fabs(m_traceMinValue[offset]);
                valMax *= gainPercent;
                valMax  = valMax<10e-4?10e-4:valMax;
            }
            doDrawWG(i,p,fBase,DataRecHeight,traceWid,points,valMax);

            fBase  += traceWid;//每一道中心轴往后移动一个道宽
            offset += traceStep;
        }
        delete points;
        points = NULL;
    }
    //绘制变面积 VA 图
    if(Mode == VA)
    {
        for(int i=0;i<traceNum;i++){
            if(scaleType == Trace)
            {
                valMax = fabs(m_traceMaxValue[offset])>fabs(m_traceMinValue[offset])?
                            fabs(m_traceMaxValue[offset]):fabs(m_traceMinValue[offset]);
                valMax *= gainPercent;
                valMax = valMax<10e-4?10e-4:valMax;
            }
            doDrawVA(i,p,fBase,DataRecHeight,traceWid,valMax,brushMode);

            fBase  += traceWid;//每一道中心轴往后移动一个道宽
            offset += traceStep;
        }
    }
    //两者同时绘制，代码冗余，提高效率
    if(Mode == VAWG)
    {
        QPointF *points = new QPointF[DataRecHeight];
        for(int i=0;i<traceNum;i++){
            if(scaleType == Trace)
            {
                valMax = fabs(m_traceMaxValue[offset])>fabs(m_traceMinValue[offset])?
                            fabs(m_traceMaxValue[offset]):fabs(m_traceMinValue[offset]);
                valMax *= gainPercent;
                valMax = valMax<10e-4?10e-4:valMax;
            }
            doDrawWGVA(i,p,fBase,DataRecHeight,traceWid,points,valMax,brushMode);
            fBase  += traceWid;//每一道中心轴往后移动一个道宽
            offset += traceStep;
        }
        delete  points;
        points = NULL;
    }

    //绘制分析道
    doDrawAnaTrace(&p);

    p.end();
    return true;
}

void QCGraphBase::doDrawWG(int i, QPainter &p,int fBase, int height
                           , float traceWidth, QPointF *points,float valMax)
{
    float d = 0.0f;//逐点绘制图像时，首先根据下标取得两两相连的点的数据
    float x = 0.0f; //将数据换算出的振幅坐标

    //画波形图时，相邻两个数据连成直线，最终形成波形图
    for(int y=0;y<height;y++)
    {
        //数据1
        d = m_DataInterWG[i*height+y];
        x =  fBase + (traceWidth)*d*2/valMax;
        points[y] =QPointF(x,y);
    }
    p.drawPolyline(points,height);
}

void QCGraphBase::doDrawVA(int i, QPainter &p,int fBase, int height,
                           float traceWidth, float valMax,eBrushMode brushMode)
{
    float d = 0.0f;//逐点绘制图像时，首先根据下标取得两两相连的点的数据
    float x = 0.0f; //将数据换算出的振幅坐标
    int   y = 0; // 绘制连线时的y轴坐标

    for(;y<height;y++)
    {
        float tmpBase=fBase;
        //数据
        d = m_DataInterWG[i*height+y];

        //获取每个数据转换出的图像振幅坐标
        x = fBase + (traceWidth)*d*2/valMax;

        if(brushMode == BrushPositive)
        {
            if(x > fBase)
            {
                p.drawLine(x,y,tmpBase,y);
            }
        }
        else if(brushMode==BrushNegative)
        {
            if(x < fBase)
            {
                p.drawLine(x,y,tmpBase,y);
            }

        }
        else if(brushMode==BrushBoth)
        {
            p.drawLine(x,y,tmpBase,y);
        }
    }

}

void QCGraphBase::doDrawWGVA(int i, QPainter &p,int fBase, int height,
                             float traceWidth,QPointF *points,float valMax,eBrushMode brushMode)
{
    float d = 0.0f;//逐点绘制图像时，首先根据下标取得两两相连的点的数据
    float x = 0.0f; //将数据换算出的振幅坐标
    int   y = 0; // 绘制连线时的y轴坐标

    for(;y<height;y++)
    {
        float tmpBase=fBase;
        //数据
        d = m_DataInterWG[i*height+y];

        //获取每个数据转换出的图像振幅坐标
        x = fBase + (traceWidth)*d*2/valMax;
        points[y] = QPointF(x,y);
        if(brushMode==BrushPositive)
        {
            if(x > fBase)
            {
                p.drawLine(x,y,tmpBase,y);
            }

        }
        else if(brushMode==BrushNegative)
        {
            if(x < fBase)
            {
                p.drawLine(x,y,tmpBase,y);
            }

        }
        else if(brushMode==BrushBoth)
        {
            p.drawLine(x,y,tmpBase,y);
        }
    }
    p.drawPolyline(points,height);

}

void QCGraphBase::drawScale(QPainter *painter)
{
    //水平刻度
    painter->save();
    //图像区左上角
    painter->translate(m_GraphRect.topLeft());

    //绘制横线刻度
    drawHScale(painter);

    //纵向刻度-----
    drawVScale(painter);

    //绘制水平Labels
    drawHLabels(painter);

    drawVLabels(painter);

    painter->restore();

    //绘制分量标识
    drawBottomLabels(painter);
}

void QCGraphBase::drawHScale(QPainter *painter)
{
    //计算要显示的道头关键字的位置
    int        pos = m_graphAttribute->showedHeadFieldPos.pos;
    DataType  dtype= m_graphAttribute->showedHeadFieldPos.dType;
    QFont font;
    QFontMetrics fontMetrcs(font);

    //总的显示道数
    int disTraceNum    = m_graphDataInfo.disTraceNum();
    int currentHoriIdx = m_graphDataInfo.currentHorizIdxBeg;

    //当前显示道间隔
    int traceStep      = m_graphDataInfo.traceStep;
    int minLen         = m_graphAttribute->horiAxesMinLen;
    //横向刻度,根据label 最小间距计算显示道间距
    DScaleUnit unit = GraphFunctions::makeDeparatedUnit(m_GraphRect.width()
                                                           ,disTraceNum,minLen);
    //qDebug()<<"data count:"<<unit.dataCount
    //           <<"data offset:"<<unit.dataOffset
    //          <<"trace step:"<<traceStep
    //         <<"currentHoriIdx:"<<currentHoriIdx;
    int   idx  = unit.dataOffset;
    float xpos = unit.pixOffset;

    int   trace = currentHoriIdx + (unit.dataOffset)*traceStep+traceStep/2;

    char *header;
    QString text;
    while(idx<disTraceNum)
    {
        painter->drawLine(xpos,-2,xpos,-2-Main_Tick_Length);

        header = (char*)(&m_headers[trace]);

        int x  =  HeaderFieldPos::headerValue(header,pos,dtype);
        text   =  QString::number(x);

        painter->drawText(xpos-fontMetrcs.width(text)/2
                          ,-3-Main_Tick_Length,
                          text);
        //绘制文本
        trace += unit.dataCount*traceStep;
        idx += unit.dataCount;
        xpos+= unit.pixCount;
    }
}

void QCGraphBase::drawVScale(QPainter *painter)
{
    int zoomedVertNum = m_graphDataInfo.getZoomedVertNum();
    int currentVertIdx = m_graphDataInfo.currentVertIdxBeg;

    //采样间隔转换成 ms(或者m)
    float dns = m_gatherInfo->sampleInterval/1000.f;

    //显示范围
    float minVal = currentVertIdx *dns;
    float maxVal = (currentVertIdx + zoomedVertNum-1)*dns;
    int minLen   = m_graphAttribute->vertAxesMinLen;
    float v_scale = float(m_GraphRect.height())/(maxVal - minVal);

    CScaleUnit unit = GraphFunctions::makeContinuousUnit(minVal,maxVal,m_GraphRect.height()
                                                            ,minLen);
    //qDebug()<<unit.corMin<<unit.corNum<<unit.corStep;
    float ypos,ypos1;
    int   width = m_GraphRect.width();
    //次刻度之间距离
    float pixDs = v_scale*unit.corStep/float(Sub_Tick_Num);
    QFont font;
    QFontMetrics fontMetrs(font);

    int girdPos = m_graphAttribute->gridPosition;
    QString text;
    for(int i =0;i<unit.corNum;i++)
    {
        ypos = v_scale *(unit.corMin + unit.corStep*i -minVal);
        //绘制之前次刻度------------------
        for(int j =1;j<Sub_Tick_Num;j++)
        {
            ypos1 = ypos - j*pixDs;
            if(ypos1<0)
                break;
            painter->drawLine(-2,ypos1,-2-Sub_Tick_Length,ypos1);
            painter->drawLine(width+1,ypos1,width+1+Sub_Tick_Length,ypos1);
        }
        painter->drawLine(-2,ypos,-2-Main_Tick_Length,ypos);
        painter->drawLine(width+1,ypos,width+1+Main_Tick_Length,ypos);

        //网格线
        if(girdPos & HorizontalGrid)
            painter->drawLine(0,ypos,m_GraphRect.width(),ypos);

        //刻度值
        text = QString::number(unit.corMin + unit.corStep*i);
        painter->drawText(-2-fontMetrs.width(text)-Sub_Tick_Length
                          ,ypos-fontMetrs.height()/2
                          ,fontMetrs.width(text)
                          ,fontMetrs.height(),Qt::AlignRight|Qt::AlignHCenter,text);
        painter->drawText(width+1+Sub_Tick_Length
                          ,ypos-fontMetrs.height()/2
                          ,fontMetrs.width(text)
                          ,fontMetrs.height()
                          ,Qt::AlignLeft|Qt::AlignHCenter,text);
    }
    //绘制一段次刻度
    for(int j =1;j<Sub_Tick_Num;j++)
    {
        ypos1 = ypos + j*pixDs;
        if(ypos>=m_GraphRect.height())
            break;
        painter->drawLine(-2,ypos1,-2-Sub_Tick_Length,ypos1);
        painter->drawLine(width+1,ypos1,width+1+Sub_Tick_Length,ypos1);
    }
}

void QCGraphBase::drawHLabels(QPainter *painter)
{
    //水平刻度名称
    QFont font;
    QFontMetrics fontMetrcs(font);
    //绘制刻度名称
    painter->drawText(0,-5-Main_Tick_Length-fontMetrcs.height()
                      ,m_graphAttribute->showedHeadFieldPos.fieldName);

    //title
    painter->drawText(0,-Top_Scale_Margin
                      ,m_GraphRect.width()
                      ,Top_Scale_Margin-2-Main_Tick_Length-fontMetrcs.height()
                      ,Qt::AlignHCenter|Qt::AlignBottom
                      ,QString("screen %1/%2")
                      .arg(m_gatherInfo->gatherNo)
                      .arg(m_gatherInfo->gatherNum));
}

void QCGraphBase::drawVLabels(QPainter *painter)
{
    painter->rotate(90);
    //绘制Label
    VertScaleType scaleType = m_graphAttribute->vertScaleType;

    QString scaleLabel;
    if(scaleType == ScaleTime)
        scaleLabel = "Time(ms)";
    else if(scaleType == ScaleDepth)
        scaleLabel = "Depth(m)";

    painter->drawText(0,0,m_GraphRect.height(),Left_Scale_Margin
                      ,Qt::AlignHCenter|Qt::AlignBottom,scaleLabel);
    painter->drawText(0,-m_GraphRect.width()-Right_Scale_Margin
                      ,m_GraphRect.height(),Right_Scale_Margin
                      ,Qt::AlignHCenter|Qt::AlignTop,scaleLabel);
}


void QCGraphBase::drawBottomLabels(QPainter *painter)
{
    painter->save();
    painter->translate(m_GraphRect.bottomLeft());

    int graphWidth = m_GraphRect.width();
    int graphHeight= m_GraphRect.height();
    painter->setClipRect(0,-graphHeight,graphWidth,graphHeight+Bottom_Scale_Margin);
    float intX = float(graphWidth)/float(m_graphDataInfo.getZoomedHorizNum());

    int horiNum = m_graphDataInfo.horiDataNum;

    //分量数
    int coms = m_graphAttribute->components;
    int t_per= horiNum/coms;
    int xpos;
    //label标签
    QStringList labels;
    if(coms == 1)
    {
        labels<<m_gatherInfo->gatherName;
    }else{
        labels<<"bhx"
             <<"bhy"
            <<"bhz"
           <<"hyd";
    }

    painter->setPen(Qt::red);
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    painter->setFont(font);
    for(int i=0;i<=coms;i++){
        xpos = (i*t_per-m_graphDataInfo.currentHorizIdxBeg)*intX;
        if(coms>1)
            painter->drawLine(xpos,-graphHeight,xpos,graphHeight+Bottom_Scale_Margin);
        else
            painter->drawLine(xpos,0,xpos,Bottom_Scale_Margin);
        if(i<coms)
            painter->drawText(xpos,0,t_per*intX,Bottom_Scale_Margin
                              ,Qt::AlignCenter,labels[i]);
    }
    painter->restore();
}

int QCGraphBase::traceAt(const QPoint &pt)
{
    if(!m_GraphRect.contains(pt))
        return -1;

    //x 坐标-------------------------
    int x = pt.x()-Left_Scale_Margin;
    return  m_graphDataInfo.currentHorizIdxBeg
            + float(x)/float(m_GraphRect.width())*m_graphDataInfo.getZoomedHorizNum();
}

int QCGraphBase::disTraceAt(const QPoint &pt)
{
    if(!m_GraphRect.contains(pt))
        return -1;
    //从显示的道中拾取
    int x = pt.x()-Left_Scale_Margin;
    int traceNum     = m_graphDataInfo.disTraceNum();
    int traceStep    = m_graphDataInfo.traceStep;
    //一道的占的像素宽度
    float traceWid = float(m_GraphRect.width())/ float(traceNum);

    //道偏移和像素偏移
    //float fBase = traceWid/2.0f;
    int  offset = m_graphDataInfo.currentHorizIdxBeg;
    offset     += traceStep/2;
    return offset+int(float(x)/traceWid)*traceStep;
}

int QCGraphBase::nsAt(const QPoint &pt)
{
    if(!m_GraphRect.contains(pt))
        return -1;
    //y坐标-----------------------
    int y = pt.y() - Top_Scale_Margin;
    return m_graphDataInfo.currentVertIdxBeg
            + float(y)/float(m_GraphRect.height())*m_graphDataInfo.getZoomedVertNum();
}

void QCGraphBase::showCoords(const QPoint &pt)
{
    //找到该道
    int trace = traceAt(pt);
    int ns    = nsAt(pt);
    if(trace<0 || ns<0)
        return;

    HeaderFieldPos headerPos = m_graphAttribute->showedHeadFieldPos;
    QString xLabel =  headerPos.fieldName;
    int pos        =  headerPos.pos;
    DataType  dType=  headerPos.dType;
    //横坐标当前显示关键字
    char  *header = (char*)(&m_headers[trace]);
    float x =  HeaderFieldPos::headerValue(header,pos,dType);

    float y = ns*m_gatherInfo->sampleInterval/1000.f;//(ms/m)
    float z = m_data[trace*m_gatherInfo->Ns+ns];
    //显示
    m_statusBar->showMessage(QString("Amplitude=%1 Time=%2 ms %3=%4")
                             .arg(z)
                             .arg(y)
                             .arg(xLabel)
                             .arg(x));
}

void QCGraphBase::doDrawAnaTrace(QPainter *p)
{
    if(m_traceHeadDlg->currentTrace()<0 || !m_AnalyInterWG)
        return;
    p->setPen(Qt::red);
    int trace = m_traceHeadDlg->currentTrace();
    int height     = m_GraphRect.height();
    QPointF *points = new QPointF[height];
    float  gainPercent = m_graphAttribute->gainPercent;
    //
    float valMax;
    valMax = (fabs(m_ValMaxG)>fabs(m_ValMinG)?fabs(m_ValMaxG):fabs(m_ValMinG));
    if(m_graphAttribute->scaleType == Trace)
        valMax = fabs(m_traceMaxValue[trace])>fabs(m_traceMinValue[trace])?
                    fabs(m_traceMaxValue[trace]):fabs(m_traceMinValue[trace]);
    valMax*= gainPercent;

    //m_AnalyInterWG-------------------
    float d = 0.0f;//逐点绘制图像时，首先根据下标取得两两相连的点的数据
    float x = 0.0f; //将数据换算出的振幅坐标

    int traceStep    = m_graphDataInfo.traceStep;
    int DataRecWidth = m_GraphRect.width();

    int  offset = m_graphDataInfo.currentHorizIdxBeg + traceStep/2;
    //显示的道数
    int traceNum       = m_graphDataInfo.disTraceNum();
    //一道的占的像素宽度
    float traceWid = float(DataRecWidth)/ float(traceNum);

    //计算该道的基准位置------------------------
    float fBase =  traceWid/2.0f+traceWid*((trace - offset)/traceStep);

    //画波形图时，相邻两个数据连成直线，最终形成波形图
    for(int y=0;y<height;y++)
    {
        //数据1
        d = m_AnalyInterWG[y];
        x =  fBase + (traceWid)*d*2/valMax;
        points[y] =QPointF(x,y);
    }

    p->drawPolyline(points,height);

    delete[] points;
    points = NULL;
    m_traceHeadDlg->setCurrentTrace(-1);
}

