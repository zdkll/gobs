#include "datamgrcontrol.h"
#include "ui_datamgrcontrol.h"

#include <QBoxLayout>
#include <QFrame>
#include <QDesktopWidget>
#include <QLabel>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "gassistant.h"

DataMgrControl::DataMgrControl(QWidget *parent) :
    QMainWindow(parent)
  ,CommModule()
  ,ui(new Ui::DataMgrControl)
{
    ui->setupUi(this);
    this->setWindowTitle("DMC(Main Window)|Project:NULL");

    m_menuMnager = new MenuManager(this);
    m_toolManager = new ToolManager(this);

    initWindow();

    m_OpenSegyDlg    = 0;
    m_ExportSegyDlg  = 0;
    m_gatherRecord   = 0;
    m_paramDlg       = new PlotParamDlg(m_QCPlotter,this);

    QRect rect = QApplication::desktop()->screenGeometry();
    this->resize(rect.width()*2/3,rect.height()*3/4);
    this->move((rect.width()-width())/2,(rect.height()-height())/2);

    m_maxShowTraces  = -1;

    m_headers        = 0;
    m_data           = 0;

    //初始化读取道集对象，计算和显示道集
    m_gatherRecord = new GatherRecord(this);

    connect(m_gatherRecord,SIGNAL(signalReadGather(int)),this,SLOT(readGather(int)));

}

DataMgrControl::~DataMgrControl()
{
    if(m_headers)
        delete[] m_headers;
    if(m_data)
        delete[] m_data;

    if(m_gatherInfo)
        delete m_gatherInfo;

    delete ui;
}

bool DataMgrControl::init()
{
    this->setWindowTitle(QString("DMC(Main Window)|Project:%1")
                         .arg(m_projName));

    m_gatherInfo     = new GatherInfo;

    //初始化道集信息
    m_QCPlotter->Initial(ui->statusBar,m_gatherInfo);

    m_jobManager->setProjInfo(m_projInfo);
    m_jobManager->init(ui->jobList,ui->logBrowser);

    return true;
}


void DataMgrControl::initWindow()
{
    //任务信息显示---------------------
    ui->mainSplitter->setStretchFactor(1,1);
    m_jobManager = new JobManager(this);

    ui->tabWidget->setCurrentIndex(1);

    //图像显示------------------------
    //创建菜单
    m_menuMnager->createMenu(this,ui->menuBar);

    //创建工具栏
    m_toolManager->createTools(this,ui->mainToolBar);

    //数据管理页

    //数据图像页-----
    m_QCPlotter = new QCPlotter(this);
    QVBoxLayout *plotterLayout = new QVBoxLayout;
    plotterLayout->addWidget(m_QCPlotter);
    plotterLayout->setContentsMargins(0,0,0,0);
    ui->tabWidget->widget(1)->setLayout(plotterLayout);
}

//菜单File选项槽函数
void DataMgrControl::slotNewSegyData()
{
    if(!m_ExportSegyDlg){
        m_ExportSegyDlg = new ExportSegyDlg(this);
        connect(m_ExportSegyDlg,SIGNAL(signalStartExport(ExportSegyParameter))
                ,this,SLOT(startExportSegyData(ExportSegyParameter)));
        m_ExportSegyDlg->setProjectInfo(m_projInfo);
    }
    m_ExportSegyDlg->exec();
}

//打开segy 文件
void DataMgrControl::slotOpenSegyFile()
{   
    if(!m_OpenSegyDlg){
        m_OpenSegyDlg = new OpenSegyDlg(this);
        connect(m_OpenSegyDlg,SIGNAL(signalOpenedFile(QList<SegyFileHandle*>)),this
                ,SLOT(slotOpenedFile(QList<SegyFileHandle *>)));
    }
    m_OpenSegyDlg->exec();
}

void DataMgrControl::slotExit()
{
    this->close();
}

void DataMgrControl::slotPreviousGather()
{
    //是否有文件
    if(m_FileHandles.size()>0)
        m_gatherRecord->readPrevGather();
}

void DataMgrControl::slotNextGather()
{
    //是否有文件
    if(m_FileHandles.size()>0)
        m_gatherRecord->readNextGather();
}

void DataMgrControl::slotMoveToGather()
{
    if(m_FileHandles.size()>0)
        m_gatherRecord->show();
}

void DataMgrControl::slotZoomIn(const bool &enable)
{
    //qDebug()<<"zoom in enable:"<<enable;
    if(enable)
    {
        m_QCPlotter->setMouseFunction(MF_ZOOM);
    }else
        m_QCPlotter->setMouseFunction(MF_NOTHING);
}

void DataMgrControl::slotZoomOut()
{
    m_QCPlotter->zoomOut();
}

void DataMgrControl::slotTraceInform(const bool &enable)
{
    if(enable)
    {
        m_QCPlotter->setMouseFunction(MF_ANALYSE);
    }else
        m_QCPlotter->setMouseFunction(MF_NOTHING);
}

void DataMgrControl::slotDisplayOption()
{
    //图像参数控制面板
    m_paramDlg->show();
}

void DataMgrControl::slotAbout()
{
    Assistant::showAssistant("DMC");
}


void DataMgrControl::startExportSegyData(const ExportSegyParameter &parameter)
{
    ui->tabWidget->setCurrentIndex(0);
    //保存job 参数
    QString jobFile = parameter.projectPath+"/work/"+parameter.jobName;

    bool ok = saveJobParameter(jobFile,parameter);
    if(!ok)
    {
        QMessageBox::critical(this,"error","Save job file failed.");
        return ;
    }

    //参数,job 文件------
    QStringList argList;
    argList<<jobFile;
    QString program;
#ifdef Q_OS_LINUX
    //程序
    program = QApplication::applicationDirPath()
            +Dir_Separator+"ExportSegyProgram";
#else  //Q_OS_WIN
    program = QApplication::applicationDirPath()
            +Dir_Separator+"ExportSegyProgram.exe";
#endif

    QProcess process;

    process.startDetached(program,argList);
}

bool DataMgrControl::saveJobParameter(const QString &jobFile,const ExportSegyParameter &paramter)
{
    QFile file(jobFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,"Error","Open Job file Error.");
        return false;
    }

    //保存成json 格式
    QJsonDocument doc;
    QJsonObject   jsobj;

    //保存参数----
    jsobj.insert(Json_Key_JobName,paramter.jobName);
    jsobj.insert(Json_Key_JobType,paramter.segyType);
    jsobj.insert(Json_Key_ProjectPath,paramter.projectPath);
    jsobj.insert(Json_Key_ProjectName,paramter.projectName);
    jsobj.insert(Json_key_AreaName,paramter.areaName);
    jsobj.insert(Json_key_DataPath,paramter.dataPath);
    jsobj.insert(Json_key_OutputPath,paramter.outputPath);

    jsobj.insert(Json_key_LineScope,QString("%1|%2").arg(paramter.lineScope.first)
                 .arg(paramter.lineScope.second));
    jsobj.insert(Json_key_StationScope,QString("%1|%2").arg(paramter.staScope.first)
                 .arg(paramter.staScope.second));

    //共检波点数据类型
    if(paramter.segyType == 0){
        jsobj.insert(Json_key_NS,paramter.traceNs);
        jsobj.insert(Json_key_DS,paramter.ds);
        //保存炮线文件，卷头文本
        QJsonArray shotLinesArray;
        foreach (QString shotLine, paramter.shotLineFiles) {
            shotLinesArray.append(shotLine);
        }
        jsobj.insert(Json_key_ShotLineFiles,shotLinesArray);
        jsobj.insert(Json_key_SegyTextHeader,paramter.textHeader);
    }

    doc.setObject(jsobj);
    file.write(doc.toJson());
    file.close();

    return true;
}

//打开文件后初始化道集控制
void DataMgrControl::slotOpenedFile(QList<SegyFileHandle*> segyFileHandles)
{
    ui->tabWidget->setCurrentIndex(1);
    m_FileHandles      = segyFileHandles;
    int maxShowTraces = m_OpenSegyDlg->maxShowTraces();
    int startTrace    = m_OpenSegyDlg->startTrace();

    m_startTrace = startTrace - maxShowTraces*ceil(float(startTrace)/float(maxShowTraces));
    //开辟数据空间
    try{
        m_maxShowTraces = maxShowTraces;
        if(m_data)
            delete [] m_data;
        if(m_headers)
            delete[] m_headers;

        //申请内存
        m_headers = new TraceHead[m_maxShowTraces*m_FileHandles.size()];

        //数据类型，采样点数，道数
        int ns = m_FileHandles[0]->binaryhead->nt;
        int dtype = m_FileHandles[0]->binaryhead->samptype;
        int byte_num = 4;
        if(dtype == 3)
            byte_num = 2;
        m_data = new char[m_maxShowTraces*ns*byte_num*m_FileHandles.size()];

        m_gatherInfo->gatherName = m_FileHandles[0]->fileName.mid(m_FileHandles[0]->fileName.lastIndexOf("/")+1);
        m_gatherInfo->Ns = ns;
        m_gatherInfo->DType = dtype;
        m_gatherInfo->NTraces = 0;
        m_gatherInfo->sampleInterval =  m_FileHandles[0]->segyInfo->sampleInterval;
    }
    catch(std::bad_alloc &){
        QMessageBox::critical(this,"Error"
                              ,"The Application will exit after catch an bad_alloc exception.");
        exit(EXIT_FAILURE);
    }
    catch(...){
        QMessageBox::critical(this,"Error"
                              ,"The Application will exit caused by alloc memory.");
        exit(EXIT_FAILURE);
    }

    m_gatherRecord->setGatherInfo(m_FileHandles[0]->segyInfo->traces
            ,m_OpenSegyDlg->maxShowTraces(),startTrace);

    m_QCPlotter->setComponents(m_OpenSegyDlg->components());

    this->setWindowTitle(QString("%1_DMC(Main Window)|Project:%2")
                         .arg(m_FileHandles[0]->fileName).arg(m_projName));
}

void DataMgrControl::readGather(const int &gatherNo)
{
    //读取数据,当前显示道集号-----------------
    //起始道
    long long startTrace =  m_startTrace+m_maxShowTraces*(gatherNo-1);
    startTrace = startTrace<0?0:startTrace;
    //读取数据---------------------
    int ok = -1;
    long long readTraces   = 0;
    long long total_traces = 0;
    int ns = m_gatherInfo->Ns;
    int bypes = 4;
    for(int i=0;i<m_FileHandles.size();i++){
        gobs_read_segy_func(m_FileHandles[i]->openTrace,startTrace,m_maxShowTraces
                            ,m_headers+total_traces,m_data+total_traces*ns*bypes
                            ,&readTraces,&ok);
        if(ok != 0){
            QMessageBox::critical(this,"Error",QString("Read Gather %1 Failed,ok=%2.")
                                  .arg(gatherNo)
                                  .arg(ok));
            return;
        }
        if(readTraces == 0){
            QMessageBox::critical(this,"Error",QString("Gather %1 has no trace.")
                                  .arg(gatherNo));
            return;
        }
        total_traces += readTraces;
    }

    //设置切换道集选项使能
    int gatherNum = m_gatherRecord->gatherNumer();
    QList<QAction *> toolActions = ui->mainToolBar->actions();
    if(gatherNo == gatherNum)
        toolActions[3]->setEnabled(false);
    else
        toolActions[3]->setEnabled(true);
    if(gatherNo == 1)
        toolActions[2]->setEnabled(false);
    else
        toolActions[2]->setEnabled(true);

    m_gatherInfo->NTraces = total_traces;
    qDebug()<<"read gather no:"<<gatherNo
           <<"read traces:"<<m_gatherInfo->NTraces;
    m_gatherInfo->gatherNum = m_gatherRecord->gatherNumer();
    m_gatherInfo->gatherNo  = m_gatherRecord->gatherNo();
    //读取数据后，设置图像数据，更新显示
    m_QCPlotter->setData(m_headers,m_data);
}

//JobManager---------------------------
JobManager::JobManager(QObject *parent)
    :QObject(parent)
{
    m_fileTimer = 0;
    m_logTimer  = 0;
    m_interval  = 5000;
    menu = 0;
}
JobManager::~JobManager()
{
    if(menu)
        delete menu;
}

void JobManager::init(QListWidget *jobList,QTextBrowser *logBrowser)
{
    m_jobList    = jobList;
    m_logBrowser = logBrowser;

    connect(m_jobList,SIGNAL(itemDoubleClicked(QListWidgetItem*))
            ,this,SLOT(slotJobItemClicked(QListWidgetItem*)));

    //右键：查看日志，重新提交作业功能
    m_jobList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_jobList,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slotCustomContextMuenuReqested(QPoint)));

    //开始刷新日志
    refreshFiles();
    m_fileTimer = this->startTimer(5000);
}

void JobManager::timerEvent(QTimerEvent *e)
{
    //刷新文件列表
    if(e->timerId() == m_fileTimer){
        refreshFiles();
    }

    //刷新日志
    if(e->timerId() == m_logTimer){
        refreshLog();
    }
}

void JobManager::slotJobItemClicked(QListWidgetItem* item)
{
    m_currentFile = item->text();
    refreshLog();
}

void JobManager::slotCustomContextMuenuReqested(QPoint pt)
{
    if(!m_jobList->itemAt(pt))
        return;

    if(!menu){
        menu = new QMenu();
        menu->addAction("Show Log",this,SLOT(slotShowLog()));
        menu->addAction("Resubmit",this,SLOT(slotResubmit()));
    }
    menu->exec(QCursor::pos());
}

void JobManager::slotShowLog()
{
    m_currentFile = m_jobList->currentItem()->text();
    refreshLog();
}

void JobManager::slotResubmit()
{
    QString jobFile = m_jobList->currentItem()->text();
    jobFile         = m_projInfo.ProjectPath +"/work/"+ jobFile;

    qDebug()<<jobFile;
    QProcess    process;
    QStringList argList;
    argList.append(jobFile);

#ifdef Q_OS_LINUX
    process.startDetached("ExportSegyProgram",argList);
#else
    process.startDetached("ExportSegyProgram.exe",argList);
#endif

}

void JobManager::refreshFiles()
{
    QDir dir(m_projInfo.ProjectPath+"/work/");

    QFileInfoList logFiles = dir.entryInfoList(QStringList()<<"*.log",
                                               QDir::Files,QDir::Time);
    m_jobList->clear();
    foreach (QFileInfo fileInfo, logFiles) {
        m_jobList->addItem(fileInfo.baseName());
    }
}

void JobManager::refreshLog()
{
    if(m_currentFile.isEmpty())
        m_logBrowser->clear();

    m_logBrowser->verticalScrollBar()->value();
    QFile file(m_projInfo.ProjectPath+"/work/"+m_currentFile+".log");
    file.open(QIODevice::ReadOnly);
    QTextStream out(&file);
    m_logBrowser->setText(out.readAll());
    file.close();
    m_logBrowser->verticalScrollBar()->setValue(m_logBrowser->verticalScrollBar()->maximum());
}


void JobManager::setAutoRefreshEnable(bool enable)
{

    if(enable)
        m_logTimer = this->startTimer(m_interval);
    else{
        this->killTimer(m_logTimer);
        m_logTimer = 0;
    }
}

void JobManager::setLogRefreshInterval(int interval)
{
    m_interval = interval*1000;
    if(m_logTimer>0)
    {
        this->killTimer(m_logTimer);
        m_logTimer = this->startTimer(m_interval);
    }
}

void DataMgrControl::on_autoRefreshCbx_clicked(bool checked)
{
    ui->spinBox->setEnabled(checked);
    m_jobManager->setAutoRefreshEnable(checked);
}

void DataMgrControl::on_spinBox_valueChanged(int arg1)
{
    m_jobManager->setLogRefreshInterval(arg1);
}

void DataMgrControl::on_refreshBtn_clicked()
{
    m_jobManager->refreshLog();
}
