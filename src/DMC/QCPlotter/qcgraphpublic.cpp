#include "qcgraphpublic.h"

#include "ui_traceheaddlg.h"
#include "qcgraphbase.h"

#include <QPainter>
#include <QDesktopWidget>

void GraphDataInfo::zoomX(const int &rectWidth, const int &cutX_1, const int &cutX_2)
{
    int tempX_1 = cutX_1 >= 0 ? cutX_1 : 0;
    tempX_1 = tempX_1 <= rectWidth ? tempX_1 : rectWidth;
    int tempX_2 = cutX_2 >= 0 ? cutX_2 : 0;
    tempX_2 = tempX_2 <= rectWidth ? tempX_2 : rectWidth;

    if(abs(tempX_1-tempX_2)>=ZOOM_MIN_PIXEL) //限制拖拉框某方向的像素
    {
        float oldZoom = m_zoomX;

        m_zoomX = ImageBaseFunction::getZoomValue(rectWidth,abs(tempX_1-tempX_2),m_zoomX);
        qDebug()<<"m_zoom_x:"<<m_zoomX;
        if (m_zoomX < getLimitZoomX())
        {
            m_zoomX = getLimitZoomX();
        }

        currentHorizIdxBeg += ImageBaseFunction::getCurrentIdxOffset(oldZoom,horiDataNum,rectWidth,tempX_1<tempX_2?tempX_1 : tempX_2);

        qDebug()<<"current hori Idx:"<<currentHorizIdxBeg;
    }
}

void GraphDataInfo::zoomY(const int &rectHeight, const int &cutY_1, const int &cutY_2)
{
    int tempY_1 = cutY_1 >= 0 ? cutY_1 : 0;
    tempY_1 = tempY_1 <= rectHeight ? tempY_1 : rectHeight;
    int tempY_2 = cutY_2 >= 0 ? cutY_2 : 0;
    tempY_2 = tempY_2 <= rectHeight ? tempY_2 : rectHeight;
    if(abs(tempY_1-tempY_2)>=ZOOM_MIN_PIXEL) //限制拖拉框某方向的像素
    {
        float oldZoom = m_zoomY;
        m_zoomY = ImageBaseFunction::getZoomValue(rectHeight,abs(tempY_1-tempY_2),m_zoomY);
        if (m_zoomY < getLimitZoomY())
            m_zoomY = getLimitZoomY();
        currentVertIdxBeg += ImageBaseFunction::getCurrentIdxOffset(oldZoom,vertDataNum,rectHeight,tempY_1<tempY_2?tempY_1 : tempY_2);

    }
}

void GraphDataInfo::zoomOut()
{
    currentHorizIdxBeg = 0;
    currentVertIdxBeg = 0;
    m_zoomX = 1.0f;
    m_zoomY = 1.0f;
}

void GraphDataInfo::zoomIn(QRect graphRect,QRect rect)
{
    zoomX(graphRect.width(),rect.left(),rect.right());
    zoomY(graphRect.height(),rect.top(),rect.bottom());
}

//Mouse_Rect_Drawer------------------------------
Mouse_Rect_Drawer::Mouse_Rect_Drawer(QColor color)
{
    m_visible = false;
    m_color   = color;
}

void Mouse_Rect_Drawer::setPointBegin(QPoint pt)
{
    m_beginPt = pt;
}

void Mouse_Rect_Drawer::setPointEnd(QPoint pt)
{
    m_endPt = pt;
}
void Mouse_Rect_Drawer::setGraphRect(const QRect &rect)
{
    m_grapRect = rect;
}

void Mouse_Rect_Drawer::show(QPainter &p)
{
    if (m_visible){
        if(!m_grapRect.intersects(QRect(m_beginPt,m_endPt)))
            return;

        //根据拖拉框和图像区范围，计算合适的缩放区域
        QRect rect = m_grapRect.intersected(QRect(m_beginPt,m_endPt));

        p.setPen(QPen(m_color,1));
        //缩放框---
        p.drawRect(rect);

        //框边缘刻度-----
        int left  = rect.left();
        int right = rect.right();
        int top   = rect.top();
        int bottom= rect.bottom();

        //左右-----
        p.drawLine(left,m_grapRect.top()-Drawer_Border_Margin
                   ,left,m_grapRect.top()-Drawer_Border_Length-Drawer_Border_Margin);
        p.drawLine(right,m_grapRect.top()-Drawer_Border_Margin
                   ,right,m_grapRect.top()-Drawer_Border_Length-Drawer_Border_Margin);

        //上下,左右两边都绘制-----
        p.drawLine(m_grapRect.left()-Drawer_Border_Margin,top
                   ,m_grapRect.left()-Drawer_Border_Length-Drawer_Border_Margin,top);
        p.drawLine(m_grapRect.left()-Drawer_Border_Margin,bottom
                   ,m_grapRect.left()-Drawer_Border_Length-Drawer_Border_Margin,bottom);
        p.drawLine(m_grapRect.right()+Drawer_Border_Margin,top
                   ,m_grapRect.right()+Drawer_Border_Length+Drawer_Border_Margin,top);
        p.drawLine(m_grapRect.right()+Drawer_Border_Margin,bottom
                   ,m_grapRect.right()+Drawer_Border_Length+Drawer_Border_Margin,bottom);
    }
}

void Mouse_Rect_Drawer::setVisible(bool visible)
{
    m_visible = visible;
}

void Mouse_Rect_Drawer::setColor(QColor color)
{
    m_color = color;
}

TraceHeadDlg::TraceHeadDlg(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::TraceHeadDlg)
{
    ui->setupUi(this);

    this->setWindowTitle("TraceInfo Dialog");

    initDlg();

    m_currentTrace = -1;

    QRect rect = QApplication::desktop()->screenGeometry();
    this->resize(420,rect.height()*3/4);
}

void TraceHeadDlg::initDlg()
{
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setAlternatingRowColors(true);

    ui->tableWidget->setColumnWidth(0,80);
    ui->tableWidget->setColumnWidth(1,120);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //获取每一行变量名称和描述Descrip
    QStringList headList;
    getTraceHeadInfo(headList);
    //初始化每一列
    ui->tableWidget->setRowCount(headList.size()/2);
    for(int i=0;i<headList.size()/2;i++)
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(headList[i*2]));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(headList[i*2+1]));
    }
}

void TraceHeadDlg::setHeaders(TraceHead *headers)
{
    m_headers = headers;
}

void TraceHeadDlg::closeEvent(QCloseEvent *e)
{
    m_currentTrace = -1;
    ((QCGraphBase*)this->parent())->objDraw();
    QDialog::closeEvent(e);
}

void TraceHeadDlg::showTrace(const int &idx)
{
    m_currentTrace = idx;
    if(m_currentTrace<0)
        return;
    //表格赋值
    ui->tableWidget->setItem(0,1,new QTableWidgetItem(QString::number(m_headers[idx].tracl)));
    ui->tableWidget->setItem(1,1,new QTableWidgetItem(QString::number(m_headers[idx].tracr)));
    ui->tableWidget->setItem(2,1,new QTableWidgetItem(QString::number(m_headers[idx].fldr)));
    ui->tableWidget->setItem(3,1,new QTableWidgetItem(QString::number(m_headers[idx].tracf)));
    ui->tableWidget->setItem(4,1,new QTableWidgetItem(QString::number(m_headers[idx].ep)));
    ui->tableWidget->setItem(5,1,new QTableWidgetItem(QString::number(m_headers[idx].cdp)));
    ui->tableWidget->setItem(6,1,new QTableWidgetItem(QString::number(m_headers[idx].cdpt)));
    ui->tableWidget->setItem(7,1,new QTableWidgetItem(QString::number(m_headers[idx].trid)));
    ui->tableWidget->setItem(8,1,new QTableWidgetItem(QString::number(m_headers[idx].nvs)));
    ui->tableWidget->setItem(9,1,new QTableWidgetItem(QString::number(m_headers[idx].nhs)));
    //10-------------------------------------------------
    //表格赋值
    ui->tableWidget->setItem(10,1,new QTableWidgetItem(QString::number(m_headers[idx].duse)));
    ui->tableWidget->setItem(11,1,new QTableWidgetItem(QString::number(m_headers[idx].offset)));
    ui->tableWidget->setItem(12,1,new QTableWidgetItem(QString::number(m_headers[idx].gelev)));
    ui->tableWidget->setItem(13,1,new QTableWidgetItem(QString::number(m_headers[idx].selev)));
    ui->tableWidget->setItem(14,1,new QTableWidgetItem(QString::number(m_headers[idx].sdepth)));
    ui->tableWidget->setItem(15,1,new QTableWidgetItem(QString::number(m_headers[idx].gdel)));
    ui->tableWidget->setItem(16,1,new QTableWidgetItem(QString::number(m_headers[idx].sdel)));
    ui->tableWidget->setItem(17,1,new QTableWidgetItem(QString::number(m_headers[idx].swdep)));
    ui->tableWidget->setItem(18,1,new QTableWidgetItem(QString::number(m_headers[idx].gwdep)));
    ui->tableWidget->setItem(19,1,new QTableWidgetItem(QString::number(m_headers[idx].scalel)));
    //20-------------------------------------------------
    //表格赋值
    ui->tableWidget->setItem(20,1,new QTableWidgetItem(QString::number(m_headers[idx].scalco)));
    ui->tableWidget->setItem(21,1,new QTableWidgetItem(QString::number(m_headers[idx].sx)));
    ui->tableWidget->setItem(22,1,new QTableWidgetItem(QString::number(m_headers[idx].sy)));
    ui->tableWidget->setItem(23,1,new QTableWidgetItem(QString::number(m_headers[idx].gx)));
    ui->tableWidget->setItem(24,1,new QTableWidgetItem(QString::number(m_headers[idx].gy)));
    ui->tableWidget->setItem(25,1,new QTableWidgetItem(QString::number(m_headers[idx].counit)));
    ui->tableWidget->setItem(26,1,new QTableWidgetItem(QString::number(m_headers[idx].wevel)));
    ui->tableWidget->setItem(27,1,new QTableWidgetItem(QString::number(m_headers[idx].swevel)));
    ui->tableWidget->setItem(28,1,new QTableWidgetItem(QString::number(m_headers[idx].sut)));
    ui->tableWidget->setItem(29,1,new QTableWidgetItem(QString::number(m_headers[idx].gut)));
    //30-------------------------------------------------
    //表格赋值
    ui->tableWidget->setItem(30,1,new QTableWidgetItem(QString::number(m_headers[idx].sstat)));
    ui->tableWidget->setItem(31,1,new QTableWidgetItem(QString::number(m_headers[idx].gstat)));
    ui->tableWidget->setItem(32,1,new QTableWidgetItem(QString::number(m_headers[idx].tstat)));
    ui->tableWidget->setItem(33,1,new QTableWidgetItem(QString::number(m_headers[idx].laga)));
    ui->tableWidget->setItem(34,1,new QTableWidgetItem(QString::number(m_headers[idx].lagb)));
    ui->tableWidget->setItem(35,1,new QTableWidgetItem(QString::number(m_headers[idx].delrt)));
    ui->tableWidget->setItem(36,1,new QTableWidgetItem(QString::number(m_headers[idx].muts)));
    ui->tableWidget->setItem(37,1,new QTableWidgetItem(QString::number(m_headers[idx].mute)));
    ui->tableWidget->setItem(38,1,new QTableWidgetItem(QString::number(m_headers[idx].ns)));
    ui->tableWidget->setItem(39,1,new QTableWidgetItem(QString::number(m_headers[idx].dt)));
    //40-------------------------------------------------
    //表格赋值
    ui->tableWidget->setItem(40,1,new QTableWidgetItem(QString::number(m_headers[idx].gain)));
    ui->tableWidget->setItem(41,1,new QTableWidgetItem(QString::number(m_headers[idx].igc)));
    ui->tableWidget->setItem(42,1,new QTableWidgetItem(QString::number(m_headers[idx].igi)));
    ui->tableWidget->setItem(43,1,new QTableWidgetItem(QString::number(m_headers[idx].corr)));
    ui->tableWidget->setItem(44,1,new QTableWidgetItem(QString::number(m_headers[idx].sfs)));
    ui->tableWidget->setItem(45,1,new QTableWidgetItem(QString::number(m_headers[idx].sfe)));
    ui->tableWidget->setItem(46,1,new QTableWidgetItem(QString::number(m_headers[idx].slen)));
    ui->tableWidget->setItem(47,1,new QTableWidgetItem(QString::number(m_headers[idx].styp)));
    ui->tableWidget->setItem(48,1,new QTableWidgetItem(QString::number(m_headers[idx].stas)));
    ui->tableWidget->setItem(49,1,new QTableWidgetItem(QString::number(m_headers[idx].stae)));
    //50-------------------------------------------------
    //表格赋值
    ui->tableWidget->setItem(50,1,new QTableWidgetItem(QString::number(m_headers[idx].tatyp)));
    ui->tableWidget->setItem(51,1,new QTableWidgetItem(QString::number(m_headers[idx].afilf)));
    ui->tableWidget->setItem(52,1,new QTableWidgetItem(QString::number(m_headers[idx].afils)));
    ui->tableWidget->setItem(53,1,new QTableWidgetItem(QString::number(m_headers[idx].nofilf)));
    ui->tableWidget->setItem(54,1,new QTableWidgetItem(QString::number(m_headers[idx].nofils)));
    ui->tableWidget->setItem(55,1,new QTableWidgetItem(QString::number(m_headers[idx].lcf)));
    ui->tableWidget->setItem(56,1,new QTableWidgetItem(QString::number(m_headers[idx].hcf)));
    ui->tableWidget->setItem(57,1,new QTableWidgetItem(QString::number(m_headers[idx].lcs)));
    ui->tableWidget->setItem(58,1,new QTableWidgetItem(QString::number(m_headers[idx].hcs)));
    ui->tableWidget->setItem(59,1,new QTableWidgetItem(QString::number(m_headers[idx].year)));
    //60-------------------------------------------------
    //表格赋值
    ui->tableWidget->setItem(60,1,new QTableWidgetItem(QString::number(m_headers[idx].day)));
    ui->tableWidget->setItem(61,1,new QTableWidgetItem(QString::number(m_headers[idx].hour)));
    ui->tableWidget->setItem(62,1,new QTableWidgetItem(QString::number(m_headers[idx].minute)));
    ui->tableWidget->setItem(63,1,new QTableWidgetItem(QString::number(m_headers[idx].sec)));
    ui->tableWidget->setItem(64,1,new QTableWidgetItem(QString::number(m_headers[idx].timbas)));
    ui->tableWidget->setItem(65,1,new QTableWidgetItem(QString::number(m_headers[idx].trwf)));
    ui->tableWidget->setItem(66,1,new QTableWidgetItem(QString::number(m_headers[idx].grnors)));
    ui->tableWidget->setItem(67,1,new QTableWidgetItem(QString::number(m_headers[idx].grnofr)));
    ui->tableWidget->setItem(68,1,new QTableWidgetItem(QString::number(m_headers[idx].grnlof)));
    ui->tableWidget->setItem(69,1,new QTableWidgetItem(QString::number(m_headers[idx].gaps)));
    ui->tableWidget->setItem(70,1,new QTableWidgetItem(QString::number(m_headers[idx].otrav)));
    //71-------------------------------------------------
}

void TraceHeadDlg::getTraceHeadInfo(QStringList &headInfos)
{
    headInfos<<"tracl"<<"Trace sequence number within line"
            <<"tracr"<<"Trace sequence number within SEG Y file(reel)"
           <<"fldr"<<"Original field record number"
          <<"tracf"<<"Trace number within original field record"
         <<"ep"<<"energy source point number"
        <<"cdp"<<"CDP Ensemble number"
       <<"cdpt"<<"trace number within the ensemble"
      <<"trid"<<"trace identification code"
     <<"nvs"<<"Number of vertically summed traces yielding this trace"
    <<"nhs"<<"Number of horizontally summed traces yielding this trace"
      //10-----------------------------------
    <<"duse"<<"Data use (1 = Production, 2 = Test)"
    <<"offset"<<"Distance from the source point to the receiver group"
    <<"gelev"<<"Receiver group elevation from sea level"
    <<"selev"<<"Surface elevation at source"
    <<"sdepth"<<"Source depth below surface"
    <<"gdel"<<"Datum elevation at receiver group"
    <<"sdel"<<"Datum elevation at source"
    <<"swdep"<<"Water depth at source"
    <<"gwdep"<<"Water depth at receiver group"
    <<"scalel"<<"Scalar factor for the previous 7 entries"
      //20--------------------------------------
    <<"scalco"<<"Scalar factor for the next 4 entries"
    <<"sx"<<"Source coordinate - X"
    <<"sy"<<"Source coordinate - Y"
    <<"gx"<<"Receiver Group coordinate - X"
    <<"gy"<<"Receiver Group coordinate - Y"
    <<"counit"<<"Coordinate units code"
    <<"wevel"<<"Weathering velocity"
    <<"swevel"<<"Subweathering velocity"
    <<"sut"<<"Uphole time at source in milliseconds"
    <<"gut"<<"Uphole time at receiver group in milliseconds"
      //30--------------------------------------------
    <<"sstat"<<"Source static correction in milliseconds"
    <<"gstat"<<"Group static correction  in milliseconds"
    <<"tstat"<<"Total static applied  in milliseconds"
    <<"laga"<<"Lag time in ms between end of header and time break"
    <<"lagb"<<"lag time in ms between the time break and shot"
    <<"delrt"<<"delay recording time, time in ms between shot and recording start"
    <<"muts"<<"mute time--start"
    <<"mute"<<"mute time--end"
    <<"ns"<<"number of samples in this trace"
    <<"dt"<<"sample interval; in micro-seconds"
      //40-------------------------------------------------
    <<"gain"<<"gain type of field instruments code"
    <<"igc"<<"instrument gain constant"
    <<"igi"<<"instrument early or initial gain"
    <<"corr"<<"correlated(1 = no 2 = yes)"
    <<"sfs"<<"sweep frequency at start"
    <<"sfe"<<"sweep frequency at end"
    <<"slen"<<"sweep length in ms"
    <<"styp"<<"sweep type code( 1 = linear 2 = cos-squared 3 = other)"
    <<"stas"<<"sweep trace length at start in ms"
    <<"stae"<<"sweep trace length at end in ms"
      //50-------------------------------------------------
    <<"tatyp"<<"taper type (1=linear, 2=cos^2, 3=other)"
    <<"afilf"<<"alias filter frequency if used"
    <<"afils"<<"alias filter slope"
    <<"nofilf"<<"notch filter frequency if used"
    <<"nofils"<<"notch filter slope"
    <<"lcf"<<"low cut frequency if used"
    <<"hcf"<<"high cut frequncy if used"
    <<"lcs"<<"low cut slope"
    <<"hcs"<<"high cut slope"
    <<"year"<<"year data recorded"
      //60-------------------------------------------------
    <<"day"<<"day of year"
    <<"hour"<<"hour of day (24 hour clock)"
    <<"minute"<<"minute of hour"
    <<"sec"<<"second of minute"
    <<"timbas"<<"time basis code"
    <<"trwf"<<"trace weighting factor"
    <<"grnors"<<"geophone group number of roll switch position one"
    <<"grnofr"<<"eophone group number of trace one within original field record"
    <<"grnlof"<<"geophone group number of last trace within original field record"
    <<"gaps"<<"gap size"
    <<"otrav"<<"overtravel taper code"
      ;//71-------------------------------------------------
}



void TraceHeadDlg::on_pushButton_clicked()
{
    this->close();
}
