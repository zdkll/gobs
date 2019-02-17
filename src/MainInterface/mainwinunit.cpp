#include "mainwinunit.h"

#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent>

MainWinUnit::MainWinUnit()
{

}



ModToolButton::ModToolButton(const QIcon &icon,const QString &text,QWidget *parent)
    :QToolButton(parent)
{
    this->setFixedSize(160,160);
    this->setIcon(icon);
    this->setIconSize(QSize(128,128));
    this->setText(text);
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setFocusPolicy(Qt::StrongFocus);
    menu = 0;
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slotCustomContextMenuRequested(QPoint)));

}

ModToolButton::~ModToolButton()
{
    if(menu)
        delete menu;
}

void ModToolButton::mouseDoubleClickEvent(QMouseEvent *e)
{
    //emit signalRun(this->text());
    QToolButton::mouseDoubleClickEvent(e);
}

void ModToolButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        emit signalRun(this->text());

    QToolButton::mousePressEvent(e);
}

void ModToolButton::slotCustomContextMenuRequested(QPoint pt)
{
    if(!menu){
        menu = new QMenu(this);
        menu->addAction("Run",this,SLOT(slotRun()));
        menu->addAction("About",this,SLOT(slotAbout()));
    }

    menu->exec(mapToGlobal(pt));
}

void ModToolButton::slotAbout()
{
    emit signalAbout(this->text());
}

void ModToolButton::slotRun()
{
    emit signalRun(this->text());
}

//---------------------------------------------
LabelLink::LabelLink(const QString &text,QWidget *parent)
    :QLabel(text,parent)
{
    //   this->set
}


//---------------------------------------------
ProjectManager::ProjectManager(QObject *parent)
    :QObject(parent),m_listWgMenu(0)
{
    QString projfileName = Core::docmentsPath() + History_Projects_File;
    //    qDebug()<<"project file:"<<projfileName;
    QFile projFile(projfileName);
    if(!projFile.open(QIODevice::ReadOnly))
        return;

    //读取项目列表
    QTextStream in(&projFile);
    m_projList = in.readAll().split(QRegExp("\n"),QString::SkipEmptyParts);
    //    qDebug()<<m_projList;
    projFile.close();
}
ProjectManager::~ProjectManager()
{

}

void ProjectManager::createRecentProjMenu(QMenu *menu)
{
    m_menu = menu;
    if(m_projList.size()<1){
        menu->setEnabled(false);
        return;
    }
    foreach (QString proj, m_projList) {
        menu->addAction(proj);
    }
    menu->addSeparator();
    menu->addAction("Clear Menu");

    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(slotRecentFiles(QAction*)));
}

void ProjectManager::slotRecentFiles(QAction* action)
{
    QString text = action->text();

    qDebug()<<"text:"<<text;

    //清除菜单选项
    if(text == "Clear Menu")
    {
        m_projList.clear();
        updateRecentProjects();
        return;
    }else //打开项目
    {
        //发送给主界面打开工区
        emit signalOpenProject(text);
    }
}

void ProjectManager::updateRecentProjects()
{
    //1 更新菜单----
    m_menu->clear();
    if(m_projList.size()<1){
        m_menu->setEnabled(false);
    }
    else
    {
        m_menu->setEnabled(true);
        foreach (QString proj, m_projList) {
            m_menu->addAction(proj);
        }
        m_menu->addSeparator();
        m_menu->addAction("Clear Menu");
    }

    //更新列表
    m_projListWg->clear();
    foreach (QString project, m_projList) {
        m_projListWg->addItem(new QListWidgetItem(project));
    }

    //项目列表文件
    QFile file(Core::docmentsPath() + History_Projects_File);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream out(&file);
    foreach(QString  proj, m_projList) {
        out<<proj<<"\n";
    }
    file.close();
}

void ProjectManager::setProjectListWidget(QListWidget *listWidget)
{
    //初始化列表
    m_projListWg = listWidget;
    m_projListWg->clear();
    foreach (QString project, m_projList) {
        m_projListWg->addItem(new QListWidgetItem(project));
    }

    m_projListWg->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_projListWg,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(slotDoubleClicked(QListWidgetItem *)));
    connect(m_projListWg,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

const QString ProjectManager::getOpenProject()
{
    return QFileDialog::getOpenFileName(NULL,"Open Project",MainWinPublic::workPath()
                                        , "gPro(*.gpro)");
}

const ProjectInfo ProjectManager::createProject()
{
    ProjectInfo projectInfo;

    CreateProjDialog m_createProjDlg;

    m_createProjDlg.exec();

    projectInfo = m_createProjDlg.getProjInfo();

    return projectInfo;
}

void ProjectManager::saveCurrentProject(const ProjectInfo &projInfo)
{
    QString projFile = projInfo.ProjectPath+Dir_Separator+projInfo.ProjectName+".gpro";
    qDebug()<<"save:"<<projFile;
    //判断当前项目是否存在，存在则只调整位置
    int index = m_projList.indexOf(projFile);
    if(index>=0)
    {
        m_projList.removeAt(index);
    }
    //不存在则开头插入
    m_projList.prepend(projFile);

    //更新最近文件列表
    updateRecentProjects();
}

void ProjectManager::slotDoubleClicked(QListWidgetItem *item)
{
    emit signalOpenProject(item->text());
}

void  ProjectManager::customContextMenuRequested(QPoint )
{
    if(!m_listWgMenu) {
        m_listWgMenu = new QMenu(m_projListWg);
        m_listWgMenu->addAction("Open",this,SLOT(listWgOpenProject()));
        m_listWgMenu->addAction("clear",this,SLOT(listWgClearRencentProject()));
    }

    QList<QAction*> actions = m_listWgMenu->actions();

    QListWidgetItem *item = m_projListWg->currentItem();
    if(!item)
    {
        actions[0]->setEnabled(false);
        actions[1]->setEnabled(false);
    }
    else{
        //open action
        if(!item->isSelected())
            actions[0]->setEnabled(false);
        else
            actions[0]->setEnabled(true);
    }

    //clear
    if(m_projListWg->count()>0)
        actions[1]->setEnabled(true);
    else
        actions[1]->setEnabled(false);

    m_listWgMenu->exec(QCursor::pos());
}

void ProjectManager::listWgOpenProject()
{
    emit signalOpenProject(m_projListWg->currentItem()->text());
}

void ProjectManager::listWgClearRencentProject()
{
    m_projList.clear();
    updateRecentProjects();
}

CreateProjDialog::CreateProjDialog()
{
    this->setWindowTitle("Create Project");
    initDlg();

    fileIsok = 0;
}

void CreateProjDialog::initDlg()
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    //project name---------------------------------
    QGridLayout *gridLayout = new QGridLayout;
    QLabel *proNameLbl = new QLabel("Project Name:",this);
    proNameEdit = new QLineEdit(this);
    proNameHint = new QLabel(this);
    proNameHint->setMinimumWidth(120);

    gridLayout->addWidget(proNameLbl,0,0);
    gridLayout->addWidget(proNameEdit,0,1);
    gridLayout->addWidget(proNameHint,0,2);

    //project path---------------------------------
    QLabel *proPathLbl = new QLabel("Project Path:",this);
    proPathEdit = new QLineEdit(this);
    proPathEdit->setReadOnly(true);
    //设置为当前workPath
    proPathEdit->setText(MainWinPublic::workPath());
    proPathBrseBtn = new QPushButton("...",this);
    proPathBrseBtn->setMaximumWidth(36);

    gridLayout->addWidget(proPathLbl,1,0);
    gridLayout->addWidget(proPathEdit,1,1);
    gridLayout->addWidget(proPathBrseBtn,1,2,Qt::AlignLeft);

    defultWorkPathBtn = new QRadioButton("Set path as defult workpath",this);
    gridLayout->addWidget(defultWorkPathBtn,2,0,1,3,Qt::AlignCenter);

    //button-----------------
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *cancelBtn = new QPushButton("Cancel",this);
    QPushButton *okBtn = new QPushButton("Ok",this);
    btnLayout->addStretch(1);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addSpacing(36);
    btnLayout->addWidget(okBtn);
    btnLayout->addStretch(1);

    gridLayout->addLayout(btnLayout,3,0,1,3);

    vLayout->addLayout(gridLayout);

    this->setLayout(vLayout);
    this->resize(580,160);

    connect(proNameEdit,SIGNAL(textChanged(QString)),this,SLOT(slotProNameChanged(QString)));
    connect(proPathBrseBtn,SIGNAL(clicked()),this,SLOT(slotBrowser()));

    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(slotCancel()));
    connect(okBtn,SIGNAL(clicked()),this,SLOT(slotOk()));
}


void CreateProjDialog::slotOk()
{
    //文件名为空
    if(proNameEdit->text().isEmpty()){
        proNameHint->setText("name is empty.");
        proNameHint->setStyleSheet("QLabel{background-color:red}");
        return;
    }

    //非法字符
    if(fileIsok == 1)
        return;

    //文件重名，提示
    if(fileIsok == 2)
    {
        QMessageBox box(QMessageBox::Warning,"warning",
                        "The File has been existed,"
                        "do you want to continue?",
                        QMessageBox::Yes|QMessageBox::No);
        switch (box.exec()) {
        case QMessageBox::Yes:
            break;
        case QMessageBox::No:
            return;
        default:
            break;
        }
    }

    m_projInfo.ProjectName = proNameEdit->text().trimmed();
    m_projInfo.ProjectPath = proPathEdit->text().trimmed();

    //如果选择当前目录作为默认workPath，则保存workpath记录
    if(defultWorkPathBtn->isChecked())
    {
        MainWinPublic::saveWorkPath(m_projInfo.ProjectPath);
    }
    this->close();
}

void CreateProjDialog::slotBrowser()
{
    QString Path = QFileDialog::getExistingDirectory(this,"Select Path"
                                                     ,proPathEdit->text().trimmed());
    if(Path.isEmpty())
        return;

    proPathEdit->setText(Path);
    slotProNameChanged(proNameEdit->text().trimmed());
}

void CreateProjDialog::slotCancel()
{
    this->close();
}

void CreateProjDialog::slotProNameChanged(const QString &text)
{
    QString proName = text.trimmed();
    QString proPath = proPathEdit->text().trimmed();
    proNameHint->setText("");
    fileIsok = 0;
    //1 判断名字是否合法
    int error_index = proName.indexOf(QRegExp(ILLEGAL_CHARS,Qt::CaseSensitive,QRegExp::RegExp2));
    if(error_index>=0)
    {
        proNameHint->setStyleSheet("QLabel{background-color:red}");
        proNameHint->setText("Invalid char \""+proName[error_index]+"\".");
        fileIsok = 1;
        return;
    }

    //2 判断名字是否存在
    if(!proPath.endsWith('/'))
        proPath += Dir_Separator;
    QString file_name = proPath + proName;

    QDir dir(file_name);
    if(dir.exists())
    {
        proNameHint->setStyleSheet("QLabel{background-color:yellow}");
        proNameHint->setText("File exists.");
        fileIsok = 2;
        return ;
    }
    proNameHint->setStyleSheet("");
}
