#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gsqlfunctions.h"

#include <QToolButton>
#include <QDir>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

#include <QListWidgetItem>
#include <QListWidget>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QBoxLayout>
#include <QMessageBox>

#include "gassistant.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_Path = MainWinPublic::rootPath();
    this->setFixedSize(QSize(860,440));

    m_projManager      = new ProjectManager;
    m_exportSpsFileDlg = 0;

    initWindow();
    initMenu();

    connect(m_projManager,SIGNAL(signalOpenProject(QString)),this,SLOT(slotOpenProject(QString)));

    QRect rect = QApplication::desktop()->screenGeometry();
    this->move((rect.width()-width())/2,(rect.height()-height())/2);
}
MainWindow::~MainWindow()
{
    delete ui;
    delete m_projManager;
}

//#include <QResizeEvent>
//void MainWindow::resizeEvent(QResizeEvent *e)
//{
//    qDebug()<<e->size();
//}

void MainWindow::initWindow()
{
    //---------------------------------------
    this->setWindowTitle(QString(WindowTile_Project)+"NULL");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    //---------------------------------------
    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *launcherLayout = new QHBoxLayout;
    ModToolButton *lmgrBtn = new ModToolButton(QIcon(m_Path+Icon_Module_LMGR),"LMGR",this);
    launcherLayout->addWidget(lmgrBtn);

    ModToolButton *rnmBtn = new ModToolButton(QIcon(m_Path+Icon_Module_RNM),"RNM",this);
    launcherLayout->addWidget(rnmBtn);

    ModToolButton *dmcBtn = new ModToolButton(QIcon(m_Path+Icon_Module_DMC),"DMC",this);
    launcherLayout->addWidget(dmcBtn);

    launcherLayout->setSpacing(36);
    ui->LauncherBox->setLayout(launcherLayout);
    //module, signal -slot----------
    connect(lmgrBtn,SIGNAL(signalRun(QString)),this,SLOT(slotRunModule(QString)));
    connect(rnmBtn,SIGNAL(signalRun(QString)),this,SLOT(slotRunModule(QString)));
    connect(dmcBtn,SIGNAL(signalRun(QString)),this,SLOT(slotRunModule(QString)));

    connect(lmgrBtn,SIGNAL(signalAbout(QString)),this,SLOT(slotAboutModule(QString)));
    connect(rnmBtn,SIGNAL(signalAbout(QString)),this,SLOT(slotAboutModule(QString)));
    connect(dmcBtn,SIGNAL(signalAbout(QString)),this,SLOT(slotAboutModule(QString)));

    //--------------------------------------------------
    QToolButton *newProjBtn = new QToolButton(this);
    newProjBtn->setText("New Project  ");
    newProjBtn->setIcon(QIcon(m_Path+Icon_New_Project));
    newProjBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QToolButton *openProjBtn = new QToolButton(this);
    openProjBtn->setText("Open Project");
    openProjBtn->setIcon(QIcon(m_Path+Icon_Open_Project));
    openProjBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(9,2,9,2);
    leftLayout->addWidget(newProjBtn);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(openProjBtn);

    leftLayout->addSpacing(12);

    //current project
    QLabel *currentLbl = new QLabel("Current Project:");
    leftLayout->addWidget(currentLbl);
    m_currentProjLbl = new QLabel(this);
    QFont font0;
    font0.setBold(true);
    font0.setUnderline(true);
    m_currentProjLbl->setFont(font0);
    m_currentProjLbl->setWordWrap(true);
    leftLayout->addWidget(m_currentProjLbl);

    leftLayout->addStretch(1);

    ui->LeftWg->setLayout(leftLayout);

    topLayout->addWidget(ui->LeftWg);
    topLayout->addWidget(ui->LauncherBox);
    mainLayout->addLayout(topLayout);

    //botttom-------------------
    QVBoxLayout *bottomLayout = new QVBoxLayout;
    QHBoxLayout *lblLayout = new QHBoxLayout;
    QLabel *recentLbl = new QLabel("Recent Projects:");

    QFont font;
    font.setItalic(true);
    // font.setUnderline(true);
    recentLbl->setFont(font);
    QPalette lblPalette = recentLbl->palette();
    lblPalette.setColor(QPalette::WindowText,Qt::gray);
    recentLbl->setPalette(lblPalette);

    lblLayout->addWidget(recentLbl);
    lblLayout->addStretch(1);
    //-------------------
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    bottomLayout->addLayout(lblLayout);
    bottomLayout->addWidget(ui->listWidget);
    ui->BottomWg->setLayout(bottomLayout);

    m_projManager->setProjectListWidget(ui->listWidget);

    mainLayout->addWidget(ui->BottomWg,1);

    this->centralWidget()->setLayout(mainLayout);

    connect(openProjBtn,SIGNAL(clicked()),this,SLOT(slotOpenProject()));
    connect(newProjBtn,SIGNAL(clicked()),this,SLOT(slotNewProject()));


}

void MainWindow::initMenu()
{
    //File Menu-------
    ui->menuFile->addAction("New Project ",this,SLOT(slotNewProject()),QKeySequence(tr("Ctrl+N")));
    ui->menuFile->addAction("Open Project ",this,SLOT(slotOpenProject()),QKeySequence(tr("Ctrl+O")));

    QMenu *recentMenu = new QMenu("Recent Projects");
    m_projManager->createRecentProjMenu(recentMenu);
    ui->menuFile->addMenu(recentMenu);


    ui->menuFile->addSeparator();
    ui->menuFile->addAction("Exit",this,SLOT(close()));

    //Module Menu-------
    QAction *LMGRAction = new QAction("Line Manager(LMGR)",ui->menuModule);
    LMGRAction->setData(QVariant("LMGR"));
    connect(LMGRAction,SIGNAL(triggered()),this,SLOT(slotRunModule()));
    ui->menuModule->addAction(LMGRAction);

    QAction *RNMAction = new QAction("Remote Node Manager(RNM)",ui->menuModule);
    RNMAction->setData(QVariant("RNM"));
    connect(RNMAction,SIGNAL(triggered()),this,SLOT(slotRunModule()));
    ui->menuModule->addAction(RNMAction);

    QAction *DMCAction = new QAction("Data Manager & QC(DMC)",ui->menuModule);
    DMCAction->setData(QVariant("DM"));
    connect(DMCAction,SIGNAL(triggered()),this,SLOT(slotRunModule()));
    ui->menuModule->addAction(DMCAction);

    //Tools Menu----------------
    ui->menuTools->addAction(tr("&Export SPS Files"),this,SLOT(slotExportSPSFiles()));

    //Help
    ui->menuHelp->addAction("Help",this,SLOT(slotHelp()));
}

void MainWindow::setCurrentProject(ProjectInfo projInfo)
{
    m_projInfo = projInfo;
    //
    setWindowTitle(WindowTile_Project+m_projInfo.ProjectName);
    QString file = projInfo.ProjectPath+Dir_Separator+projInfo.ProjectName+".gpro";
    m_currentProjLbl->setText(projInfo.ProjectName+"("+file+""+")");
    //项目管理
    m_projManager->saveCurrentProject(m_projInfo);
}
void MainWindow::setCurrentProject(const QString &projPath,const QString &projName)
{
    m_projInfo.ProjectPath = projPath;
    m_projInfo.ProjectName = projName;

    setWindowTitle(WindowTile_Project+projName);
    QString file = projPath+Dir_Separator+projName+".gpro";
    m_currentProjLbl->setText(projName+"("+file+""+")");

    //项目管理
    m_projManager->saveCurrentProject(m_projInfo);
}

//.gpro文件,表示gobs软件项目文件
int MainWindow::slotOpenProject()
{
    QString profileName = ProjectManager::getOpenProject();
    if(profileName.isEmpty())
        return -1;
    return slotOpenProject(profileName);
}

int MainWindow::slotOpenProject(const QString &profileName)
{
    //设置当前工区
    int index = profileName.lastIndexOf(Dir_Separator);

    QString projPath = profileName.left(index);
    QString projName = profileName.mid(index+1);
    projName.chop(5);   //移除".gpro"

    setCurrentProject(projPath,projName);
    return 0;
}

//创建工区
void MainWindow::slotNewProject()
{
    ProjectInfo projectInfo = ProjectManager::createProject();
    if(projectInfo.ProjectName.isEmpty() || projectInfo.ProjectPath.isEmpty()){
        return;
    }
    //创建目录和Project文件
    projectInfo.ProjectPath += Dir_Separator+projectInfo.ProjectName;
    int ok  = Project::createProject(projectInfo);
    if(ok != 0)
    {
        QMessageBox::warning(this,"error",QString("Create project file error,ok =%1").arg(ok));
        return;
    }

    //创建数据库
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    bool ret = sqlFunc->openDataBase(projectInfo.ProjectPath,projectInfo.ProjectName);
    if(!ret)
    {
        QMessageBox::critical(this,"error",QString("Open Database error!"));
        return;
    }

//    sqlFunc->createTable()
    sqlFunc->closeDataBase();

    this->setCurrentProject(projectInfo);
}

void MainWindow::slotAboutModule(const QString &modName)
{
    qDebug()<<"About:"<<modName;

    QString informStr;
    if(modName == "LMGR")
        informStr= "LMGR module displays the area and manages the GOBS that corresponds to reciever." ;
    else if(modName == "RNM")
        informStr= "RNM module monitors GOBS parameter status and implements FTP operatoions.";
    else if(modName == "DMC")
        informStr= "DMC module extracts segy data and display segy files.";
    QMessageBox::information(this,modName,informStr);

}

void MainWindow::slotRunModule(const QString &modName)
{
    //判断是否打开工区
    if(m_projInfo.ProjectPath.isEmpty() || m_projInfo.ProjectName.isEmpty())
    {
        int ret = slotOpenProject();
        if(ret!=0)
            return;
    }

    qDebug()<<"mod name:"<<modName;
    QString key = modName;

    //如果是菜单选项运行----
    if(key.isEmpty())
    {
        QAction *action = dynamic_cast<QAction *>(sender());
        if(action)
            key = action->data().toString();
        else
            return;
    }
    //----------------------------------------------------
    //判断进程是否存在，存在则进程
    QProcess *process = m_processes.value(key,0);
    if(process)
    {
        QProcess::ProcessState state = process->state();
        if(state == QProcess::Running)
        {
            QMessageBox::warning(this,"Note","The Module is Running");
            return;
        }
    }else
        process = new QProcess(this);

    //根据模块key 值和工区加载启动模块-----------
    //每个模块单例模式
    QString binDir = MainWinPublic::appPath()+Dir_Separator;
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    //参数工区路径和名称
    QStringList argsList;
    argsList<<m_projInfo.ProjectPath<<m_projInfo.ProjectName;

#ifdef Q_OS_LINUX
    if(key == "LMGR")
        process->start(binDir+"LineManager",argsList);
    else if(key == "RNM")
        process->start(binDir+"RNManager",argsList);
    else if(key == "DMC")
        process->start(binDir+"DataMgrControl",argsList);
#else   //Q_OS_WIN
    if(key == "LMGR")
        process->start(binDir+"LineManager.exe",argsList);
    else if(key == "RNM")
        process->start(binDir+"RNManager.exe",argsList);
    else if(key == "DMC")
        process->start(binDir+"DataMgrControl.exe",argsList);
#endif
    m_processes.insert(key,process);
}


void MainWindow::slotExportSPSFiles()
{
    if(!m_exportSpsFileDlg){
        m_exportSpsFileDlg = new ExportSPSFilesDlg(this);
    }
    m_exportSpsFileDlg->show();
}

void MainWindow::slotHelp()
{
    bool ret = Assistant::showAssistant("Launcher");
    if(!ret){
        QMessageBox::warning(this,"warning","Open the help document failed.");
    }
}


