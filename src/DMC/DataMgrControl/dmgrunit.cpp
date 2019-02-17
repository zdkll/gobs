#include "dmgrunit.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QListWidget>

#include "ui_exportsegydlg.h"
#include "ui_opensegydlg.h"
#include "ui_plotparamdlg.h"

#include "datamgrcontrol.h"
#include "gsegy.h"

//menuManager
MenuManager::MenuManager(QObject *parent)
    :QObject(parent)
{

}

void MenuManager::createMenu(DataMgrControl *dataMgrControl,QMenuBar *menuBar)
{
    //菜单
    fileMenu = new QMenu("File",menuBar);
    fileMenu->addAction(tr("&Export Seg-Y Data")
                        ,dataMgrControl,SLOT(slotNewSegyData()),QKeySequence(tr("Ctrl+E")));
    fileMenu->addAction(tr("&Open Seg-Y File")
                        ,dataMgrControl,SLOT(slotOpenSegyFile()),QKeySequence(tr("Ctrl+O")));
    fileMenu->addAction(tr("&Exit"),dataMgrControl,SLOT(slotExit()),QKeySequence(tr("Ctrl+Q")));
    menuBar->addMenu(fileMenu);

    //tools
    toolsMenu = new QMenu(tr("Tools"),menuBar);
    toolsMenu->addAction(tr("Display Option"),dataMgrControl,SLOT(slotDisplayOption()));

    menuBar->addMenu(toolsMenu);

    //其他菜单扩展
    //...
    helpMenu = new QMenu(tr("Help"));
    helpMenu->addAction(tr("Help"),dataMgrControl,SLOT(slotAbout()));
    menuBar->addMenu(helpMenu);
}

//toolManager
ToolManager::ToolManager(QObject *parent)
    :QObject(parent)
{

}

void ToolManager::createTools(DataMgrControl *dataMgrControl,QToolBar *toolBar)
{
    QString m_Path = QApplication::applicationDirPath()+"/..";
    toolBar->setIconSize(QSize(24,24));
    //open segy file
    toolBar->addAction(QIcon(m_Path+ICON_OPEN_FILE),"Open Segy File"
                       ,dataMgrControl,SLOT(slotOpenSegyFile()));
    toolBar->addSeparator();

    //前后播放，跳转到某一道集
    toolBar->addAction(QIcon(m_Path+ICON_PREVIOUS),"Previous Gather"
                       ,dataMgrControl,SLOT(slotPreviousGather()));
    toolBar->addAction(QIcon(m_Path+ICON_NEXT),"Next Gather"
                       ,dataMgrControl,SLOT(slotNextGather()));
    toolBar->addAction(QIcon(m_Path+ICON_MOVE_TO),"Move To Gather"
                       ,dataMgrControl,SLOT(slotMoveToGather()));

    toolBar->addSeparator();

    //zoom In 缩放
    m_zoomInBtn = new DownToolButton(toolBar);
    m_zoomInBtn->setText("Zoom In");
    m_zoomInBtn->setToolTip("Zoom In");
    m_zoomInBtn->setIcon(QIcon(m_Path+ICON_ZOOM_IN));

    connect(m_zoomInBtn,SIGNAL(btnTriggerDowned(bool)), this
            ,SLOT(slotZoomIn(bool)));
    connect(this,SIGNAL(signalZoomInEnable(bool)),dataMgrControl,
            SLOT(slotZoomIn(bool)));
    toolBar->addWidget(m_zoomInBtn);

    QToolButton *zoomOutBtn = new QToolButton(toolBar);
    zoomOutBtn->setText("Zoom Out");
    zoomOutBtn->setToolTip("Zoom Out");
    zoomOutBtn->setIcon(QIcon(m_Path+ICON_ZOOM_OUT_FULL));
    connect(zoomOutBtn,SIGNAL(clicked()),dataMgrControl,
            SLOT(slotZoomOut()));
    toolBar->addWidget(zoomOutBtn);

    toolBar->addSeparator();

    //trace header
    m_traceInfBtn = new DownToolButton(toolBar);
    m_traceInfBtn->setText("Trace Information");
    m_traceInfBtn->setIcon(QIcon(m_Path+ICON_INFORMATION));
    toolBar->addWidget(m_traceInfBtn);
    m_traceInfBtn->setToolTip("Trace Header Information");
    connect(m_traceInfBtn,SIGNAL(btnTriggerDowned(bool))
            ,this,SLOT(slotTraceInformation(bool)));
    connect(this,SIGNAL(signalTraceInformEnable(bool))
            ,dataMgrControl,SLOT(slotTraceInform(bool)));

    //display option
    toolBar->addAction(QIcon(m_Path+ICON_SETTINGS),"Display Option",dataMgrControl
                       ,SLOT(slotDisplayOption()));
}
void ToolManager::slotZoomIn(bool enable)
{
    bool zoomEnable = enable;
    if(zoomEnable)
        m_traceInfBtn->setBtnDown(false);
    emit signalZoomInEnable(zoomEnable);
}

void ToolManager::slotTraceInformation(bool enable)
{
    bool traceInforEnable = enable;
    if(traceInforEnable)
        m_zoomInBtn->setBtnDown(false);
    emit signalTraceInformEnable(traceInforEnable);
}

//OpenSegyDlg---------------------------------
OpenSegyDlg::OpenSegyDlg(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::OpenSegyDlg)
{
    ui->setupUi(this);
    this->setWindowTitle("Open Segy File Dialog");


    m_fileDetailDlg  = new FileDetailDlg(this);

    m_OpenFilesType  = One_Component;//默认打开一个分量文件显示

    ui->maxShowTraceSpx->setRange(1,INT_MAX);
    ui->maxShowTraceSpx->setValue(500);
    ui->startTraceSpx->setRange(1,INT_MAX);
    ui->startTraceSpx->setValue(1);

    m_maxShowTraces = 500;
    ui->FileExpandWg->hide();
    ui->label->setText("File:");

    m_brwserBtnGrp  = new QButtonGroup(this);
    m_brwserBtnGrp->addButton(ui->brwserBtn,0);
    m_brwserBtnGrp->addButton(ui->brwserBtn_2,1);
    m_brwserBtnGrp->addButton(ui->brwserBtn_3,2);
    m_brwserBtnGrp->addButton(ui->brwserBtn_4,3);

    m_fileNameEdits.append(ui->fileNameEdit);
    m_fileNameEdits.append(ui->fileNameEdit_2);
    m_fileNameEdits.append(ui->fileNameEdit_3);
    m_fileNameEdits.append(ui->fileNameEdit_4);
    connect(m_brwserBtnGrp,SIGNAL(buttonClicked(int)),this,SLOT(slotBrwBtnClicked(int)));
    this->adjustSize();

}

OpenSegyDlg::~OpenSegyDlg()
{
    //如果当前有打开文件则关闭文件-----
    if(m_segyFileHandles.size()>0)
    {
        int ok = -1;
        for(int i=0;i<m_segyFileHandles.size();i++)
            gobs_close_segy_func(m_segyFileHandles[i]->openTrace,&ok);
    }
    qDeleteAll(m_segyFileHandles);
}

void OpenSegyDlg::slotBrwBtnClicked(int id)
{
    if(m_currentPath.isEmpty())
        m_currentPath = QDir::currentPath();

    //打开文件系统选择文件
    QString fileName = QFileDialog::getOpenFileName(this,"Choose File to Open"
                                                    ,m_currentPath,tr("Segy(*.segy)"));
    if(fileName.isEmpty())
        return;
    m_currentPath = QFileInfo(fileName).filePath();

    //一分量
    QStringList m_fileList;
    if(m_OpenFilesType == One_Component){
        m_fileNameEdits[id]->setText(fileName);
        m_fileList<<fileName;
        m_fileNameEdits[1]->clear();
        m_fileNameEdits[2]->clear();
        m_fileNameEdits[3]->clear();
    }else if(m_OpenFilesType == Four_Component){
        //四分量，根据选择文件判断查找其他文件，保证文件存在并且正确
        QStringList componentSuffixs(QStringList()<<"bhx.segy"
                                     <<"bhy.segy"
                                     <<"bhz.segy"
                                     <<"hyd.segy");
        QList<int> fileIndex(QList<int>()<<0<<1<<2<<3);
        int idx = componentSuffixs.indexOf(fileName.right(8));
        if(idx != id){
            QMessageBox::warning(this,"warning",QString("Need input %1 file.")
                                 .arg(componentSuffixs[idx]));
            return;
        }
        m_fileNameEdits[fileIndex[idx]]->setText(fileName);
        fileIndex.removeAt(idx);
        //设置其他文件，需要每个分量文件都存在才能打开显示
        QString prefixName = fileName;
        prefixName.chop(8);
        bool file_ok = true;
        for(int i=0;i<fileIndex.size();i++){
            fileName = prefixName+componentSuffixs[fileIndex[i]];
            if(!QFile(fileName).exists()){
                m_fileNameEdits[fileIndex[i]]->clear();
                file_ok  = false;
            }else
                m_fileNameEdits[fileIndex[i]]->setText(fileName);
        }
        //文件没有问题，打开文件
        if(!file_ok) return;
        for(int i =0;i<m_fileNameEdits.size();i++)
            m_fileList.append(m_fileNameEdits[i]->text().trimmed());
    }

    //打开文件，获取基本信息
    if(!openSegyFile(m_fileList))
    {
        QMessageBox::warning(this,"Open File","Open File Error.");
    }
}

bool OpenSegyDlg::openSegyFile(const QStringList &files)
{
    //打开多个文件
    qDebug()<<"open files:"<<files;
    //打开文件获取信息
    int mode = Read_Only;
    int ok = -1;
    //打开所有分量文件
    QList<SegyFileHandle *> segyFileHandles;
    for(int i=0;i<files.size();i++){
        char *binary3200 = new char[3200];
        BinaryHead *binaryhead = new BinaryHead;
        SegyInfo   *segyInfo   = new SegyInfo;
        OpenTrace *openTrace = gobs_open_segy_func(files[i].toLatin1().data(),
                                                   &mode,binary3200,binaryhead,
                                                   segyInfo,&ok);
        qDebug()<<"open segy file,ok="<<ok;
        if(ok != 0)
        {
            delete[] binary3200;
            delete   binaryhead;
            delete   segyInfo;
            qDeleteAll(segyFileHandles);
            segyFileHandles.clear();
            return false;
        }
        //2 保存文件信息
        ui->nsEdit->setText(QString::number(segyInfo->ns));
        ui->sampleInterEdit->setText(QString::number(segyInfo->sampleInterval));
        ui->nTraceEdit->setText(QString::number(segyInfo->traces));
        ui->maxShowTraceSpx->setRange(1,segyInfo->traces);
        ui->startTraceSpx->setRange(1,segyInfo->traces);

        SegyFileHandle *segyFileHandle = new SegyFileHandle;
        //更新当前文件信息
        segyFileHandle->fileName   = files[i];
        segyFileHandle->binary3200 = binary3200;
        segyFileHandle->binaryhead = binaryhead;
        segyFileHandle->segyInfo   = segyInfo;
        segyFileHandle->openTrace  = openTrace;

        segyFileHandles.append(segyFileHandle);
    }

    //打开完成，关闭之前打开的文件
    for(int i=0;i<m_segyFileHandles.size();i++){
        //打开成功，需要关闭上一个打开的文件
        if(m_segyFileHandles[i]->openTrace)
            gobs_close_segy_func(m_segyFileHandles[i]->openTrace,&ok);
    }
    qDeleteAll(m_segyFileHandles);

    m_segyFileHandles = segyFileHandles;

    //保存路径
    m_currentPath = QFileInfo(files[0]).path();
    //文件信息对话框显示信息
    m_fileDetailDlg->setSegyFileInfo(m_segyFileHandles[0]->binary3200
            ,m_segyFileHandles[0]->binaryhead,m_segyFileHandles[0]->segyInfo);

    return true;
}

bool OpenSegyDlg::getFileList(QStringList &fileList)
{
    if(m_OpenFilesType == One_Component)
    {
        if(m_fileNameEdits[0]->text().trimmed().isEmpty())
            return false;

        fileList.append(m_fileNameEdits[0]->text().trimmed());
    }else{
        for(int i=0;i<4;i++){
            if(m_fileNameEdits[i]->text().trimmed().isEmpty())
                return false;

            fileList.append(m_fileNameEdits[i]->text().trimmed());
        }
    }
    return true;
}

void OpenSegyDlg::on_okBtn_clicked()
{
    QStringList fileList;
    //获取文件
    if(!getFileList(fileList))
        return;
    bool opened = true;
    //分量改变
    if(fileList.size()!=m_segyFileHandles.size()){
        opened = false;
    }
    else{
        //判断每个分量文件是否相等
        for(int i=0;i<m_segyFileHandles.size();i++)
            if(m_segyFileHandles[i]->fileName != fileList[i])
            {
                opened  = false;
                break;
            }
    }

    if(!opened)
    {
        //打开文件
        if(!openSegyFile(fileList))
        {
            QMessageBox::warning(this,"Open File","Open File Error.");
            return;
        }
    }

    m_maxShowTraces = ui->maxShowTraceSpx->value();
    //读取数据时候从0开始
    m_startTrace    = ui->startTraceSpx->value()-1;
    //发送文件信息给主界面
    emit signalOpenedFile(m_segyFileHandles);

    this->close();
}

void OpenSegyDlg::on_cancelBtn_clicked()
{
    this->close();
}

void OpenSegyDlg::on_detailBtn_clicked()
{
    m_fileDetailDlg->show();
}

void OpenSegyDlg::on_OneComRbtn_clicked()
{
    if(m_OpenFilesType==One_Component)
        return;
    m_OpenFilesType = One_Component;
    ui->label->setText("File:");
    int decHeight = ui->FileExpandWg->height();
    ui->FileExpandWg->hide();
    ui->groupBox_3->resize(ui->groupBox_3->width()
                           ,ui->groupBox_3->height() -decHeight);
    this->adjustSize();
}

void OpenSegyDlg::on_FourComRbtn_clicked()
{
    if(m_OpenFilesType==Four_Component)
        return;
    m_OpenFilesType = Four_Component;
    ui->label->setText("File(bhx):");
    ui->FileExpandWg->show();
}


//GatherRecord------------------------
GatherRecord::GatherRecord(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Read Gather");

    initDlg();
    this->resize(276,84);
}

void GatherRecord::initDlg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    m_readLabel  = new QLabel("Read Gather()");
    m_gatherSpx  = new QSpinBox;
    QHBoxLayout *horiLayout = new QHBoxLayout;
    horiLayout->addWidget(m_readLabel);
    horiLayout->addWidget(m_gatherSpx);

    QPushButton *okBtn     = new QPushButton(tr("&Ok"));
    QPushButton *closeBtn = new QPushButton(tr("&Close"));

    QHBoxLayout *horiLayout1 = new QHBoxLayout;
    horiLayout1->addWidget(okBtn,0,Qt::AlignHCenter);
    horiLayout1->addWidget(closeBtn,0,Qt::AlignHCenter);

    connect(okBtn,SIGNAL(clicked()),this,SLOT(slotOkClicked()));
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(slotCloseClicked()));

    mainLayout->addLayout(horiLayout);
    mainLayout->addLayout(horiLayout1);
    this->setLayout(mainLayout);
}

void GatherRecord::setGatherInfo(const int &totalTraces,const int &maxTraces,const int &startTrace)
{
    if(totalTraces<1){
        QMessageBox::warning(this,"warning","Read no trace.");
        return;
    }
    //计算总的道集数
    m_gatherNum = ceil(float(startTrace)/float(maxTraces))
            + ceil(float(totalTraces-startTrace)/float(maxTraces));

    m_readLabel->setText(QString("Read Gather(%1~%2)").arg(1)
                         .arg(m_gatherNum));

    m_CurrentGather = ceil(float(startTrace)/float(maxTraces))+1;
    m_gatherSpx->setRange(1,m_gatherNum);
    m_gatherSpx->setValue(m_CurrentGather);

    emit signalReadGather(m_CurrentGather);
}

void GatherRecord::readNextGather()
{
    if(m_CurrentGather<m_gatherNum)
    {
        m_CurrentGather++;
        m_gatherSpx->setValue(m_CurrentGather);

        emit signalReadGather(m_CurrentGather);
    }
}

void GatherRecord::readPrevGather()
{
    if(m_CurrentGather>1)
    {
        m_CurrentGather--;
        m_gatherSpx->setValue(m_CurrentGather);
        emit signalReadGather(m_CurrentGather);
    }
}

void GatherRecord::slotOkClicked()
{
    int gatherNo = m_gatherSpx->value();
    m_CurrentGather = gatherNo;

    emit signalReadGather(m_CurrentGather);
}

void GatherRecord::slotCloseClicked()
{
    this->close();
}

//ExportSegyDlg-------------------------------
ExportSegyDlg::ExportSegyDlg(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::ExportSegyDlg)
{
    ui->setupUi(this);
    this->setWindowTitle("Export Seg-y Data");

    //设置validator
    ui->traceLenEdit->setValidator(new QIntValidator(100,100000,this));
    m_shotLineDlg = new ShotLineDlg(this);

    //
    m_areaDataInfo = 0;
    ui->textHeaderFrame->setFrameShape(QFrame::NoFrame);
}

ExportSegyDlg::~ExportSegyDlg()
{
    if(m_areaDataInfo){
        m_areaDataInfo->clearData();
        delete m_areaDataInfo;
        m_areaDataInfo = 0;
    }
}

void ExportSegyDlg::setProjectInfo(const ProjectInfo & projInfo)
{
    m_projInfo = projInfo;

    m_exportParameters.projectPath = m_projInfo.ProjectPath;
    m_exportParameters.projectName = m_projInfo.ProjectName;

    initByProject();
}

void ExportSegyDlg::initByProject()
{
    //初始化工区列表
    QString projFile = m_projInfo.ProjectPath+Dir_Separator+m_projInfo.ProjectName+Project_File_Suffix;
    QStringList areaList = Area::areasFromProject(projFile);
    ui->areasCbx->addItems(areaList);


    //设置默认原始数据 /data
    //ui->dataPathEdit->setText(QString("%1/%2").arg(m_projInfo.ProjectPath)
    //                    .arg("data"));
}

void ExportSegyDlg::updateScopeInfo()
{
    if(!m_areaDataInfo)
        return;
    if(ui->shotRbtn->isChecked())
    {
        StationInfo *shotStationInfo = m_areaDataInfo->shotStationInfo;
        //line
        ui->fromLineSpx->setRange(1,shotStationInfo->lineNum);
        ui->toLineSpx->setRange(1,shotStationInfo->lineNum);
        ui->fromLineSpx->setValue(1);
        ui->toLineSpx->setValue(shotStationInfo->lineNum);

        //station，计算站点最多的线
        int max_staNum = 0;
        for(int i =0;i<shotStationInfo->lineNum;i++){
            max_staNum = max_staNum<shotStationInfo->staLines[i].staNum?
                        shotStationInfo->staLines[i].staNum:max_staNum;
        }
        ui->fromStaSpx->setRange(1,max_staNum);
        ui->toStaSpx->setRange(1,max_staNum);
        ui->fromStaSpx->setValue(1);
        ui->toStaSpx->setValue(max_staNum);
    }

    if(ui->recvRbtn->isChecked())
    {
        StationInfo *recvStationInfo = m_areaDataInfo->recvStationInfo;
        //line
        ui->fromLineSpx->setRange(1,recvStationInfo->lineNum);
        ui->toLineSpx->setRange(1,recvStationInfo->lineNum);
        ui->fromLineSpx->setValue(1);
        ui->toLineSpx->setValue(recvStationInfo->lineNum);

        //station，计算站点最多的线
        int max_staNum = 0;
        for(int i =0;i<recvStationInfo->lineNum;i++){
            max_staNum = max_staNum<recvStationInfo->staLines[i].staNum?
                        recvStationInfo->staLines[i].staNum:max_staNum;
        }
        ui->fromStaSpx->setRange(1,max_staNum);
        ui->toStaSpx->setRange(1,max_staNum);
        ui->fromStaSpx->setValue(1);
        ui->toStaSpx->setValue(max_staNum);
    }
}


void ExportSegyDlg::on_CloseBtn_clicked()
{
    this->close();
}


void ExportSegyDlg::on_ShotLineBrwser_clicked()
{
    if(!m_shotLineDlg->shotLineFiles().isEmpty())
        m_shotLineDlg->show();
    QStringList shotlineFiles = QFileDialog::getOpenFileNames(this,"Select ShotLine Files"                                                          ,QDir::currentPath());
    if(shotlineFiles.isEmpty())
        return;

    shotlineFiles.sort();
    m_shotLineDlg->setShotLineFiles(shotlineFiles);
    m_shotLineDlg->show();
}

void ExportSegyDlg::on_areasCbx_currentIndexChanged(const QString &arg1)
{
    //根据area 工区，读取工区炮检点信息，初始化选择范围
    QString areaName = arg1;

    QStringList spsfiles = Area::spsFiles(m_projInfo,areaName);
    if(spsfiles.size()<2){
        QMessageBox::warning(this,"waring","Current area sps files error.");
        return;
    }

    if(m_areaDataInfo){
        m_areaDataInfo->clearData();
        delete m_areaDataInfo;
    }
    QString errString;
    m_areaDataInfo =  new AreaDataInfo;
    int ok = gobs_sps_files_SR_read(spsfiles,m_areaDataInfo,&errString);
    if(ok!=0)
    {
        QMessageBox::critical(this,"error",QString("Read area sps files error,Error:%1")
                              .arg(errString));
        return;
    }

    //设置最大范围
    updateScopeInfo();
}

void ExportSegyDlg::on_recvRbtn_clicked(bool checked)
{
    //共检波点
    if(checked){
        ui->ScopeGbx->setTitle("Reciever Scope:");
        ui->lineLbl->setText("Reciever Line :");
        ui->stationLbl->setText("Reciever Station:");
        ui->dataPathLbl->setText("Origin Data Path:");

        ui->textHeaderFrame->show();
        ui->gatherInfoGbx->show();
        adjustSize();


        updateScopeInfo();
    }
}

void ExportSegyDlg::on_shotRbtn_clicked(bool checked)
{
    if(checked){
        ui->ScopeGbx->setTitle("Shot Scope:");
        ui->lineLbl->setText("Shot Line :");
        ui->stationLbl->setText("Shot Station:");
        ui->dataPathLbl->setText("RecvSegy Data Path:");

        ui->textHeaderFrame->hide();
        ui->gatherInfoGbx->hide();
        adjustSize();
        updateScopeInfo();
    }
}

void ExportSegyDlg::on_fromLineSpx_valueChanged(int arg1)
{
    if(arg1>ui->toLineSpx->value()) {
        ui->toLineSpx->setValue(arg1);
    }
}

void ExportSegyDlg::on_toLineSpx_valueChanged(int arg1)
{
    if(arg1<ui->fromLineSpx->value()) {
        ui->fromLineSpx->setValue(arg1);
    }
}

void ExportSegyDlg::on_fromStaSpx_valueChanged(int arg1)
{
    if(arg1>ui->toStaSpx->value()) {
        ui->toStaSpx->setValue(arg1);
    }
}

void ExportSegyDlg::on_toStaSpx_valueChanged(int arg1)
{
    if(arg1<ui->fromStaSpx->value()) {
        ui->fromStaSpx->setValue(arg1);
    }
}

void ExportSegyDlg::on_dataPathBrwser_clicked()
{
    QString dataPath = QFileDialog::getExistingDirectory(this,"Select Origin Data Path"
                                                         ,QDir::currentPath());
    if(dataPath.isEmpty())
        return;
    ui->dataPathEdit->setText(dataPath);
}

void ExportSegyDlg::on_outputPathBrwser_clicked()
{
    QString dataPath = QFileDialog::getExistingDirectory(this,"Select Ouput Path"
                                                         ,QDir::currentPath());
    if(dataPath.isEmpty())
        return;
    ui->outputFilePathEdit->setText(dataPath);
}



void ExportSegyDlg::on_startBtn_clicked()
{
    //判断处理数据类型-共检波点/共炮
    if(ui->recvRbtn->isChecked())
        m_exportParameters.segyType = 0;
    else
        m_exportParameters.segyType = 1;

    //Basic Parameter 基本参数
    //job name
    if(ui->jobEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"warning","Input Job Name.");
        return;
    }
    m_exportParameters.jobName = ui->jobEdit->text().trimmed();
    //area
    if(ui->areasCbx->currentText().isEmpty())
    {
        QMessageBox::warning(this,"warning","Must choose a area.");
        return;
    }
    m_exportParameters.areaName = ui->areasCbx->currentText();

    //数据目录
    if(ui->dataPathEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"warning","Must select data path.");
        return;
    }
    if(!QDir(ui->dataPathEdit->text()).exists())
    {
        QMessageBox::warning(this,"warning","Origin Data Path  not exist");
        return;
    }
    m_exportParameters.dataPath = ui->dataPathEdit->text();

    //ouput Path
    if(ui->outputFilePathEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"warning","Input output file path.");
        return;
    }
    if(!QDir(ui->outputFilePathEdit->text()).exists()){
        QMessageBox::warning(this,"warning","Output file path not exists.");
        return;
    }
    m_exportParameters.outputPath = ui->outputFilePathEdit->text();

    //共检波点数据文件信息------------------
    if(m_exportParameters.segyType == 0){
        //trace len
        const QValidator *validator = ui->traceLenEdit->validator();
        int pos = 0;
        QString text = ui->traceLenEdit->text();
        if(validator->validate(text,pos) != QValidator::Acceptable)
        {
            QMessageBox::warning(this,"warning","Trace Len must be in 100~100000.");
            return;
        }
        m_exportParameters.traceNs = ui->traceLenEdit->text().toInt();
        m_exportParameters.ds      = ui->dsSpx->value();

        //shotlineFiles
        if(m_shotLineDlg->shotLineFiles().size()<1)
        {
            QMessageBox::warning(this,"warning","Need Select ShotLine Files.");
            return;
        }
        m_exportParameters.shotLineFiles = m_shotLineDlg->shotLineFiles();

        //text header
        m_exportParameters.textHeader = ui->textEdit->toPlainText();
    }
    //作业范围
    m_exportParameters.lineScope.first  = ui->fromLineSpx->value();
    m_exportParameters.lineScope.second = ui->toLineSpx->value();

    m_exportParameters.staScope.first  = ui->fromStaSpx->value();
    m_exportParameters.staScope.second = ui->toStaSpx->value();

    this->close();

    //start
    emit signalStartExport(m_exportParameters);
}

ShotLineDlg::ShotLineDlg(QDialog *parent)
    :QDialog(parent)
{
    this->setWindowTitle("ShotLine Files");
    initDlg();

    this->resize(240,360);
}

void ShotLineDlg::setShotLineFiles(const QStringList &fileList)
{
    m_shotLineFiles = fileList;
    listWidget->clear();
    listWidget->addItems(fileList);
}

bool ShotLineDlg::saveShotLineFiles(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream out(&file);
    foreach (QString shotFile, m_shotLineFiles) {
        out<<shotFile<<"\n";
    }
    file.close();
    return true;
}

void ShotLineDlg::initDlg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch(1);
    QPushButton *okBtn = new QPushButton("&Ok",this);
    btnLayout->addWidget(okBtn);
    btnLayout->addStretch(1);
    mainLayout->addLayout(btnLayout);
    //
    connect(okBtn,SIGNAL(clicked()),this,SLOT(close()));

    this->setLayout(mainLayout);
}


//参数控制面板-PlotParamDlg-----------------------------
PlotParamDlg::PlotParamDlg(QCPlotter *plotter,QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::PlotParamDlg)
    ,m_QCPlotter(plotter)
{
    ui->setupUi(this);
    this->setWindowTitle("QCPlot Option Dialog");
    initDlg();
    ui->tabWidget->setCurrentIndex(0);
}

void PlotParamDlg::initDlg()
{
    //WT/VA----------------------  //图像模式
    QButtonGroup *m_graphModeGrp = new QButtonGroup(this);
    m_graphModeGrp->addButton(ui->WTVArBtn,0);
    ui->WTVArBtn->setChecked(true);
    m_graphModeGrp->addButton(ui->WTrBtn,1);
    m_graphModeGrp->addButton(ui->VArBtn,2);

    //interpolation---------
    QButtonGroup *m_interpolatoinGrp= new QButtonGroup(this);
    m_interpolatoinGrp->addButton(ui->sincRBtn,2);
    ui->sincRBtn->setChecked(true);
    m_interpolatoinGrp->addButton(ui->linearRbtn,1);

    //brush
    ui->positiveCbx->setChecked(true);

    //gain
    ui->gainEdit->setValidator(new QDoubleValidator(0.0,1000.f,6,this));
    ui->gainEdit->setText("1.0");
    //trace inc
    ui->traceIncSpx->setRange(1,1000);
    ui->traceIncSpx->setValue(1);

    //Show header label
    ui->horiAxesLbl->addItems(showHeaderPosList);

    //Axes Min Length
    ui->horiAxesMinLenEdit->setRange(10,1000);
    ui->horiAxesMinLenEdit->setValue(50);
    ui->vertAxesMinLenEdit->setRange(5,1000);
    ui->vertAxesMinLenEdit->setValue(40);

    connect(m_graphModeGrp,SIGNAL(buttonToggled(int,bool)),this,SLOT(slotGraphModeChanged(int,bool)));
    connect(m_interpolatoinGrp,SIGNAL(buttonClicked(int)),this,SLOT(slotInterpolationChanged(int)));

    //start-end Time scope
    ui->startTimeEdit->setValidator(new QIntValidator(0,20000));
    ui->endTimeEdit->setValidator(new QIntValidator(0,20000));
}

void PlotParamDlg::on_positiveCbx_toggled(bool checked)
{
    //四种情况
    if(checked && ui->negativeCbx->isChecked())
        m_QCPlotter->setBrushMode(BrushBoth);
    else if(checked && !ui->negativeCbx->isChecked())
        m_QCPlotter->setBrushMode(BrushPositive);
    else if(!checked && ui->negativeCbx->isChecked())
        m_QCPlotter->setBrushMode(BrushNegative);
    else
    {
        ui->WTrBtn->setChecked(true);
        m_QCPlotter->setBrushMode(NoBrush);
    }
}

void PlotParamDlg::on_negativeCbx_toggled(bool checked)
{
    //四种情况-------------
    if(checked && ui->positiveCbx->isChecked())
        m_QCPlotter->setBrushMode(BrushBoth);
    else if(checked && !ui->positiveCbx->isChecked())
        m_QCPlotter->setBrushMode(BrushNegative);
    else if(!checked && ui->positiveCbx->isChecked())
        m_QCPlotter->setBrushMode(BrushPositive);
    else
    {
        ui->WTrBtn->setChecked(true);
        m_QCPlotter->setBrushMode(NoBrush);
    }
}

void PlotParamDlg::on_ScaleScopeCbx_currentIndexChanged(int index)
{
    if(index == 0)
        m_QCPlotter->setScaleScope(Global);
    else if(index == 1)
        m_QCPlotter->setScaleScope(Trace);
}

void PlotParamDlg::on_gainEdit_textChanged(const QString &arg1)
{
    if(arg1.trimmed().isEmpty())
        return;
    float value = arg1.trimmed().toFloat();
    value = value<0?10e-6:value;
    value = 1.0/value;
    m_QCPlotter->setGain(value);
}

void PlotParamDlg::on_okBtn_clicked()
{
    this->close();
}

void PlotParamDlg::on_horiAxesLbl_currentIndexChanged(const QString &headField)
{
    TraceHead      traceHead;
    char*          pos0 = (char*)&traceHead;
    HeaderFieldPos headerFieldPos;
    headerFieldPos.fieldName = headField;
    if(headField == "Trace No.")
    {
        //道号tracr
        headerFieldPos.pos = (char*)&traceHead.tracr-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "FFID")
    {
        headerFieldPos.pos   = (char*)&traceHead.fldr-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "TraceSeqFFID")
    {
        headerFieldPos.pos = (char*)&traceHead.tracf-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "TraceSeqLine")
    {
        headerFieldPos.pos = (char*)&traceHead.tracl-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "TraceSeqReel")
    {
        headerFieldPos.pos = (char*)&traceHead.tracr-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "CDP")
    {
        headerFieldPos.pos = (char*)&traceHead.cdp-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "OFFSET")
    {
        headerFieldPos.pos = (char*)&traceHead.offset-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Source Point")
    {
        headerFieldPos.pos = (char*)&traceHead.ep-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Source X")
    {
        headerFieldPos.pos = (char*)&traceHead.sx-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Source Y")
    {
        headerFieldPos.pos = (char*)&traceHead.sy-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Receiver X")
    {
        headerFieldPos.pos = (char*)&traceHead.gx-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Receiver Y")
    {
        headerFieldPos.pos   = (char*)&traceHead.gy-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Receiver Line")
    {
        headerFieldPos.pos   = (char*)&traceHead.unass[2]-pos0;
        headerFieldPos.dType = Int_Type;
    }
    else if(headField == "Receiver No.")
    {
        headerFieldPos.pos   = (char*)&traceHead.unass[3]-pos0;
        headerFieldPos.dType = Int_Type;
    }

    m_QCPlotter->setShowHeaderFielPos(headerFieldPos);
}

void PlotParamDlg::on_vertAxesLbl_currentIndexChanged(int index)
{
    if(index == 0)
        m_QCPlotter->setVertScaleType(ScaleTime);
    if(index == 1)
        m_QCPlotter->setVertScaleType(ScaleDepth);
}

void PlotParamDlg::on_horiAxesMinLenEdit_valueChanged(int arg1)
{
    m_QCPlotter->setMinAxesLength(Qt::Horizontal,arg1);
}

void PlotParamDlg::on_vertAxesMinLenEdit_valueChanged(int arg1)
{
    m_QCPlotter->setMinAxesLength(Qt::Vertical,arg1);
}

void PlotParamDlg::on_showHoriGridCbx_clicked(bool checked)
{
    if(checked)
        m_QCPlotter->setGridShow(HorizontalGrid);
    else
        m_QCPlotter->setGridShow(NoneGrid);
}

void PlotParamDlg::on_traceIncSpx_valueChanged(int arg1)
{
    m_QCPlotter->setTraceInc(arg1);
}

void PlotParamDlg::slotGraphModeChanged(int mode,bool checked)
{
    if(checked){
        if(mode == 0){
            m_QCPlotter->setGraphMode(VAWG);
            //保证笔刷选中其一
            if(!ui->positiveCbx->isChecked() && !ui->negativeCbx->isChecked())
            {
                ui->positiveCbx->setChecked(true);
            }
        }
        else if(mode == 1)
            m_QCPlotter->setGraphMode(WG);
        else if(mode == 2){
            m_QCPlotter->setGraphMode(VA);
            //保证笔刷选中其一
            if(!ui->positiveCbx->isChecked() && !ui->negativeCbx->isChecked())
            {
                ui->positiveCbx->setChecked(true);
            }
        }
    }
}

void PlotParamDlg::slotInterpolationChanged(int interType)
{
    m_QCPlotter->setInterType(interType);
}

void PlotParamDlg::on_startTimeEdit_editingFinished()
{
    int startTime = ui->startTimeEdit->text().toInt();
    int endTime   = ui->endTimeEdit->text().toInt();
    if(startTime>endTime)
        return;
    m_QCPlotter->setFixedTimeScope(QPair<int,int>(startTime,endTime));
}

void PlotParamDlg::on_endTimeEdit_editingFinished()
{
    int startTime = ui->startTimeEdit->text().toInt();
    int endTime   = ui->endTimeEdit->text().toInt();
    if(startTime>endTime)
        return;
    m_QCPlotter->setFixedTimeScope(QPair<int,int>(startTime,endTime));
}

//FileDetailDlg------------------
FileDetailDlg::FileDetailDlg(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Segy Header Detail Dialog");
    initDlg();
    this->resize(570,724);
}

void FileDetailDlg::initDlg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QTabWidget *tabWg = new QTabWidget(this);

    //text header
    m_txtHeaderBrser = new QTextBrowser(this);
    tabWg->insertTab(0,m_txtHeaderBrser,"Text Header");

    //binary header
    m_binHeaderTab = new QTableWidget(this);
    m_binHeaderTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_binHeaderTab->setColumnCount(3);
    m_binHeaderTab->setHorizontalHeaderLabels(
                QStringList()<<"Field"
                <<"Value"
                <<"Description");
    m_binHeaderTab->setAlternatingRowColors(true);
    QStringList binaryHeaderInfos;
    getBinaryHeaderInfos(binaryHeaderInfos);
    m_binHeaderTab->setRowCount(binaryHeaderInfos.size()/2);
    //m_binHeaderTab->setColumnWidth(0,);
    for(int i =0;i<binaryHeaderInfos.size()/2;i++)
    {
        m_binHeaderTab->setItem(i,0,new QTableWidgetItem(binaryHeaderInfos[i*2]));
        m_binHeaderTab->setItem(i,2,new QTableWidgetItem(binaryHeaderInfos[i*2+1]));
    }
    m_binHeaderTab->horizontalHeader()->setStretchLastSection(true);
    tabWg->insertTab(1,m_binHeaderTab,"Text Header");

    mainLayout->addWidget(tabWg);
    mainLayout->addWidget(new GLine(this));
    QPushButton *okBtn = new QPushButton(tr("&Ok"));
    okBtn->setMaximumWidth(44);
    mainLayout->addWidget(okBtn,0,Qt::AlignHCenter);
    this->setLayout(mainLayout);

    connect(okBtn,SIGNAL(clicked()),this,SLOT(close()));
}


void FileDetailDlg::setSegyFileInfo(char *textHeader,BinaryHead *header,SegyInfo */*segyInfo*/)
{
    //text header------
    m_txtHeaderBrser->setText(QString(textHeader));

    //binary header----
    m_binHeaderTab->setItem(0,1,new QTableWidgetItem(QString::number(header->jobid)));
    m_binHeaderTab->setItem(1,1,new QTableWidgetItem(QString::number(header->lineno)));
    m_binHeaderTab->setItem(2,1,new QTableWidgetItem(QString::number(header->reelno)));
    m_binHeaderTab->setItem(3,1,new QTableWidgetItem(QString::number(header->ntrace)));
    m_binHeaderTab->setItem(4,1,new QTableWidgetItem(QString::number(header->naux)));
    m_binHeaderTab->setItem(5,1,new QTableWidgetItem(QString::number(header->mudt)));
    m_binHeaderTab->setItem(6,1,new QTableWidgetItem(QString::number(header->omudt)));
    m_binHeaderTab->setItem(7,1,new QTableWidgetItem(QString::number(header->nt)));
    m_binHeaderTab->setItem(8,1,new QTableWidgetItem(QString::number(header->ont)));
    m_binHeaderTab->setItem(9,1,new QTableWidgetItem(QString::number(header->samptype)));
    //10-----------------------------------------
    m_binHeaderTab->setItem(10,1,new QTableWidgetItem(QString::number(header->fold)));
    m_binHeaderTab->setItem(11,1,new QTableWidgetItem(QString::number(header->sort)));
    m_binHeaderTab->setItem(12,1,new QTableWidgetItem(QString::number(header->sums)));
    m_binHeaderTab->setItem(13,1,new QTableWidgetItem(QString::number(header->sweep0)));
    m_binHeaderTab->setItem(14,1,new QTableWidgetItem(QString::number(header->sweepf)));
    m_binHeaderTab->setItem(15,1,new QTableWidgetItem(QString::number(header->sweepl)));
    m_binHeaderTab->setItem(16,1,new QTableWidgetItem(QString::number(header->sweept)));
    m_binHeaderTab->setItem(17,1,new QTableWidgetItem(QString::number(header->sweeptr)));
    m_binHeaderTab->setItem(18,1,new QTableWidgetItem(QString::number(header->taper0)));
    m_binHeaderTab->setItem(19,1,new QTableWidgetItem(QString::number(header->taperf)));
    //20-----------------------------------------
    m_binHeaderTab->setItem(20,1,new QTableWidgetItem(QString::number(header->tapert)));
    m_binHeaderTab->setItem(21,1,new QTableWidgetItem(QString::number(header->cor)));
    m_binHeaderTab->setItem(22,1,new QTableWidgetItem(QString::number(header->gain)));
    m_binHeaderTab->setItem(23,1,new QTableWidgetItem(QString::number(header->recov)));
    m_binHeaderTab->setItem(24,1,new QTableWidgetItem(QString::number(header->units)));
    m_binHeaderTab->setItem(25,1,new QTableWidgetItem(QString::number(header->pol)));
    m_binHeaderTab->setItem(26,1,new QTableWidgetItem(QString::number(header->vibpol)));
    //27-----------------------------------------

}

void FileDetailDlg::getBinaryHeaderInfos(QStringList &fieldAndDesc)
{
    fieldAndDesc<<"jobid"<<"Job identification number"
               <<"lineno"<<"Line number"
              <<"reelno"<<"Tape reel number"
             <<"ntrace"<<"number of data traces per record"
            <<"naux"<<"number of auxiliary traces per record"
           <<"mudt"<<"sample interval in micro secs for this reel"
          <<"omudt"<<"sample interval of original field data"
         <<"nt"<<"number of samples per data trace here"
        <<"ont"<<"number of samples per original field data trace"
       <<"samptype"<<"data sample format code"
         //10------------------------
      <<"fold"<<"expected CDP fold"
     <<"sort"<<"Trace sorting code"
    <<"sums"<<"Vertical sum code (how many summed)"
    <<"sweep0"<<"Sweep frequency start, Hz"
    <<"sweepf"<<"Sweep frequency end, Hz"
    <<"sweepl"<<"Sweep length, milliseconds"
    <<"sweept"<<"Sweep type code"
    <<"sweeptr"<<"Trace number of sweep channel"
    <<"taper0"<<"Sweep trace taper length at start, milliseconds"
    <<"taperf"<<"Sweep trace taper length at end, milliseconds"
      //20------------------------
    <<"tapert"<<"Sweep taper type "
    <<"cor"<<"Correlated data traces "
    <<"gain"<<"Binary gain recovered "
    <<"recov"<<"Gain recovered method code"
    <<"units"<<"Measurement system units"
    <<"pol"<<"Impulse signal polarity"
    <<"vibpol"<<"Vibratory polarity code"
      //27------------------------
      ;
}





