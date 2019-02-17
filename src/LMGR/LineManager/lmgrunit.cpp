#include "lmgrunit.h"
#include "linemanager.h"


#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QRadioButton>

#include <QMessageBox>


LmgrUnit::LmgrUnit()
{

}

//---------------------------------------
StationsInfoWg::StationsInfoWg(QWidget *parent)
    :QWidget(parent)
{

    initWg();
}

void StationsInfoWg::initWg()
{
    QFont font;
    int lbl_min_w = font.pointSize()*7;

    QHBoxLayout *infoLayout = new QHBoxLayout;
    QLabel *shotsNameLbl = new QLabel("Shots:");
    m_shotsLbl = new QLabel("");
    m_shotsLbl->setMinimumWidth(lbl_min_w);
    infoLayout->addWidget(shotsNameLbl);
    infoLayout->addWidget(m_shotsLbl);

    QLabel *recvsNameLbl = new QLabel("Nodes:");
    m_recvLbl = new QLabel("");
    m_recvLbl->setMinimumWidth(lbl_min_w);
    infoLayout->addWidget(recvsNameLbl);
    infoLayout->addWidget(m_recvLbl);

    QLabel *deployedNameLbl = new QLabel("Deployed(Recvs):");
    m_deployedLbl = new QLabel("");
    m_deployedLbl->setMinimumWidth(lbl_min_w);
    infoLayout->addWidget(deployedNameLbl);
    infoLayout->addWidget(m_deployedLbl);

    QLabel *pickedupNameLbl = new QLabel("Picked Up(Recvs):");
    m_pickedLbl = new QLabel("");
    m_pickedLbl->setMinimumWidth(lbl_min_w);
    infoLayout->addWidget(pickedupNameLbl);
    infoLayout->addWidget(m_pickedLbl);

    infoLayout->addStretch(1);
    infoLayout->setContentsMargins(6,4,6,4);

    this->setLayout(infoLayout);
}


void StationsInfoWg::setStationNum(const int &shots,const int recvs)
{
    m_shotsLbl->setText(QString::number(shots));
    m_recvLbl->setText(QString::number(recvs));
}


void StationsInfoWg::setRNInfoNum(const int &depolyed,const int &pickedUp)
{
    m_deployedLbl->setText(QString::number(depolyed));
    m_pickedLbl->setText(QString::number(pickedUp));
}

void StationsInfoWg::clearData()
{
    m_shotsLbl->clear();
    m_recvLbl->clear();
    m_deployedLbl->clear();
    m_pickedLbl->clear();
}


//-------------------------------
void MenuManager::createMenu(QMenuBar *menuBar,LineManager *lineManager)
{
    menuFile = new QMenu("File");
    menuFile->addAction(tr("&New Area..."),lineManager,SLOT(slotNewArea()),QKeySequence(tr("Ctrl+N")));
    menuFile->addAction(tr("&Open Area..."),lineManager,SLOT(slotOpenArea()),QKeySequence(tr("Ctrl+O")));
    menuFile->addAction(tr("&Update Current Area(SPS)..."),lineManager,SLOT(slotUpdateAreaSPS()));
    //刷新工区
    menuFile->addAction(tr("Refresh Current Area"),lineManager,SLOT(slotUpdateCurrent()));
    menuFile->addAction("Close Current Area",lineManager,SLOT(slotCloseCurrent()),QKeySequence(tr("Ctrl+W")));
    menuFile->addSeparator();
    menuFile->addAction("Delete Area...",lineManager,SLOT(slotDeleteArea()));
    menuFile->addSeparator();
    menuFile->addAction(tr("E&xit"),lineManager,SLOT(slotExit()),QKeySequence(tr("Ctrl+Q")));
    menuBar->addMenu(menuFile);

    //Edit-----------------
    menuEdit = new QMenu("Edit");
    menuEdit->addAction("RN Deployments...",lineManager,SLOT(slotRNDeployments()));
    menuEdit->addAction("Assign RNs to Stations...",lineManager,SLOT(slotAssignRNDeployments()));
    menuEdit->addSeparator();
    menuEdit->addAction("Configure Deployments System...",lineManager,SLOT(slotConfigDeploySys()));

    dis_con_DpySysAct = new QAction("Connect/Dis Deployments System",menuEdit);
    dis_con_DpySysAct->setCheckable(true);
    connect(dis_con_DpySysAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotDis_ConDeploySys(bool)));
    menuEdit->addAction(dis_con_DpySysAct);
    menuBar->addMenu(menuEdit);

    //View-----------------
    menuView = new QMenu("View");
    //Recv - Shots
    QAction *showRecvAct = new QAction("Show Recievers",menuView);
    showRecvAct->setCheckable(true);
    showRecvAct->setChecked(true);
    connect(showRecvAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotShowRecvs(bool)));
    menuView->addAction(showRecvAct);

    QAction *showShotsAct = new QAction("Show Shots",menuView);
    showShotsAct->setCheckable(true);
    showShotsAct->setChecked(true);
    connect(showShotsAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotShowShots(bool)));
    menuView->addAction(showShotsAct);
    menuView->addSeparator();

    //Recv - Shot lines
    QAction *showRecvlsAct = new QAction("Show Reciever Lines",menuView);
    showRecvlsAct->setCheckable(true);
    connect(showRecvlsAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotShowRecvLines(bool)));
    menuView->addAction(showRecvlsAct);

    QAction *showShotlsAct = new QAction("Show Shot Lines",menuView);
    showShotlsAct->setCheckable(true);
    connect(showShotlsAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotShowShotLines(bool)));
    menuView->addAction(showShotlsAct);
    menuView->addSeparator();

    //Shot - Recv Text
    QAction *showRecvTxtAct = new QAction("Show Reciever Text",menuView);
    showRecvTxtAct->setCheckable(true);
    connect(showRecvTxtAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotShowRecvText(bool)));
    menuView->addAction(showRecvTxtAct);

    QAction *showShotTxtAct = new QAction("Show Shot Text",menuView);
    showShotTxtAct->setCheckable(true);
    connect(showShotTxtAct,SIGNAL(triggered(bool)),lineManager,SLOT(slotShowShotText(bool)));
    menuView->addAction(showShotTxtAct);

    menuView->addSeparator();

    //Preference
    menuView->addAction("Preference...",lineManager,SLOT(slotPreference()));

    menuBar->addMenu(menuView);

    //Help
    QMenu *menuHelp = new QMenu("Help");
    menuHelp->addAction("About...",lineManager,SLOT(slotAbout()),QKeySequence(tr("Ctrl+H")));
    menuBar->addMenu(menuHelp);

    //初始化菜单状态
    updateMenuStatus(false);
}

void MenuManager::updateMenuStatus(const bool &enable)
{
    if(m_enable == enable)
        return;
    m_enable = enable;
    //初始化菜单状态
    //File
    QList<QAction*> fileActoins = menuFile->actions();
    fileActoins.at(2)->setEnabled(enable);
    fileActoins.at(3)->setEnabled(enable);//update
    fileActoins.at(4)->setEnabled(enable);//close current

    //Edit
    QList<QAction*> editActoins = menuEdit->actions();
    editActoins.at(0)->setEnabled(enable);//RN Deployments...
    //    editActoins.at(1)->setEnabled(enable);//Assign RN to Stations
    //    editActoins.at(3)->setEnabled(enable);//Configure DeploySys
    //    editActoins.at(4)->setEnabled(enable);//Con/Disconnect DeploySys

    //View
    QList<QAction*> viewActoins = menuView->actions();
    viewActoins.at(0)->setEnabled(enable);//Show Recvs
    viewActoins.at(1)->setEnabled(enable);//Show Shots
    //separator--
    viewActoins.at(3)->setEnabled(enable);//Show Recv Lines
    viewActoins.at(4)->setEnabled(enable);//Show Shots Lines
}

void MenuManager::setDeploySysConnected(const bool &connected)
{
    dis_con_DpySysAct->setChecked(connected);
}

//---------------------------------
void ToolManager::createToolBar(QToolBar *toolBar,LineManager *lineManager)
{
    //文件目录
    QString    m_Path = Core::rootPath();
    m_toolBar = toolBar;

    //Open Area
    toolBar->addAction(QIcon(m_Path+ICON_NEW_FILE),"New Area",lineManager,SLOT(slotNewArea()));
    toolBar->addAction(QIcon(m_Path+ICON_OPEN_FILE),"Open Area",lineManager,SLOT(slotOpenArea()));
    toolBar->addAction(QIcon(m_Path+ICON_CLOSE),"Close Area",lineManager,SLOT(slotCloseCurrent()));
    toolBar->addSeparator();

    toolBar->addAction(QIcon(m_Path+ICON_DELETE),"Delete Area",lineManager,SLOT(slotDeleteArea()));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(m_Path+ICON_UPDATE),"Update Current Area",lineManager,SLOT(slotUpdateCurrent()));
    toolBar->addSeparator();

    toolBar->addAction(QIcon(m_Path+ICON_ZOOM_IN),"Zoom In(5%)",lineManager,SLOT(slotZoomIn()));
    toolBar->addAction(QIcon(m_Path+ICON_ZOOM_OUT),"Zoom Out(5%)",lineManager,SLOT(slotZoomOut()));
    toolBar->addAction(QIcon(m_Path+ICON_ZOOM_IN_FULL),"Zoom In Fully",lineManager,SLOT(slotZoomInFull()));
    toolBar->addAction(QIcon(m_Path+ICON_ZOOM_OUT_FULL),"Zoom Out Fully",lineManager,SLOT(slotZoomOutFull()));
    toolBar->addSeparator();

    toolBar->addAction(QIcon(m_Path+ICON_ITEM_LIST),"Show RN Deployments",lineManager,SLOT(slotRNDeployments()));

    //初始化工具栏状态
    updateToolBarStatus(false);
}

void ToolManager::updateToolBarStatus(const bool &enable)
{
    if(m_enable == enable)
        return;
    m_enable = enable;
    QList<QAction *> toolActions = m_toolBar->actions();

    toolActions.at(2)->setEnabled(enable); //Close
    toolActions.at(6)->setEnabled(enable); //Update
    toolActions.at(13)->setEnabled(enable); //RN Deployments
}


//-------------------------------------------
AbstractAreaDialog::AbstractAreaDialog(QWidget *parent)
    :QDialog(parent),m_AreaDatainfo(0),first_choose(true)
{
    this->setWindowTitle("Create Area");
    createBaseDlg();

    m_updateOpt = ReplaceSps;
    this->resize(QSize(540,290));
}

AbstractAreaDialog::~AbstractAreaDialog()
{
    if(m_AreaDatainfo)
        delete m_AreaDatainfo;
}

void AbstractAreaDialog::setAreaOption(AreaOption option)
{
    m_areaOpt = option;
    if(option == CreateArea)
        m_updateGrpBox->hide();
    else
        m_updateGrpBox->show();
}
void AbstractAreaDialog::setProgressValue(const int &value)
{
    m_progressBar->setValue(value);
}
void AbstractAreaDialog::setOperateHint(const QString &hint)
{
    qDebug()<<"set hint:"<<hint;
    m_progressHintLbl->setText(hint);
}

void AbstractAreaDialog::setOptBtnText(const QString &text)
{
    m_operateBtn->setText(text);
}

void AbstractAreaDialog::setAreaEditReadOnly(const bool &readOnly)
{
    m_areaNameEdt->setReadOnly(readOnly);
}

void AbstractAreaDialog::setAreaName(const QString &areaName)
{
    m_areaNameEdt->setText(areaName);
}

void AbstractAreaDialog::createBaseDlg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    //Area name-----------------------
    QHBoxLayout  *areaNameLayout = new QHBoxLayout;
    QLabel    *areaNameLbl = new QLabel("Area Name:");
    m_areaNameEdt = new QLineEdit;
    m_areaNameEdt->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z_]+[a-zA-Z0-9_]+")));
    areaNameLayout->addWidget(areaNameLbl);
    areaNameLayout->addWidget(m_areaNameEdt);
    mainLayout->addLayout(areaNameLayout);

    //Select SPS Files---------------------------------------------
    QGroupBox *spsGrpBox = new QGroupBox("Select SPS Files");
    QGridLayout *grpLayout = new QGridLayout;
    grpLayout->setContentsMargins(0,4,0,4);
    grpLayout->setColumnStretch(1,1);

    QLabel    *RfileLbl = new QLabel("R File:");
    m_RfileEdt = new QLineEdit;
    QPushButton *RbrwBtn = new QPushButton("...");
    RbrwBtn->setProperty("type","R");
    RbrwBtn->setMaximumWidth(36);
    grpLayout->addWidget(RfileLbl,0,0);
    grpLayout->addWidget(m_RfileEdt,0,1);
    grpLayout->addWidget(RbrwBtn,0,2);

    QLabel *SfileLbl = new QLabel("S File:");
    m_SfileEdt = new QLineEdit;
    QPushButton *SbrwBtn = new QPushButton("...");
    SbrwBtn->setProperty("type","S");
    SbrwBtn->setMaximumWidth(36);
    grpLayout->addWidget(SfileLbl,1,0);
    grpLayout->addWidget(m_SfileEdt,1,1);
    grpLayout->addWidget(SbrwBtn,1,2);

    QLabel *XfileLbl = new QLabel("X File:");
    m_XfileEdt = new QLineEdit;
    QPushButton *XbrwBtn = new QPushButton("...");
    XbrwBtn->setProperty("type","X");
    XbrwBtn->setMaximumWidth(36);
    grpLayout->addWidget(XfileLbl,2,0);
    grpLayout->addWidget(m_XfileEdt,2,1);
    grpLayout->addWidget(XbrwBtn,2,2);

    m_autoFileDectCbx = new QCheckBox("Auto File Detect",this);
    m_autoFileDectCbx->setChecked(true);
    grpLayout->addWidget(m_autoFileDectCbx,3,0,1,3,Qt::AlignLeft);

    spsGrpBox->setLayout(grpLayout);
    mainLayout->addWidget(spsGrpBox);

    m_updateGrpBox = new QGroupBox("Update Option",this);
    QHBoxLayout  *cheBoxLayout = new QHBoxLayout;

    QRadioButton *replaceRbx = new QRadioButton("Replace(Need S,R Files)");
    cheBoxLayout->addWidget(replaceRbx);
    replaceRbx->setChecked(true);

    QRadioButton *mergeRbx = new QRadioButton("Merge(Need S File)");
    cheBoxLayout->addWidget(mergeRbx);
    m_updateGrpBox->setLayout(cheBoxLayout);
    mainLayout->addWidget(m_updateGrpBox);
    m_updateGrpBox->hide();

    //ProgressBar --------------------------
    QFrame      *progressFr = new QFrame(this);
    QVBoxLayout *progressVlayout = new QVBoxLayout;
    m_progressHintLbl = new QLabel("                           ");
    //m_progressHintLbl->setMinimumWidth(120);
    m_progressHintLbl->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    progressVlayout->addWidget(m_progressHintLbl,1,Qt::AlignHCenter);
    m_progressBar = new QProgressBar;
    progressVlayout->addWidget(m_progressBar);
    progressVlayout->setContentsMargins(2,4,2,0);

    progressFr->setLayout(progressVlayout);
    progressFr->setFrameShape(QFrame::Box);

    mainLayout->addWidget(progressFr);

    //分割线------------------------------------
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    //buttons-------------------------------
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch(1);
    m_operateBtn = new QPushButton("operate");
    btnLayout->addWidget(m_operateBtn);
    btnLayout->addStretch(1);
    QPushButton *cancelBtn = new QPushButton("Cancel");
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch(1);
    QPushButton *helpBtn = new QPushButton("Help");
    btnLayout->addWidget(helpBtn);
    btnLayout->addStretch(1);

    mainLayout->addLayout(btnLayout);

    this->setLayout(mainLayout);

    //signal - slots------------------------------------------
    connect(RbrwBtn,SIGNAL(clicked(bool)),this,SLOT(slotBrowserSPSFile()));
    connect(SbrwBtn,SIGNAL(clicked(bool)),this,SLOT(slotBrowserSPSFile()));
    connect(XbrwBtn,SIGNAL(clicked(bool)),this,SLOT(slotBrowserSPSFile()));

    connect(m_operateBtn,SIGNAL(clicked(bool)),this,SLOT(slotOperate()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(helpBtn,SIGNAL(clicked(bool)),this,SLOT(slotHelp()));

    connect(replaceRbx,SIGNAL(clicked(bool)),this,SLOT(slotReplaceUpdate()));
    connect(mergeRbx,SIGNAL(clicked(bool)),this,SLOT(slotMergeUpdate()));
}

void AbstractAreaDialog::slotBrowserSPSFile()
{
    QPushButton  *brwsBtn = static_cast<QPushButton *>(sender());
    QString brwFileType = brwsBtn->property("type").toString();

    QString Path;
    if(first_choose)
        Path = Core::homePath();
    //(*.R* *.r*)格式
    QString filter = "SPS "+brwFileType+" Files(*."+brwFileType+"* *."+brwFileType.toLower()+"*)";

    QString fileName = QFileDialog::getOpenFileName(this,"Select SPS "+brwFileType+" File",Path,filter);
    if(fileName.isEmpty())
        return;
    first_choose = false;
    if(brwFileType == "R")
    {
        m_RfileEdt->setText(fileName);
    }
    if(brwFileType == "S")
    {
        m_SfileEdt->setText(fileName);
    }
    if(brwFileType == "X")
    {
        m_XfileEdt->setText(fileName);
    }
    if(m_autoFileDectCbx->isChecked())
        autoDetectFile(brwFileType);
}

void AbstractAreaDialog::autoDetectFile(const QString &type)
{
    //确定输入的文件
    QString inputFile;
    if(type == "R")
        inputFile = rFileName();
    if(type == "S")
        inputFile = sFileName();
    if(type == "X")
        inputFile = xFileName();

    //R file 查找-----------------------
    int   dotIndex= inputFile.lastIndexOf(".");
    QChar typeChar = inputFile.at(dotIndex+1);
    if(type != "R")
    {
        QString rstFile = inputFile;
        if(typeChar.isLower())
            rstFile.replace(dotIndex+1,1,'r');
        else
            rstFile.replace(dotIndex+1,1,'R');
        QFile file(rstFile);
        if(file.exists())
            m_RfileEdt->setText(rstFile);
    }

    //S file 查找-------------
    if(type != "S")
    {
        QString rstFile = inputFile;
        if(typeChar.isLower())
            rstFile.replace(dotIndex+1,1,'s');
        else
            rstFile.replace(dotIndex+1,1,'S');
        QFile file(rstFile);
        if(file.exists())
            m_SfileEdt->setText(rstFile);
    }

    //X file 查找-------------
    if(type != "X")
    {
        QString rstFile = inputFile;
        if(typeChar.isLower())
            rstFile.replace(dotIndex+1,1,'x');
        else
            rstFile.replace(dotIndex+1,1,'X');
        QFile file(rstFile);
        if(file.exists())
            m_XfileEdt->setText(rstFile);
    }
}

void AbstractAreaDialog::slotOperate()
{
    m_spsFiles.clear();
    //1 判断输入参数是否正确
    QString area_name = areaName();
    if(area_name.isEmpty())
    {
        QMessageBox::warning(this,"warning","Please input area name!");
        return;
    }

    QString fileName  = rFileName();
    //Merge更新可以没有R文件
    if(!(m_areaOpt == UpdateArea && m_updateOpt == MergeSps)){

        if(fileName.isEmpty())
        {
            QMessageBox::warning(this,"warning","Please input R file!");
            return;
        }
    }
    m_spsFiles.append(fileName);

    fileName = sFileName();
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"warning","Please input S file!");
        return;
    }
    m_spsFiles.append(fileName);

    //x 文件可选
    fileName = xFileName();
    //    if(fileName.isEmpty())
    //    {
    //        QMessageBox::warning(this,"warning","Please input X file!");
    //        return;
    //    }
    m_spsFiles.append(fileName);

    //2 执行操作run
    run();
}


void AbstractAreaDialog::slotReplaceUpdate()
{
    m_updateOpt = ReplaceSps;
}

void AbstractAreaDialog::slotMergeUpdate()
{
    m_updateOpt = MergeSps;
}

//Create Area------------------------------------------
CreateAreaDialog::CreateAreaDialog(QWidget *parent)
    :AbstractAreaDialog(parent)
{
    setOptBtnText("Create");
    setAreaOption(CreateArea);
}

void CreateAreaDialog::run()
{
    //判断工区名是否合法,是否已经存在等
    QString area_name = areaName();
    int index = area_name.indexOf(QRegExp(ILLEGAL_CHARS));
    if(index>=0)
    {
        QMessageBox::warning(this,"warning"
                             ,QString("Area name contains invalid char \"%1\"").arg(area_name.at(index)));
        return;
    }

    QString areaFile = m_projInfo.ProjectPath+Dir_Separator+area_name;

    QFileInfo info(areaFile);
    //已经存在文件或者目录
    if(info.exists())
    {
        //是文件，直接删除
        if(info.isFile())
        {
            QFile file(info.absoluteFilePath());
            file.remove();
        }
        //目录，提示是否覆盖
        if(info.isDir())
        {
            QMessageBox box(QMessageBox::Warning,tr("warning")
                            ,tr("The dir has been existed, do you wan to overwrite it?")
                            ,QMessageBox::Yes |QMessageBox::No,this);
            switch(box.exec())
            {
            case QMessageBox::Yes:
                //先删除该工区
                emit signalDeleteArea(area_name);
                break;
            case QMessageBox::No:
                return;
            }
        }
    }

    //发送信号给主界面------
    emit signalCreateArea(areaName());
}

void CreateAreaDialog::slotHelp()
{
    QMessageBox::information(this,"Help Information","To create a new area,"
                                                     "Input a areaName,and select sps files, and S,R files are"
                                                     "needed, X file is optional.");
}


//Update Area------------------------------------------
UpdateAreaDialog::UpdateAreaDialog(QWidget *parent)
    :AbstractAreaDialog(parent)
{
    this->setWindowTitle("Update Current Area");
    setOptBtnText("Update");

    setAreaEditReadOnly(true);
    setAreaOption(UpdateArea);
}
void UpdateAreaDialog::updateArea(const QString &areaName)
{
    setAreaName(areaName);
    this->exec();
}

void UpdateAreaDialog::run()
{
    //判断sps 文件

    //发送信号给主界面------
    emit signalUpdateArea();
}

void UpdateAreaDialog::slotHelp()
{
    QMessageBox::information(this,"Help Information","To update the current area with new sps files,"
                                                     ",please select sps files, and S,R files are"
                                                     "needed, X file is optional.");
}

/*AbstractAreaList------------------------------*/
AreaListDlg::AreaListDlg(QWidget *parent)
    :QDialog(parent),m_areaOpt(Open_Area)
{
    this->setWindowFlags(Qt::Dialog);

    initDlg();

    if(m_areaOpt == Open_Area)
        this->setWindowTitle("Select Existing Area to open");
    else if(m_areaOpt == Delete_Area)
        this->setWindowTitle("Select Existing Area to delete");
}

void AreaListDlg::setAreaOpt(const AreaOpt &opt)
{
    m_areaOpt = opt;
    if(m_areaOpt == Open_Area)
        this->setWindowTitle("Select Existing Area to open");
    else if(m_areaOpt == Delete_Area)
        this->setWindowTitle("Select Existing Area to delete");
}

AreaListDlg::~AreaListDlg()
{

}

void AreaListDlg::initDlg()
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    QHBoxLayout  *lblLayout = new QHBoxLayout;

    QLabel       *areaname_lbl= new QLabel("Area Name:");
    lblLayout->addWidget(areaname_lbl);
    lblLayout->addStretch(1);
    vLayout->addLayout(lblLayout);

    m_listWidget = new QListWidget(this);
    m_listWidget->setWindowTitle("area Names");
    vLayout->addWidget(m_listWidget);

    QHBoxLayout *hLayout = new QHBoxLayout;
    QPushButton *okBtn = new QPushButton("Ok");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    hLayout->addStretch(1);
    hLayout->addWidget(okBtn);
    hLayout->addWidget(cancelBtn);

    vLayout->addLayout(hLayout);

    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(slotOk()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(m_listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(slotOk()));
    this->setLayout(vLayout);
    this->resize(300,400);
}

void AreaListDlg::updateAreaList()
{
    //项目文件--------------------------
    QString projFile = m_projInfo.ProjectPath + Dir_Separator + m_projInfo.ProjectName+Project_File_Suffix;
    QStringList  areaList= Area::areasFromProject(projFile);

    m_listWidget->clear();
    for(int i=0;i<areaList.size();i++)
        m_listWidget->addItem(new QListWidgetItem(areaList.at(i)));
}

void AreaListDlg::showAreaList()
{
    //项目文件--
    QString projFile = m_projInfo.ProjectPath + Dir_Separator + m_projInfo.ProjectName+Project_File_Suffix;
    QStringList  areaList= Area::areasFromProject(projFile);

    m_listWidget->clear();
    for(int i=0;i<areaList.size();i++)
        m_listWidget->addItem(new QListWidgetItem(areaList.at(i)));

    this->exec();
}


void AreaListDlg::slotOk()
{
    QListWidgetItem *item = m_listWidget->currentItem();
    if(!item)
        return;
    QString areaName = item->text();

    if(m_areaOpt == Open_Area){
        emit signalOpenArea(areaName);
        this->close();
    }
    else if(m_areaOpt == Delete_Area){
        emit signalDeleteArea(areaName);
    }
}

//----------------------------------------------------
PreferenceDlg::PreferenceDlg(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Color Preference");
    initDlg();
}

void PreferenceDlg::initDlg()
{
    m_btnGroup   = new QButtonGroup(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QFrame      *buttonsFr = new QFrame(this);
    buttonsFr->setFrameShape(QFrame::Box);
    QGridLayout *buttonsLayout = new QGridLayout;

    //backGroud----
    QLabel       *bkColorLbl = new QLabel("BackGroud Color",this);
    m_bkColorBtn = new ColorPickButton(this);
    m_bkColorBtn->setColor(Qt::black);
    m_bkColorBtn->setId(Backdroud_Color);
    buttonsLayout->addWidget(bkColorLbl,0,0);
    buttonsLayout->addWidget(m_bkColorBtn,0,1);
    m_colorMap.insert(Backdroud_Color,Qt::black);
    m_btnGroup->addButton(m_bkColorBtn,Backdroud_Color);


    //Shot
    QLabel *shotColorLbl = new QLabel("Shot Color",this);
    m_shotColorBtn = new ColorPickButton(this);
    m_shotColorBtn->setColor(Qt::red);
    m_shotColorBtn->setId(Shot_Color);
    buttonsLayout->addWidget(shotColorLbl,1,0);
    buttonsLayout->addWidget(m_shotColorBtn,1,1);
    m_colorMap.insert(Shot_Color,Qt::red);
    m_btnGroup->addButton(m_shotColorBtn,Shot_Color);

    //Fired Shot
    QLabel *firedShotColorLbl = new QLabel("Fired Shot Color",this);
    m_firedShotColorBtn = new ColorPickButton(this);
    m_firedShotColorBtn->setColor(Qt::cyan);
    m_firedShotColorBtn->setId(Fired_Shot_Color);
    buttonsLayout->addWidget(firedShotColorLbl,2,0);
    buttonsLayout->addWidget(m_firedShotColorBtn,2,1);
    m_colorMap.insert(Fired_Shot_Color,Qt::cyan);
    m_btnGroup->addButton(m_firedShotColorBtn,Fired_Shot_Color);

    //recv
    QLabel *recvColorLbl = new QLabel("Reciever Color",this);
    m_recvColorBtn = new ColorPickButton(this);
    m_recvColorBtn->setColor(Qt::white);
    m_recvColorBtn->setId(Recv_Color);
    buttonsLayout->addWidget(recvColorLbl,3,0);
    buttonsLayout->addWidget(m_recvColorBtn,3,1);
    m_colorMap.insert(Recv_Color,Qt::white);
    m_btnGroup->addButton(m_recvColorBtn,Recv_Color);

    //shotLine
    QLabel *shotLineColorLbl = new QLabel("Shot Line Color",this);
    m_shotLineColorBtn = new ColorPickButton(this);
    m_shotLineColorBtn->setColor(Qt::cyan);
    m_shotLineColorBtn->setId(Shot_Line_Color);
    buttonsLayout->addWidget(shotLineColorLbl,4,0);
    buttonsLayout->addWidget(m_shotLineColorBtn,4,1);
    m_colorMap.insert(Shot_Line_Color,Qt::cyan);
    m_btnGroup->addButton(m_shotLineColorBtn,Shot_Line_Color);
    //recvLine
    QLabel *recvLineColorLbl = new QLabel("Reciever Line Color",this);
    m_recvLineColorBtn = new ColorPickButton(this);
    m_recvLineColorBtn->setColor(Qt::magenta);
    m_recvLineColorBtn->setId(Recv_Line_Color);
    buttonsLayout->addWidget(recvLineColorLbl,5,0);
    buttonsLayout->addWidget(m_recvLineColorBtn,5,1);
    m_colorMap.insert(Recv_Line_Color,Qt::magenta);
    m_btnGroup->addButton(m_recvLineColorBtn,Recv_Line_Color);
    //Legend
    QLabel *legendColorLbl = new QLabel("Legend Color",this);
    m_legendColorBtn = new ColorPickButton(this);
    m_legendColorBtn->setColor(Qt::white);
    m_legendColorBtn->setId(Legend_Color);
    buttonsLayout->addWidget(legendColorLbl,6,0);
    buttonsLayout->addWidget(m_legendColorBtn,6,1);
    m_colorMap.insert(Legend_Color,Qt::white);
    m_btnGroup->addButton(m_legendColorBtn,Legend_Color);
    //Deployed Station
    QLabel *deployedColorLbl = new QLabel("Deployed Reciever Color",this);
    m_deployedColorBtn = new ColorPickButton(this);
    m_deployedColorBtn->setColor(Qt::yellow);
    m_deployedColorBtn->setId(Deployed_Recv_Color);
    buttonsLayout->addWidget(deployedColorLbl,7,0);
    buttonsLayout->addWidget(m_deployedColorBtn,7,1);
    m_colorMap.insert(Deployed_Recv_Color,Qt::yellow);
    m_btnGroup->addButton(m_deployedColorBtn,Deployed_Recv_Color);
    //Picked Up
    QLabel *pickedColorLbl = new QLabel("Picked Up Reciever Color          ",this);
    m_pickedColorBtn = new ColorPickButton(this);
    m_pickedColorBtn->setColor(Qt::blue);
    m_pickedColorBtn->setId(PickedUp_Recv_Color);
    buttonsLayout->addWidget(pickedColorLbl,8,0);
    buttonsLayout->addWidget(m_pickedColorBtn,8,1);
    m_colorMap.insert(PickedUp_Recv_Color,Qt::blue);
    m_btnGroup->addButton(m_pickedColorBtn,PickedUp_Recv_Color);

    //Download gobs
    QLabel *downloadedColorLbl = new QLabel("Downloaded Reciever Color          ",this);
    m_downloadedColorBtn = new ColorPickButton(this);
    m_downloadedColorBtn->setColor(Qt::green);
    m_downloadedColorBtn->setId(Downloaded_Recv_Color);
    buttonsLayout->addWidget(downloadedColorLbl,9,0);
    buttonsLayout->addWidget(m_downloadedColorBtn,9,1);
    m_colorMap.insert(Downloaded_Recv_Color,Qt::green);
    m_btnGroup->addButton(m_downloadedColorBtn,Downloaded_Recv_Color);

    //shot Station Text
    QLabel *shotTextColorLbl = new QLabel("Shot Text Color",this);
    m_shotTextColorBtn = new ColorPickButton(this);
    m_shotTextColorBtn->setColor(QColor(255,153,0));
    m_shotTextColorBtn->setId(Shot_Text_Color);
    buttonsLayout->addWidget(shotTextColorLbl,10,0);
    buttonsLayout->addWidget(m_shotTextColorBtn,10,1);
    m_colorMap.insert(Shot_Text_Color,QColor(255,153,0));
    m_btnGroup->addButton(m_pickedColorBtn,Shot_Text_Color);

    //Reciever Station Text
    QLabel *recvTextColorLbl = new QLabel("Shot Text Color",this);
    m_recvTextColorBtn = new ColorPickButton(this);
    m_recvTextColorBtn->setColor(QColor(0,255,51));
    m_recvTextColorBtn->setId(Recv_Text_Color);
    buttonsLayout->addWidget(recvTextColorLbl,11,0);
    buttonsLayout->addWidget(m_recvTextColorBtn,11,1);
    m_colorMap.insert(Recv_Text_Color,QColor(0,255,51));
    m_btnGroup->addButton(m_recvTextColorBtn,Recv_Text_Color);

    buttonsFr->setLayout(buttonsLayout);
    mainLayout->addWidget(buttonsFr);

    QFrame  *hLine1 = new QFrame(this);
    hLine1->setFrameShape(QFrame::HLine);
    hLine1->setFrameShadow(QFrame::Sunken);
    hLine1->setLineWidth(2);
    mainLayout->addWidget(hLine1);

    QFrame   *pointSizeFr = new QFrame(this);
    mainLayout->addWidget(pointSizeFr);


    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    QPushButton *okBtn = new QPushButton("Ok");
    btnLayout->addWidget(okBtn);

    btnLayout->addStretch(1);
    QPushButton *cancelBtn = new QPushButton("Cancel");
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch(1);

    mainLayout->addLayout(btnLayout);

    this->setLayout(mainLayout);

    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(slotOkBtn()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(slotCancelBtn()));

    connect(m_bkColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_shotColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_firedShotColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_recvColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_shotLineColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_recvLineColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_legendColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_deployedColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_pickedColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_downloadedColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_shotTextColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
    connect(m_recvTextColorBtn,SIGNAL(colorChanged(int,QColor)),SIGNAL(colorChanged(int,QColor)));
}


void PreferenceDlg::slotOkBtn()
{
    //刷新值
    m_colorMap.insert(m_bkColorBtn->id(),m_bkColorBtn->color());
    m_colorMap.insert(m_shotColorBtn->id(),m_shotColorBtn->color());
    m_colorMap.insert(m_recvColorBtn->id(),m_recvColorBtn->color());
    m_colorMap.insert(m_shotLineColorBtn->id(),m_shotLineColorBtn->color());
    m_colorMap.insert(m_recvLineColorBtn->id(),m_recvLineColorBtn->color());
    m_colorMap.insert(m_legendColorBtn->id(),m_legendColorBtn->color());
    m_colorMap.insert(m_deployedColorBtn->id(),m_deployedColorBtn->color());
    m_colorMap.insert(m_pickedColorBtn->id(),m_pickedColorBtn->color());
    m_colorMap.insert(m_shotTextColorBtn->id(),m_shotTextColorBtn->color());
    m_colorMap.insert(m_recvTextColorBtn->id(),m_recvTextColorBtn->color());

    this->close();
}

void PreferenceDlg::slotCancelBtn()
{
    //使用现有的值刷新设置
    emit colorMapChanged(m_colorMap);

    this->close();
}
