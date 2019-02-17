#include "linemanager.h"
#include "ui_linemanager.h"
#include "deploysystem.h"
#include "assignnodedlg.h"
#include "rndeployments.h"
#include "gsqlfunctions.h"

#include <QDesktopWidget>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QSpinBox>

#include "gassistant.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


LineManager::LineManager(QWidget *parent) :
    QMainWindow(parent),CommModule(),
    ui(new Ui::LineManager)
{
    ui->setupUi(this);
    this->setWindowTitle("LMGR_");
    m_menuManger = new MenuManager(this);
    m_toolManager  = new ToolManager(this);
    m_AreaDataInfo = 0;

    m_CreateAreaDialog = 0;
    m_UpdateAreaDialog = 0;

    m_AreaDataInfo     = 0;
    m_AreaListDlg      = 0;
    m_statusBar        = 0;

    m_AssignNodeDlg    = 0;

    m_depolySystem = new DeploySystem(this);
    m_RNDepolyments = new RNDeployments(this);

    initWindow();

    m_AreaListDlg = new AreaListDlg(this);

    connect(m_AreaListDlg,SIGNAL(signalOpenArea(QString)),this,SLOT(slotOpenArea(QString)));
    connect(m_AreaListDlg,SIGNAL(signalDeleteArea(QString)),this,SLOT(slotDeleteArea(QString)));


    m_AreaView->setRNDepolyments(m_RNDepolyments);

    connect(m_depolySystem,SIGNAL(signalDepolySysConnected(bool)),this,SLOT(slotDepolySysConnected(bool)));
    connect(m_depolySystem,SIGNAL(signalDepolySysStateChanged(bool)),this,SLOT(setDepolySysCommStatus(bool)));

    connect(m_depolySystem,SIGNAL(signalUpdateDevice(QString)),this,SLOT(updateRecvedDevice(QString)));

    connect(m_RNDepolyments,SIGNAL(signalUpdateRNDepolyments()),this,SLOT(slotUpdateRNDepolyments()));

    connect(m_RNDepolyments,SIGNAL(signalUpdateRecvCoord(float,int,Coordinate)),this,SLOT(slotupdateRecvCoord(float,int,Coordinate)));

    QRect rect = QApplication::desktop()->screenGeometry();
    this->resize(rect.width()*2/3,rect.height()*3/4);
    this->move((rect.width()-width())/2,(rect.height()-height())/2);
}

LineManager::~LineManager()
{
    delete ui;
    if(m_AreaDataInfo)
        delete m_AreaDataInfo;
    if(m_statusBar)
        delete m_statusBar;
}

bool LineManager::init()
{
    //打开数据库
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    bool ok = sqlFunc->openDataBase(m_projInfo.ProjectPath,m_projInfo.ProjectName);
    if(!ok)
    {
        QMessageBox::critical(this,"error","Open DataBase Error");
    }

    m_depolySystem->setProjInfo(m_projInfo);

    m_RNDepolyments->setProjInfo(m_projInfo);

    //工区列表
    m_AreaListDlg->setProjInfo(ProjectInfo(m_projPath,m_projName));
    return ok;
}

void LineManager::slotCreateArea(const QString &areaName)
{
    if(!m_AreaDataInfo)
        m_AreaDataInfo = new AreaDataInfo;

    //1 读取数据----------------
    m_CreateAreaDialog->setProgressInfo("Reading sps files...",1);
    AreaDataInfo  *areaDataInfo = new AreaDataInfo;
    QStringList spsFiles = m_CreateAreaDialog->spsFiles();
    int ok = 0; QString errStr;
    ok = gobs_sps_files_SR_read(spsFiles,areaDataInfo,&errStr);
    if(ok !=0 )
    {
        QMessageBox::warning(this,"error","Read SPS File Error:"+errStr);
        return;
    }

    m_CreateAreaDialog->setProgressInfo("Creating Area...",20);
    //2 创建工区文件,保存工区-------------
    ok = Area::createArea(m_projInfo,areaName,spsFiles);
    if(ok !=0 )
    {
        qDebug()<<"create area ok:"<<ok;
        QMessageBox::warning(this,"error","Create Area Failed.");
        return;
    }

    m_CreateAreaDialog->setProgressInfo("Updating...",40);

    //3 创建设备表
    m_depolySystem->addNewArea(areaName);
    bool ret = SqlFunctions::instance()->createTable(areaName);
    if(!ret)
        throw "Create DataBase Table error!";

    //投放编辑添加工区
    if(m_AssignNodeDlg)
        m_AssignNodeDlg->addNewArea(areaName);

    //保存数据--------------------
    setCurrentAreaData(areaName,areaDataInfo);

    m_CreateAreaDialog->setProgressInfo("finished",100);
    m_CreateAreaDialog->close();

#ifdef Q_OS_LINUX
    sleep(1);
#else
    Sleep(1);
#endif

}

void LineManager::slotUpdateArea()
{
    //1  更新工区-------------
    QStringList spsFiles = m_UpdateAreaDialog->spsFiles();
    int ok = 0; QString errStr;
    m_UpdateAreaDialog->setProgressInfo("Updating Area...",1);
    ok = Area::updateArea(m_projInfo,m_currentArea,spsFiles,m_UpdateAreaDialog->updateOption());
    if(ok !=0 )
    {
        qDebug()<<"update area ok:"<<ok;
        QMessageBox::warning(this,"error","Update Area Failed.");
        return;
    }

    //2 读取数据--------------------
    spsFiles = Area::spsFiles(m_projInfo,m_currentArea);
    m_UpdateAreaDialog->setProgressInfo("Reading sps files...",20);
    AreaDataInfo  *areaDataInfo = new AreaDataInfo;
    ok = gobs_sps_files_SR_read(spsFiles,areaDataInfo,&errStr);
    if(ok !=0 )
    {
        QMessageBox::warning(this,"error","Read SPS File Error:"+errStr);
        return;
    }

    m_UpdateAreaDialog->setProgressInfo("Updating...",40);

    //直接清除投放信息，可能站点信息更改
    bool ret = SqlFunctions::instance()->clearTable(m_currentArea);
    if(!ret)
        throw "Clear DataBase Table error!";
    //显示当前工区设备信息
    m_RNDepolyments->setCurrentArea(m_currentArea);
    //更新统计信息
    m_StationsInfoWg->setRNInfoNum(m_RNDepolyments->depolyedCount()
                                   ,m_RNDepolyments->pickedupCount());

    //扫描当前工区的设备
    m_depolySystem->setCurrentArea(m_currentArea);

    //保存数据--------------------
    if(m_AreaDataInfo)
        delete m_AreaDataInfo;
    m_AreaDataInfo = areaDataInfo;
    //传递绘图数据初始化
    m_AreaView->setAreaData(m_AreaDataInfo);

    m_UpdateAreaDialog->setProgressInfo("finished",100);
    m_UpdateAreaDialog->close();
}

void LineManager::slotOpenArea(const QString &areaName)
{
    qDebug()<<"open area name:"<<areaName;
    if(areaName == m_currentArea)
    {
        QMessageBox::warning(this,"warning"
                             ,QString("Area %1 has Opened").arg(areaName));
        return;
    }

    //读取工区数据
    QStringList spsfiles = Area::spsFiles(m_projInfo,areaName);
    qDebug()<<"sps files:"<<spsfiles;
    AreaDataInfo *areaDataInfo = new AreaDataInfo;
    int ok = 0;QString errStr;
    ok = gobs_sps_files_SR_read(spsfiles,areaDataInfo,&errStr);
    if(ok != 0)
    {
        QMessageBox::warning(this,"warning"
                             ,QString("Read data failed."));
        return;
    }

    setCurrentAreaData(areaName,areaDataInfo);
}

void LineManager::setCurrentAreaData(const QString &areaName,AreaDataInfo *areaDataInfo)
{
    if(m_AreaDataInfo)
        delete m_AreaDataInfo;

    m_AreaDataInfo = areaDataInfo;

    updateMenuToolStatus(true); //更新菜单，使能
    setCurrentArea(areaName);   //设置当前工区

    //显示当前工区设备信息
    m_RNDepolyments->setCurrentArea(areaName);

    //更新统计信息
    m_StationsInfoWg->setRNInfoNum(m_RNDepolyments->depolyedCount()
                                   ,m_RNDepolyments->pickedupCount());

    //扫描当前工区的设备
    m_depolySystem->setCurrentArea(areaName);

    //传递绘图数据初始化
    m_AreaView->setAreaData(m_AreaDataInfo);

    //刷新数据统计信息
    m_StationsInfoWg->setStationNum(m_AreaView->shotNum(),m_AreaView->recvNum());

    //设置查找范围
    {
        m_searchEdit->setEnabled(true);
    }
}

void LineManager::slotDeleteArea(const QString &areaName)
{
    qDebug()<<"delete area name:"<<areaName;
    //删除工区是否是当前工区
    if(areaName == m_currentArea)
    {
        //是当前工区，先关闭工区----
        slotCloseCurrent();
    }
    int ok = Area::deleteArea(m_projInfo,areaName);
    qDebug()<<"deleted area --------------";
    if(ok != 0)
    {
        QMessageBox::warning(this,"warning"
                             ,QString("Delete Area Failed.ok=%1").arg(ok));
        return;
    }
    //删除工区对应的数据库表
    SqlFunctions::instance()->dropTable(areaName);

    //更新工区列表--------
    m_AreaListDlg->updateAreaList();

    if(m_AssignNodeDlg)
        m_AssignNodeDlg->deleteArea(m_currentArea);
    if(m_depolySystem)
        m_depolySystem->deleteArea(m_currentArea);
}

void LineManager::slotDepolySysConnected(const bool &connected)
{
    m_menuManger->setDeploySysConnected(connected);
}

void LineManager::setCurrentArea(const QString &areaName)
{
    m_currentArea = areaName;
    //设置窗口标题
    this->setWindowTitle("LMGR_"+m_currentArea);
}

void LineManager::setDepolySysCommStatus(const bool &connected)
{
    QColor color;
    if(connected)
        color = Qt::green;
    else
        color = Qt::red;
    //------------------------------

    QPixmap pix(16,16);
    pix.fill(this->palette().color(QPalette::Window));
    QPainter painter(&pix);
    painter.setBrush(color);
    painter.drawEllipse(1,1,14,14);
    m_ComStatusLbl->setPixmap(pix);
}

void LineManager::updateRecvedDevice(const QString &areaName)
{
    if(areaName == m_currentArea)
        m_RNDepolyments->initDepolyments();

}
void LineManager::slotUpdateRNDepolyments()
{
    //更新当前统计信息
    m_StationsInfoWg->setRNInfoNum(m_RNDepolyments->depolyedCount()
                                   ,m_RNDepolyments->pickedupCount());

    //更新站点状态
    m_AreaView->updateRecieverStatus();
}

void LineManager::slotSerachDevice()
{
    QString device = m_searchEdit->text();
    device  = "G" +device.mid(1).rightJustified(3,QChar('0'));

    SqlFunctions *sqlFunc = SqlFunctions::instance();

    QStringList fieldList;
    fieldList<<TABLE_FIELD_LINE
            <<TABLE_FIELD_STATION;
    QList<Condition> conds;
    Condition cond;
    cond.fieldName = TABLE_FIELD_DEVICE;
    cond.fieldValue= device;
    conds.append(cond);
    bool ok = false;
    QSqlQuery query = sqlFunc->query(m_currentArea,fieldList,conds,&ok);

    while(query.next()){
        if(query.value(0).toInt()>0 && query.value(1).toInt()>0){
            m_AreaView->searchRecvItem(query.value(0).toFloat()
                                       ,query.value(1).toInt());
            return ;
        }
    }

    QMessageBox::warning(this,"warning","The area does not contain the GOBS.");
}

void LineManager::slotupdateRecvCoord(float line,int station,Coordinate cord)
{
    return;
    //更新对应的站点信息
    StationInfo *recvStationInfo = m_AreaDataInfo->recvStationInfo;
    for(int i=0;i<recvStationInfo->lineNum;i++)
    {
        if(recvStationInfo->staLines[i].line == line){
            StaLine staLine = recvStationInfo->staLines[i];
            for(int j=0;j<staLine.staNum;j++)
                if(staLine.stations[j].sp == station){
                    staLine.stations[j].x = cord.x;
                    staLine.stations[j].y = cord.y;
                    staLine.stations[j].del = cord.z;
                    break;
                }
            break;
        }
    }

    //更新R 文件
    //    QStringList spsFiles =  Area::spsFiles(m_projInfo,m_currentArea);
    //    QFile file(spsFiles[0]);
    //    file.open(QIODevice::ReadOnly);
    //    QString   str=  file.readAll();
    //    file.close();
}

void LineManager::initWindow()
{
    //创建菜单------
    m_menuManger->createMenu(ui->menuBar,this);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0,0,0,0);
    //创建工具栏----
    QHBoxLayout *toolBarLayout = new QHBoxLayout;
    QToolBar   *m_toolBar = new QToolBar(this);
    m_toolManager->createToolBar(m_toolBar,this);
    toolBarLayout->addWidget(m_toolBar);
    //查看某个接收点设备
    m_toolBar->addSeparator();
    QHBoxLayout *searchLayout = new QHBoxLayout;
    QLabel *devLbl = new QLabel("Depolyed GOBS:");
    searchLayout->addWidget(devLbl);
    m_searchEdit = new QLineEdit;
    toolBarLayout->addWidget(devLbl);
    toolBarLayout->addWidget(m_searchEdit);
    m_searchEdit->setInputMask("G000; ");
    m_searchEdit->setMaximumWidth(64);
    m_searchEdit->setEnabled(false);
    connect(m_searchEdit,SIGNAL(returnPressed()),this,SLOT(slotSerachDevice()));

    toolBarLayout->addStretch(1);
    //Comm State - DeplSys
    QLabel *comStsLbl = new QLabel("Comm Status:");
    toolBarLayout->addWidget(comStsLbl);

    QFrame *dplSysFr = new QFrame(this);
    dplSysFr->setFrameShape(QFrame::Box);
    dplSysFr->setFrameShadow(QFrame::Raised);
    toolBarLayout->addWidget(dplSysFr);
    QHBoxLayout *dplSysLayout = new QHBoxLayout;

    m_ComStatusLbl = new QLabel("");
    m_ComStatusLbl->setFixedWidth(20);
    setDepolySysCommStatus(false);     //初始设置通信状态为断开
    QLabel *m_dplSysbl = new QLabel("RFID  ");
    dplSysLayout->addWidget(m_ComStatusLbl);
    dplSysLayout->addWidget(m_dplSysbl);

    dplSysFr->setLayout(dplSysLayout);

    centralLayout->addLayout(toolBarLayout);

    //information
    m_StationsInfoWg = new StationsInfoWg(this);

    centralLayout->addWidget(ui->line);
    centralLayout->addWidget(m_StationsInfoWg);

    //图像显示区---------
    QVBoxLayout *grapLayout = new QVBoxLayout;
    grapLayout->setContentsMargins(6,0,6,4);
    m_AreaView = new LmgrAreaView(this);
    grapLayout->addWidget(m_AreaView);
    centralLayout->addLayout(grapLayout,1);

    //状态栏
    m_statusBar = new StationInfoBar(ui->statusBar);
    m_AreaView->setStatusBar(m_statusBar);

    ui->centralWidget->setLayout(centralLayout);
}

//slots--------------------------
//读取SPS 文件加载显示
void LineManager::slotNewArea()
{
    if(!m_CreateAreaDialog){
        m_CreateAreaDialog = new CreateAreaDialog(this);
        m_CreateAreaDialog->setProjectInfo(m_projPath,m_projName);
        connect(m_CreateAreaDialog,SIGNAL(signalCreateArea(QString)),this,SLOT(slotCreateArea(QString)));
        connect(m_CreateAreaDialog,SIGNAL(signalDeleteArea(QString)),this,SLOT(slotDeleteArea(QString)));
    }

    m_CreateAreaDialog->exec();
}

//打开工区
void LineManager::slotOpenArea()
{

    m_AreaListDlg->setAreaOpt(AreaListDlg::Open_Area);
    m_AreaListDlg->showAreaList();
}

void LineManager::slotUpdateAreaSPS()
{
    //更新工区SPS 文件信息
    if(!m_UpdateAreaDialog){
        m_UpdateAreaDialog = new UpdateAreaDialog(this);
        m_UpdateAreaDialog->setProjectInfo(m_projPath,m_projName);
        connect(m_UpdateAreaDialog,SIGNAL(signalUpdateArea()),this,SLOT(slotUpdateArea()));
    }
    m_UpdateAreaDialog->updateArea(m_currentArea);
}

void LineManager::slotUpdateCurrent(const QString &areaName)
{
    //刷新当前工区
    if(areaName.isEmpty() || (!areaName.isEmpty() && areaName == m_currentArea ))
    {
        //更新GOBS下载信息
        m_depolySystem->updateDownload();

        //刷新站点设备表
        m_RNDepolyments->initDepolyments(true);

        //更新当前统计信息
        m_StationsInfoWg->setRNInfoNum(m_RNDepolyments->depolyedCount()
                                       ,m_RNDepolyments->pickedupCount());


        //更新站点状态
        m_AreaView->updateRecieverStatus();
    }
}

void LineManager::slotCloseCurrent()
{
    //关闭当前工区,显示区域清除
    if(m_AreaDataInfo)
    {
        m_AreaView->setAreaData(0);//图像区域数据清 0
        delete m_AreaDataInfo;
        m_AreaDataInfo = 0;
    }
    if(m_RNDepolyments)
        m_RNDepolyments->closeCurrentArea();

    m_StationsInfoWg->clearData();
    setCurrentArea("");
    updateMenuToolStatus(false);
}

void LineManager::slotDeleteArea()
{
    if(!m_AreaListDlg){
        m_AreaListDlg = new AreaListDlg(this);
        m_AreaListDlg->setProjInfo(ProjectInfo(m_projPath,m_projName));
        connect(m_AreaListDlg,SIGNAL(signalOpenArea(QString)),this,SLOT(slotOpenArea(QString)));
        connect(m_AreaListDlg,SIGNAL(signalDeleteArea(QString)),this,SLOT(slotDeleteArea(QString)));
    }
    m_AreaListDlg->setAreaOpt(AreaListDlg::Delete_Area);
    m_AreaListDlg->showAreaList();
}

void LineManager::slotExit()
{
    this->close();
}


void LineManager::slotRNDeployments()
{
    m_RNDepolyments->show();
}

void LineManager::slotAssignRNDeployments()
{
    if(!m_AssignNodeDlg){
        m_AssignNodeDlg = new AssignNodeDlg(this);
        m_AssignNodeDlg->setProjInfo(m_projInfo);
        m_AssignNodeDlg->initDepolyments();
        //投放更新
        connect(m_AssignNodeDlg,SIGNAL(signalUpdateAssignedDev(QString))
                ,this,SLOT(slotUpdateCurrent(QString)));
    }
    m_AssignNodeDlg->show();
}

void LineManager::slotConfigDeploySys()
{
    m_depolySystem->show();
}

void LineManager::slotDis_ConDeploySys(const bool&con)
{
    m_depolySystem->connectToHost(con);
}


//View-----
void LineManager::slotShowRecvs(const bool &enable)
{
    m_AreaView->setShowItemFlag(BaseItem::RecvItem,enable);
}

void LineManager::slotShowShots(const bool &enable)
{
    m_AreaView->setShowItemFlag(BaseItem::ShotItem,enable);
}

void LineManager::slotShowRecvLines(const bool &enable)
{
    m_AreaView->setShowItemFlag(BaseItem::RecvLineItem,enable);
}

void LineManager::slotShowShotLines(const bool &enable)
{
    m_AreaView->setShowItemFlag(BaseItem::ShotLineItem,enable);
}

void LineManager::slotShowRecvText(const bool &enable)
{
    m_AreaView->setShowItemFlag(BaseItem::RecvTextItem,enable);
}

void LineManager::slotShowShotText(const bool &enable)
{
    m_AreaView->setShowItemFlag(BaseItem::ShotTextItem,enable);
}

void LineManager::slotPreference()
{
    m_AreaView->setPreference();
}

void LineManager::slotAbout()
{
    Assistant::showAssistant("LMGR");
}

//ZoomIn 5%
void LineManager::slotZoomIn()
{
    m_AreaView->zoomIn();
}

//ZoomOut 5%
void LineManager::slotZoomOut()
{
    m_AreaView->zoomOut();
}

void LineManager::slotZoomInFull()
{
    m_AreaView->zoomInFull();
}

void LineManager::slotZoomOutFull()
{
    m_AreaView->zoomOutFull();
}

