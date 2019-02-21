#include "rnmgrunit.h"
#include "rnmanager.h"

#include <QStyledItemDelegate>
#include <QDesktopWidget>


//状态刷新时间
#define  Update_Interval_3    "3s"
#define  Update_Interval_5    "5s"
#define  Update_Interval_10   "10s"
#define  Update_Interval_20   "20s"
#define  Update_Interval_30   "30s"


#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHostInfo>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QCloseEvent>
#include <QSpinBox>

#include "searchthread.h"
#include "gpublicwidgets.h"

MenuManager::MenuManager(QObject *parent)
    :QObject(parent)
{

}

void MenuManager::createMenu(QMenuBar *menuBar,RNManager *rmnodemanager)
{
    menuFile = new QMenu("File");
    menuFile->addAction(tr("&Set Search IP Scope..."),rmnodemanager,SLOT(slotSetSearchIpScope()));
    menuFile->addSeparator();
    menuFile->addAction(tr("&Quit"),rmnodemanager,SLOT(slotQuit()),QKeySequence(tr("Ctrl+Q")));
    menuBar->addMenu(menuFile);

    //edit----
    menuEdit = new QMenu("Edit");
    menuEdit->addAction(tr("&Limits"),rmnodemanager,SLOT(slotLimits()));
    menuEdit->addSeparator();

    QMenu *update_starus_interval = new QMenu("Update Status Interval");
    menuEdit->addMenu(update_starus_interval);

    //update_starus_interval-------------------------------
    QVBoxLayout *btnLayout = new QVBoxLayout();
    QButtonGroup *m_btnGroup = new QButtonGroup(this);

    QRadioButton *timeBtn1 = new QRadioButton(Update_Interval_3);
    QRadioButton *timeBtn2 = new QRadioButton(Update_Interval_5);
    QRadioButton *timeBtn3 = new QRadioButton(Update_Interval_10);
    QRadioButton *timeBtn4 = new QRadioButton(Update_Interval_20);
    QRadioButton *timeBtn5 = new QRadioButton(Update_Interval_30);

    //垂直布局
    btnLayout->addWidget(timeBtn1);
    btnLayout->addWidget(timeBtn2);
    btnLayout->addWidget(timeBtn3);
    btnLayout->addWidget(timeBtn4);
    btnLayout->addWidget(timeBtn5);
    //按钮添加至按钮组
    m_btnGroup->addButton(timeBtn1);
    m_btnGroup->addButton(timeBtn2);
    m_btnGroup->addButton(timeBtn3);
    m_btnGroup->addButton(timeBtn4);
    m_btnGroup->addButton(timeBtn5);
    timeBtn2->setChecked(true);
    update_starus_interval->setLayout(btnLayout);
    m_btnGroup->setExclusive(true);
    connect(m_btnGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(slotUpdateIntervalChanged(QAbstractButton*)));
    connect(this,SIGNAL(signalUpdateIntervalChanged(int)),rmnodemanager,SLOT(slotSetUpdateInterval(int)));
    //start/stop update------
    start_stop_update_act = new QAction("&Start/Stop Update",menuEdit);
    start_stop_update_act->setCheckable(true);
    connect(start_stop_update_act,SIGNAL(triggered(bool)),rmnodemanager,SLOT(slotStartStopUpdate(bool)));
    menuEdit->addAction(start_stop_update_act);
    menuEdit->addSeparator();

    //采样率设置，下载管理
    menuEdit->addAction(tr("&Node Ftp Manager..."),rmnodemanager,SLOT(slotNodeFtpManager()));
    menuBar->addMenu(menuEdit);

    //view-------------------------
    menuView = new QMenu("View");
    QAction *allConnectAct = new QAction("Show All/Connected",menuView);
    allConnectAct->setCheckable(true);
    allConnectAct->setChecked(false);
    connect(allConnectAct,SIGNAL(triggered(bool)),rmnodemanager,SLOT(slotShowAllOrConnected(bool)));
    menuView->addAction(allConnectAct);
    menuView->addAction(tr("&Show Status Report..."),rmnodemanager,SLOT(slotShowStatusReport()));
    menuView->addSeparator();


    QMenu  *displayFilterMenu = new QMenu(tr("&Display Filters"),menuView);
    menuView->addMenu(displayFilterMenu);
    QVBoxLayout *vFiltersLayout = new QVBoxLayout;

    //Filters
    filtersGroupBox = new QButtonGroup(this);

    QRadioButton *nodeStatusBtn = new QRadioButton("Node Status");
    nodeStatusBtn->setChecked(true);
    QRadioButton *voltageBtn = new QRadioButton("Voltage");
    QRadioButton *temperatureBtn = new QRadioButton("Temperature");
    QRadioButton *pressureBtn = new QRadioButton("Pressure");
//    QRadioButton *eleCurrBtn = new QRadioButton("EleCurr");
//    QRadioButton *chargeVoltBtn = new QRadioButton("Charge Voltage");
    QRadioButton *storageBtn = new QRadioButton("Memory Capacity");
    QRadioButton *selfTestBtn = new QRadioButton("Self Test");
    QRadioButton *downLoadBtn = new QRadioButton("Downloaded");

    vFiltersLayout->addWidget(nodeStatusBtn);
    filtersGroupBox->addButton(nodeStatusBtn,Status);
    vFiltersLayout->addWidget(voltageBtn);
    filtersGroupBox->addButton(voltageBtn,Voltage);
    vFiltersLayout->addWidget(temperatureBtn);
    filtersGroupBox->addButton(temperatureBtn,Temperature);
    vFiltersLayout->addWidget(pressureBtn);
    filtersGroupBox->addButton(pressureBtn,Pressure);
//    vFiltersLayout->addWidget(eleCurrBtn);
//    filtersGroupBox->addButton(eleCurrBtn,EleCurr);
//    vFiltersLayout->addWidget(chargeVoltBtn);
//    filtersGroupBox->addButton(chargeVoltBtn,ChargeVolt);
    vFiltersLayout->addWidget(storageBtn);
    filtersGroupBox->addButton(storageBtn,Memory_Capacity);
    vFiltersLayout->addWidget(selfTestBtn);
    filtersGroupBox->addButton(selfTestBtn,SelfTest);
    vFiltersLayout->addWidget(downLoadBtn);
    filtersGroupBox->addButton(downLoadBtn,DownLoad);

    filtersGroupBox->setExclusive(true);

    connect(filtersGroupBox,SIGNAL(buttonClicked(int)),rmnodemanager,SLOT(slotDisplayFilters(int)));

    //设置间隔
    displayFilterMenu->setLayout(vFiltersLayout);

    //Print Warning Information

    QAction *printWarnInfoAct = new QAction("Print/Ignore Warning Message",menuView);
    printWarnInfoAct->setCheckable(true);
    printWarnInfoAct->setChecked(true);
    connect(printWarnInfoAct,SIGNAL(toggled(bool))
            ,rmnodemanager,SLOT(slotPrintWarningEnable(bool)));
    menuView->addAction(printWarnInfoAct);
    menuBar->addMenu(menuView);

    //Help---
    menuAbout = new QMenu("Help");
    menuAbout->addAction(tr("&About..."),rmnodemanager,SLOT(slotAbout()));
    menuBar->addMenu(menuAbout);

    setDisplayedNode(false);
}


void MenuManager::setUpdateStatus(const bool &start)
{
    if(start_stop_update_act->isChecked() != start)
        start_stop_update_act->setChecked(start);
}

void MenuManager::setDisplayedNode(const bool &displayed)
{
    //Edit
    QList<QAction *> edit_actionList = menuEdit->actions();
    edit_actionList[3]->setEnabled(displayed);//update status
    edit_actionList[5]->setEnabled(displayed);

    //View
    QList<QAction *> view_actionList = menuView->actions();
    view_actionList[0]->setEnabled(displayed);//显示全部或者已经链接节点项
    view_actionList[1]->setEnabled(displayed);//全部状态报告
}

void MenuManager::setDisplayFilters(const int &status)
{
    filtersGroupBox->button(status)->setChecked(true);
}

void MenuManager::slotUpdateIntervalChanged(QAbstractButton *btn)
{
    //    qDebug()<<"update interVal:"<<btn->text();
    QString text = btn->text();
    int interval = 1000;
    if(text == Update_Interval_3)
        interval*=3;
    else if(text == Update_Interval_5)
        interval*=5;
    else if(text == Update_Interval_10)
        interval*=10;
    else if(text == Update_Interval_20)
        interval*=20;
    else if(text == Update_Interval_30)
        interval*=30;
    emit signalUpdateIntervalChanged(interval);
}


//--------------------------
ToolManager::ToolManager(QObject *parent)
    :QObject(parent)
{
    m_Path = Core::rootPath();
}

void ToolManager::createToolBar(QToolBar *toolBar,RNManager *rmnodemanager)
{
    //文件目录-------------
    m_toolBar = toolBar;

    //set search Ip Scope
    toolBar->addAction(QIcon(m_Path+ICON_SEARCH_SET),"Set Search IP Scope",rmnodemanager,SLOT(slotSetSearchIpScope()));
    toolBar->addSeparator();

    updateAction = new QAction(QIcon(m_Path+ICON_START),"Start Update",toolBar);
    updateAction->setData(false);
    connect(updateAction,SIGNAL(triggered(bool)),this,SLOT(slotStartOrStopUpdate()));
    connect(this,SIGNAL(signalStartOrStopUpdate(bool)),rmnodemanager,SLOT(slotStartStopUpdate(bool)));
    toolBar->addAction(updateAction);

    //down load and storage manager
    toolBar->addAction(QIcon(m_Path+ICON_DATA_MANAGER),"DownLoad & Storage Manager",rmnodemanager, SLOT(slotNodeFtpManager()));

    //status report
    toolBar->addAction(QIcon(m_Path+ICON_ITEM_LIST),"Status Report",rmnodemanager, SLOT(slotShowStatusReport()));

    toolBar->addSeparator();

    QLabel *devNoLbl = new QLabel("Serial.No:");
    m_searchEdit = new QLineEdit("G");
    m_searchEdit->setInputMask("G000; ");
    m_searchEdit->setCursorPosition(1);
    m_searchEdit->setMaximumWidth(80);
    toolBar->addWidget(devNoLbl);
    toolBar->addWidget(m_searchEdit);

    connect(m_searchEdit,SIGNAL(returnPressed()),this,SLOT(slotSearchNode()));

    setDisplayedNode(false);
}

void ToolManager::setUpdateStatus(const bool &start)
{
    if(updateAction->data().toBool()!=start)
    {
        if(!start)
        {
            updateAction->setIcon(QIcon(m_Path+ICON_START));
            updateAction->setToolTip("Start Update");
            updateAction->setData(false);
        }else
        {
            updateAction->setIcon(QIcon(m_Path+ICON_STOP));
            updateAction->setToolTip("Stop Update");
            updateAction->setData(true);
        }
    }
}

void ToolManager::setDisplayedNode(const bool &displayed)
{
    QList<QAction *> actionList = m_toolBar->actions();
    actionList[2]->setEnabled(displayed);
    actionList[3]->setEnabled(displayed);
    actionList[4]->setEnabled(displayed);
}

void ToolManager::slotSetUpdateInterval(const QString &text)
{
    int interval = 1000;
    if(text == Update_Interval_3)
        interval *= 3;
    else if(text == Update_Interval_5)
        interval *= 5;
    else if(text == Update_Interval_10)
        interval *= 10;
    else if(text == Update_Interval_20)
        interval *= 20;
    else if(text == Update_Interval_30)
        interval *= 30;

    emit signalUpdateIntervalChanged(interval);
}
void ToolManager::slotSearchNode()
{
    uint dev_no = m_searchEdit->text().mid(1).toUInt();
    emit signalSearchNode(dev_no);
}

void ToolManager::slotStartOrStopUpdate()
{
    QAction *action = static_cast<QAction *>(sender());
    bool start = action->data().toBool();
    if(!start)
    {
        action->setIcon(QIcon(m_Path+ICON_STOP));
        action->setData(true);
        emit signalStartOrStopUpdate(true);
    }else
    {
        action->setIcon(QIcon(m_Path+ICON_START));
        action->setData(false);
        emit signalStartOrStopUpdate(false);
    }
}

//SearchIpDialog--------------------------------
SearchIpDialog::SearchIpDialog(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle(tr("Search Hosts"));
    this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);

    initDlg();
    this->resize(SearchIP_Dialog_Width,SearchIP_Dialog_height);

    QRect screenRt = QApplication::desktop()->screenGeometry();
    this->move((screenRt.width() - width())/2,(screenRt.height() - height())/2);
}

SearchIpDialog::~SearchIpDialog()
{
    if(m_Nodes.size()>0){
        qDeleteAll(m_Nodes);
        m_Nodes.clear();
    }
}

void SearchIpDialog::closeEvent(QCloseEvent *e)
{
    //如果有扫描线程运行则阻止关闭
    if(m_threads.size()>0){
        e->ignore();
        QMessageBox::warning(this,"warning","Searching is busy,please stop first.");
    }
    else
        QDialog::closeEvent(e);
}


void SearchIpDialog::initDlg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    //loca host ip--------------
    QHBoxLayout *localHostLayout = new QHBoxLayout;
    QLabel *localHostIp = new QLabel(tr("Local Host IP :"));
    m_localHostIp = new QLabel(tr("127.0.0.1"));
    m_localHostIp->setAlignment(Qt::AlignCenter);
    QPushButton *checkLcHostIPBtn = new QPushButton(tr("Check"));
    checkLcHostIP();//初始化IP

    localHostLayout->addWidget(localHostIp);
    localHostLayout->addWidget(m_localHostIp );
    localHostLayout->addWidget(checkLcHostIPBtn,0,Qt::AlignTop);
    localHostLayout->addStretch(1);
    localHostLayout->setSpacing(24);
    mainLayout->addLayout(localHostLayout);

    GLine *line = new  GLine(this);
    line->setMaximumWidth(360);
    mainLayout->addWidget(line);

    //IP Address From -To----------------
    QHBoxLayout *searchFromLayout = new QHBoxLayout;
    QLabel *searchIpFromLbl = new QLabel(tr("Search IP Scope:"));
    m_ipFromEdit = new IpAddrEdit(this);
    QLabel *toLbl = new QLabel("—");
    m_ipToEdit   = new IpAddrEdit(this);
    m_ipFromEdit->setMaximumWidth(180);
    m_ipToEdit->setMaximumWidth(180);
    m_ipFromEdit->setText("192.168.1.2");
    m_ipToEdit->setText("192.168.1.254");

    //
    QLabel   *wtimeLbl = new QLabel("| Max Wait Time(Msec):");
    m_wtimeSpx = new QSpinBox(this);
    m_wtimeSpx->setRange(1,3000);
    m_wtimeSpx->setValue(2);

    //searchFromLayout->addStretch(1);
    searchFromLayout->addWidget(searchIpFromLbl);
    searchFromLayout->addWidget(m_ipFromEdit);
    searchFromLayout->addWidget(toLbl);
    searchFromLayout->addWidget(m_ipToEdit);
    searchFromLayout->addWidget(wtimeLbl);
    searchFromLayout->addWidget(m_wtimeSpx);
    searchFromLayout->addStretch(1);
    searchFromLayout->setSpacing(16);
    mainLayout->addLayout(searchFromLayout);

    //wait time

    //hLine-----
    mainLayout->addWidget(new GLine(this));

    //info----
    QHBoxLayout *infoLayout = new QHBoxLayout;
    QLabel *totalNdLbl = new QLabel("Total Nodes:");
    m_totalNodesLbl = new QLabel("0");
    m_totalNodesLbl->setMinimumWidth(60);
    QLabel *searchedNdLbl = new QLabel("Connected Nodes:");
    m_SearchedNodesLbl = new QLabel("0");
    m_totalNodesLbl->setMinimumWidth(60);
    infoLayout->addWidget(totalNdLbl);
    infoLayout->addWidget(m_totalNodesLbl);
    infoLayout->addWidget(searchedNdLbl);
    infoLayout->addWidget(m_SearchedNodesLbl);
    infoLayout->addStretch(1);
    mainLayout->addLayout(infoLayout);

    //进度条-----------------------------
    QFrame      *progressFr = new QFrame(this);
    progressFr->setFrameShape(QFrame::Box);
    QVBoxLayout *progressVlayout = new QVBoxLayout;
    m_progressBar = new QProgressBar;
    progressVlayout->addWidget(m_progressBar);
    progressVlayout->setContentsMargins(2,2,2,2);

    progressFr->setLayout(progressVlayout);

    mainLayout->addWidget(progressFr);

    //搜索返回主机列表--------------------------
    m_hostsTab = new QTableWidget(this);
    m_hostsTab->setColumnCount(3);
    m_hostsTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList headerlbls;
    headerlbls<<"IP"<<"Device.No"<<"State(Connected)";
    m_hostsTab->setHorizontalHeaderLabels(headerlbls);
    m_hostsTab->setColumnWidth(0,240);
    m_hostsTab->horizontalHeader()->setStretchLastSection(true);
    m_hostsTab->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(m_hostsTab);

    this->setLayout(mainLayout);

    //hLine-----
    mainLayout->addWidget(new GLine(this));

    //buttons---
    QHBoxLayout *btnLayout = new QHBoxLayout;
    m_searchBtn = new QPushButton(tr(SearchIP_Button_Search));
    m_displayBtn = new QPushButton("Init View(Close)");
    m_closeBtn = new QPushButton(tr("Close"));
    btnLayout->addStretch(1);
    btnLayout->addWidget(m_searchBtn);
    btnLayout->addStretch(1);
    btnLayout->addWidget(m_displayBtn);
    btnLayout->addStretch(1);
    btnLayout->addWidget(m_closeBtn);
    btnLayout->addStretch(1);
    mainLayout->addLayout(btnLayout);

    //signals-slots----------------------
    connect(checkLcHostIPBtn,SIGNAL(clicked(bool)),this,SLOT(checkLcHostIP()));
    connect(m_searchBtn,SIGNAL(clicked(bool)),this,SLOT(slotSearchBtnClicked()));
    connect(m_displayBtn,SIGNAL(clicked(bool)),this,SLOT(slotDisplayNodes()));
    connect(m_closeBtn,SIGNAL(clicked(bool)),this,SLOT(close()));
}

void SearchIpDialog::checkLcHostIP()
{
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> address = info.addresses();
    if(address.size()<1)
        return;
    foreach (QHostAddress addr, address) {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol){
            m_localHostIp->setText(addr.toString());
        }
    }
}


void SearchIpDialog::slotSearchBtnClicked()
{
    if(m_searchBtn->text() == SearchIP_Button_Stop)
    {
        //停止搜索
        stopSearch();
        return;
    }
    //检查输入IP范围是否合理
    QString fromIP = m_ipFromEdit->text();
    if(fromIP.isEmpty()){
        QMessageBox::warning(this,"warning","Start IP is wrong.");
        return;
    }
    QString toIP =   m_ipToEdit->text();
    if(toIP.isEmpty()){
        QMessageBox::warning(this,"warning","Last IP is wrong.");
        return;
    }

    QHostAddress addr1(fromIP);
    QHostAddress addr2(toIP);

    //满足 起始< 结束-----
    if(addr1.toIPv4Address()>addr2.toIPv4Address())
    {
        QMessageBox::warning(this,"warning","IP Scope is wrong.");
        return;
    }

    //初始化
    m_progressBar->setValue(0);
    m_searchBtn->setText(SearchIP_Button_Stop);
    m_displayBtn->setEnabled(false);
    m_closeBtn->setEnabled(false);
    m_searchedNodes = 0;

    //表格初始化
    m_hostsTab->clear();
    m_hostsTab->setHorizontalHeaderLabels(QStringList()<<"IP"<<"Device.No"<<"State(Connected)");

    m_startIP =  addr1.toIPv4Address();
    m_lastIP  =  addr2.toIPv4Address();

    m_totalIp = findNodesFromScope(m_startIP,m_lastIP);

    int addrNum = m_totalIp.size();

    m_hostsTab->setRowCount(addrNum);

    for(int i=0;i<addrNum;i++){
        m_hostsTab->setItem(i,0,new QTableWidgetItem(QHostAddress(m_totalIp[i]).toString()));
        m_hostsTab->setItem(i,1
                            ,new QTableWidgetItem(QString("G%1").arg(ip2DeviceNo(m_totalIp[i]),3,10,QChar('0'))));
    }

    m_totalNodesLbl->setText(QString::number(addrNum));
    m_SearchedNodesLbl->setText("0");

    m_progressBar->setRange(0,addrNum);

    searchIpScope(m_totalIp);
}

void SearchIpDialog::slotDisplayNodes()
{
    qDeleteAll(m_Nodes);
    m_Nodes.clear();

    //读取节点下载记录文件，初始化下载状态
    QStringList downloadedGOBS = Project::finishedDownloadGOBS(m_projectInfo);

    //初始化显示节点列表
    int node_size = m_hostsTab->rowCount();
    m_Nodes.resize(node_size);
    bool status;
    int count = 0;
    uint devNo = 0;
    for(int i=0;i<node_size;i++)
    {
        //创建节点信息结构体
        Node *node = new Node(this);
        if(downloadedGOBS.contains(m_hostsTab->item(i,1)->text()))
            node->setDownloaded(true);

        QTableWidgetItem *item = m_hostsTab->item(i,2);
        status = item->data(Node_Role_State).toBool();

        node->setIP(item->data(Node_Role_IP).toUInt());
        node->setState(status);

        devNo = m_hostsTab->item(i,1)->text().mid(1).toUInt();
        node->setDevNo(devNo);

        m_Nodes[count] = node;

        count ++;
    }
    m_Nodes.resize(count);
    this->close();
    emit displayNodes(m_Nodes);
}

void SearchIpDialog::searchIpScope(QVector<uint> &hosts)
{
    int thread_num = QThread::idealThreadCount();
    thread_num = thread_num>hosts.size()?hosts.size():thread_num;

    int wtime = m_wtimeSpx->value();
    uint threadnum = uint(thread_num);
    for(quint32 i =0 ;i<threadnum;i++)
    {
        SearchThread *thread = new SearchThread(this);

        thread->setTotalHosts(m_totalIp);
        TaskCount taskCount;
        taskCount.start = i;
        taskCount.end   = m_totalIp.size()-1;

        taskCount.step = thread_num;

        thread->setTaskCount(taskCount);
        thread->setId(i);
        thread->setWaitTime(wtime);
        connect(thread,SIGNAL(searchedHost(quint32,bool)),this,SLOT(slotSearchedHost(quint32,bool)));
        connect(thread,SIGNAL(searchFinished(int)),this,SLOT(slotThreadFinished(int)));
        connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
        m_threads.insert(i,thread);
        thread->start();
    }
}

void SearchIpDialog::stopSearch()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    foreach(SearchThread *thread,m_threads.values())
        thread->stop();
    while(m_threads.size()>0)
    {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    //搜索结束---------
    m_searchBtn->setText(SearchIP_Button_Search);
    qApp->restoreOverrideCursor();

    m_displayBtn->setEnabled(true);
    m_closeBtn->setEnabled(true);
}

QVector<uint> SearchIpDialog::findNodesFromScope(const quint32 &start,const quint32 &last)
{
    QVector<uint> rst;
    rst.resize(last-start +1);

    int num = 0;
    for(uint i=start;i<=last;i++)
    {
        if(isNodeIP(i)){
            rst[num] = i;
            num++;
        }
    }
    rst.resize(num);
    return rst;
}

void SearchIpDialog::slotSearchedHost(const quint32 &index,const bool &available)
{
    m_progressBar->setValue(m_progressBar->value()+1);
    //行号------
    if(available){
        QTableWidgetItem *item = new QTableWidgetItem(Node_Connected);
        item->setData(Node_Role_IP,m_totalIp[index]);
        item->setData(Node_Role_State,true);
        m_hostsTab->setItem(index,2,item);
    }else
    {
        QTableWidgetItem *item = new QTableWidgetItem(Node_Not_Avaliable);
        item->setData(Node_Role_IP,m_totalIp[index]);
        item->setData(Node_Role_State,false);
        m_hostsTab->setItem(index,2,item);
        return;
    }

    m_searchedNodes ++;
    m_SearchedNodesLbl->setText(QString::number(m_searchedNodes));
}

void SearchIpDialog::slotThreadFinished(const int &id)
{
    //    qDebug()<<"thread:"<<id<<" finished";
    delete m_threads.value(id);
    m_threads.remove(id);

    //搜索结束
    if(m_threads.size() == 0){
        m_searchBtn->setText(SearchIP_Button_Search);
        m_displayBtn->setEnabled(true);
        m_closeBtn->setEnabled(true);
    }
}




//LimitWg---------------------------
LimitWg::LimitWg(const Limit &limit,QWidget *parent)
    :QWidget(parent),m_limit(limit),m_buddyWg(0)
{
    m_defultVal = m_limit.value;
    initWg();
}
void LimitWg::setLimit(const Limit &limit)
{
    m_limit = limit;
    m_defultVal = m_limit.value;
    m_colorFrame->setColor(m_limit.color);
    m_logicLbl->setText(logicText(m_limit.operation));
    m_valEdit->setText(QString::number(m_limit.value));
    m_suffixLbl->setText(m_limit.suffix);
}

void LimitWg::reset()
{
    m_valEdit->setText(QString::number(m_limit.value));
}

Limit LimitWg::limit(bool *ok )
{
    bool ret= false;
    if(ok)
        *ok = true;
    //判断格式
    m_valEdit->text().toFloat(&ret);
    if(!ret) {
        if(ok)
            *ok = ret;
        return m_limit;
    }

    //判断值的范围
    if(m_valEdit->validator())
    {
        const QDoubleValidator *validator = static_cast<const QDoubleValidator*>(m_valEdit->validator());
        if(validator)
        {
            QString text = m_valEdit->text();
            int pos = 0;
            if(validator->validate(text,pos) != QValidator::Acceptable)
            {
                ret = false;
                if(ok)
                    *ok = false;
                return m_limit;
            }
        }
    }

    Limit limit = m_limit;
    limit.value = m_valEdit->text().toFloat();

    return limit;
}

void LimitWg::initWg()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    m_colorFrame = new ColorFrame(this);
    m_colorFrame->setColor(m_limit.color);
    m_logicLbl   = new QLabel(logicText(m_limit.operation));
    m_valEdit    = new QLineEdit(QString::number(m_limit.value));
    m_suffixLbl  = new QLabel(m_limit.suffix);

    mainLayout->addWidget(m_colorFrame);
    mainLayout->addWidget(m_logicLbl);
    mainLayout->addWidget(m_valEdit);
    mainLayout->addWidget(m_suffixLbl);

    this->setLayout(mainLayout);
}

LimitGroupBox::LimitGroupBox(QWidget* parent)
    :QGroupBox(parent)
{
    initLayout();
}

LimitGroupBox::LimitGroupBox(const QString &title, QWidget* parent)
    :QGroupBox(title,parent)
{
    initLayout();
}

void LimitGroupBox::addLimit(const Limit &limit)
{
    LimitWg *limitWg = new LimitWg(limit,this);
    limitWg->setId(m_childWidgets.size());

    m_childWidgets.append(limitWg);
    mainLayout->addWidget(limitWg);
}

QList<Limit> LimitGroupBox::limits(bool *ok)
{
    QList<Limit> lis;
    if(ok)
        *ok  = true;
    bool ret = false;
    foreach(LimitWg *widget, m_childWidgets) {
        lis.append(widget->limit(&ret));
        if(!ret)
        {
            if(ok)
                *ok = ret;
            m_errString = "limit value is out of range"+QString("(%1,%2)")
                    .arg(m_validator->bottom())
                    .arg(m_validator->top());
        }
    }

    //    qDebug()<<lis.at(0).operation;
    //大小顺序是否正确
    if(lis.at(0).operation == Big)
    {
        if(lis.first().value>lis.last().value){
            if(ok)
                *ok = false;
            //数值不对，重置
            lis.clear();
            foreach(LimitWg *widget, m_childWidgets) {
                widget->reset();
                lis.append(widget->defultLimit());
            }
            m_errString = "limit value is error,first is bigger than second value.";
        }
    }
    else
    {
        if(lis.first().value<lis.last().value){
            if(ok)
                *ok = false;
            //数值不对，重置
            lis.clear();
            foreach(LimitWg *widget, m_childWidgets) {
                widget->reset();
                lis.append(widget->defultLimit());
            }
            m_errString = "limit value is error,first is less than second value.";
        }
    }

    return lis;
}

LimitData LimitGroupBox::limitData(bool *ok)
{
    LimitData liData;
    if(ok)
        *ok = true;
    //先获取判断当前 Limit 设置
    bool ret = false;
    QList<Limit> lis = limits(&ret);
    if(!ret)
    {
        if(ok)
            *ok = ret;
    }

    //第一级
    liData.operate = lis.first().operation;
    liData.first = lis.first().value;
    //第二级
    liData.second = lis.last().value;

    return liData;
}

void LimitGroupBox::setValidator(QValidator *validator)
{
    foreach (LimitWg *limitWg, m_childWidgets) {
        limitWg->setValidator(validator);
    }
    m_validator = static_cast<QDoubleValidator*>(validator);
}

void LimitGroupBox::initLayout()
{
    mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(2,2,2,2);
    this->setLayout(mainLayout);
}



//参数状态显示限制编辑对话框-------------------
LimitsDialog::LimitsDialog(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Define Limits");
    initDlg();

    this->resize(600,300);
}

LimitInfo LimitsDialog::getLimitInfo(const int &status)
{
    LimitInfo    limitInfo;
    QList<Limit> limits;
    //显示节点链接状态
    if(status == 0)
    {
        limitInfo.operate = Equal;

        Limit limit;
        limit.color = QColor(Qt::green);
        limit.suffix = "Connected";
        limits.append(limit);

        limit.color = QColor(Qt::gray);
        limit.suffix = "Not Avaliable";
        limits.append(limit);
    }
    else if(status == 5)//自检状态
    {
        limitInfo.operate = Equal;

        Limit limit;
        limit.color = QColor(Qt::green);
        limit.suffix = "Normal";
        limits.append(limit);

        limit.color = QColor(Qt::red);
        limit.suffix = "Warning";
        limits.append(limit);
    }
    else if(status == 6)//下载
    {
        limitInfo.operate = Equal;

        Limit limit;
        limit.color = QColor(Qt::green);//未下载
        limit.suffix = "UnDownload";
        limits.append(limit);

        limit.color = QColor(Qt::blue);//已经下载完成
        limit.suffix = "Downloaded";
        limits.append(limit);
    }
    else{
        bool ok = false;
        limits = m_groupBoxs.at(status-1)->limits(&ok);
        if(!ok){
            QMessageBox::warning(NULL,"warning",m_groupBoxs.at(status-1)->title()
                                 + " limit value is wrong,error:"+m_groupBoxs.at(status-1)->errString());
        }
        limitInfo.operate = limits.at(0).operation;
    }
    limitInfo.limits = limits;
    return limitInfo;
}


//3*3 布局显示
void LimitsDialog::initDlg()
{
    QVBoxLayout *vlayout = new QVBoxLayout;
    QGridLayout  *mainLayout = new QGridLayout;
    Limit limit;
    m_limitData.resize(4);

    //Voltage
    LimitGroupBox *valtageBox = new LimitGroupBox("Voltage",this);
    limit.color     = QColor(Qt::yellow);
    limit.operation = Less;
    limit.value     = 3.7;
    limit.suffix    = "V";
    valtageBox->addLimit(limit);
    limit.color     = QColor(Qt::red);
    limit.operation = Less;
    limit.value     = 3.5;
    limit.suffix    = "V";
    valtageBox->addLimit(limit);
    m_groupBoxs.append(valtageBox);
    mainLayout->addWidget(valtageBox,0,0);
    m_limitData[0].operate = Less;
    m_limitData[0].first   = 3.7;
    m_limitData[0].second  = 3.5;
    QDoubleValidator  *validator = new QDoubleValidator(valtageBox);
    //0～5V
    validator->setRange(0,5,6);
    valtageBox->setValidator(validator);

    //Temperature
    LimitGroupBox *temperBox = new LimitGroupBox("Temperature",this);
    limit.color     = QColor(Qt::yellow);
    limit.operation = Big;
    limit.value     = 30;
    limit.suffix    = "℃";
    temperBox->addLimit(limit);
    limit.color     = QColor(Qt::red);
    limit.operation = Big;
    limit.value     = 50;
    limit.suffix    = "℃";
    temperBox->addLimit(limit);
    m_groupBoxs.append(temperBox);
    mainLayout->addWidget(temperBox,0,1);
    m_limitData[1].operate= Big;
    m_limitData[1].first  = 30;
    m_limitData[1].second = 50;
    validator = new QDoubleValidator(temperBox);
    //0～50 ℃
    validator->setRange(0,50,6);
    temperBox->setValidator(validator);

    //Pressure
    LimitGroupBox *PressureBox = new LimitGroupBox("Pressure",this);
    limit.color     = QColor(Qt::yellow);
    limit.operation = Big;
    limit.value     = 0.90;
    limit.suffix    = "atm";
    PressureBox->addLimit(limit);
    limit.color     = QColor(Qt::red);
    limit.operation = Big;
    limit.value     = 1.0;
    limit.suffix    = "atm";
    PressureBox->addLimit(limit);
    m_groupBoxs.append(PressureBox);
    mainLayout->addWidget(PressureBox,0,2);
    m_limitData[2].operate=Big;
    m_limitData[2].first  = 0.90;
    m_limitData[2].second = 1.0;
    validator = new QDoubleValidator(PressureBox);
    //0～1.0
    validator->setRange(0,1.0,6);
    PressureBox->setValidator(validator);

    /**********取消电流和工作电压****************************/
//    //Current
//    LimitGroupBox *currentBox = new LimitGroupBox("Current",this);
//    limit.color     = QColor(Qt::yellow);
//    limit.operation = Less;
//    limit.value     = 2.0;
//    limit.suffix    = "A";
//    currentBox->addLimit(limit);
//    limit.color     = QColor(Qt::red);
//    limit.operation = Less;
//    limit.value     = 1.0;
//    limit.suffix    = "A";
//    currentBox->addLimit(limit);
//    m_groupBoxs.append(currentBox);
//    mainLayout->addWidget(currentBox,1,0);
//    m_limitData[3].operate=Less;
//    m_limitData[3].first  = 2.0;
//    m_limitData[3].second = 1.0;
//    validator = new QDoubleValidator(currentBox);
//    //0～5.0
//    validator->setRange(0.0,5.0,6);
//    currentBox->setValidator(validator);

    //Charge Voltage
//    LimitGroupBox *chargeVolBox = new LimitGroupBox("Charge Voltage",this);
//    limit.color     = QColor(Qt::yellow);
//    limit.operation = Less;
//    limit.value     = 2.0;
//    limit.suffix    = "V";
//    chargeVolBox->addLimit(limit);
//    limit.color     = QColor(Qt::red);
//    limit.operation = Less;
//    limit.value     = 1;
//    limit.suffix    = "V";
//    chargeVolBox->addLimit(limit);
//    m_groupBoxs.append(chargeVolBox);
//    mainLayout->addWidget(chargeVolBox,1,1);
//    m_limitData[4].operate=Less;
//    m_limitData[4].first  = 2.0;
//    m_limitData[4].second = 1.0;
//    validator = new QDoubleValidator(chargeVolBox);
//    //0～5.0 V
//    validator->setRange(0.0,12.0,6);
//    chargeVolBox->setValidator(validator);

    //Memory Capacity
    LimitGroupBox *memoryBox = new LimitGroupBox("Memory Capacity",this);
    limit.color     = QColor(Qt::yellow);
    limit.operation = Less;
    limit.value     = 5;
    limit.suffix    = "GB";
    memoryBox->addLimit(limit);
    limit.color     = QColor(Qt::red);
    limit.operation = Less;
    limit.value     = 1;
    limit.suffix    = "GB";
    memoryBox->addLimit(limit);
    m_groupBoxs.append(memoryBox);
    m_limitData[3].operate=Less;
    m_limitData[3].first  = 5;
    m_limitData[3].second = 1;
    validator = new QDoubleValidator(memoryBox);
    //0～32.0 G
    validator->setRange(0.0,32.0,6);
    memoryBox->setValidator(validator);

    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(memoryBox,1,1);

    vlayout->addLayout(mainLayout);

    vlayout->addWidget(new GLine(this));

    QHBoxLayout  *btnLayout = new QHBoxLayout;

    QPushButton   *cancelBtn = new QPushButton(tr("&Cancel"));
    QPushButton   *okBtn     = new QPushButton(tr("&Ok"));
    btnLayout->addStretch(1);
    okBtn->setDefault(true);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);

    vlayout->addLayout(btnLayout);
    this->setLayout(vlayout);

    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(cancelBtnClicked()));
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(okBtnClicked()));
}

void LimitsDialog::cancelBtnClicked()
{
    this->close();
}

void LimitsDialog::okBtnClicked()
{
    bool ok = false;
    //更新阀值数组
    for(int i = 0;i<m_groupBoxs.size();i++)
    {
        m_limitData[i] = m_groupBoxs[i]->limitData(&ok);
        if(!ok){
            QMessageBox::warning(this,"warning",m_groupBoxs[i]->title()+
                                 " limit data is wrong,error:"+m_groupBoxs[i]->errString());
            return;
        }
    }

    //发送更新
    emit limitsChanged();
    emit limitsChanged(m_limitData);
    this->close();
}




