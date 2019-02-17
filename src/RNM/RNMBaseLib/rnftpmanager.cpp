#include "rnftpmanager.h"
#include "ui_rnftpmanager.h"

#define   Ftp_Manager_Column_Count  6
#define   Ftp_Column_DevNo          0
#define   Ftp_Column_State          1
#define   Ftp_Column_Memory         2
#define   Ftp_Column_DownLoad       3
#define   Ftp_Column_SampleFrequency    4
#define   Ftp_Column_Note           5

#include <QMessageBox>
#include <QEvent>
#include <QPainter>
#include <QFileDialog>
#include <QDateTimeEdit>

FtpTableModel::FtpTableModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void FtpTableModel::setNodes(QVector<Node *> &nodes)
{
    beginResetModel();
    m_nodes = nodes;
    //    m_selecteds.resize(m_nodes.size());
    m_selecteds = QVector<bool>(m_nodes.size(),false);
    endResetModel();
}

int FtpTableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_nodes.size();
}

int FtpTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return Ftp_Manager_Column_Count;
}

QVariant FtpTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int rw = index.row();
    int col = index.column();


    //设置使能标识
    if(role == Qt::UserRole)
        return m_nodes[rw]->state();

    if(col == Ftp_Column_DevNo){
        if(role == Qt::DisplayRole)
            return QString("G%1").arg(m_nodes[rw]->no(),3,10,QChar('0'));
        if(role == Qt::EditRole)
            return m_selecteds[rw];
    }
    if(col == Ftp_Column_State){
        if(role == Qt::DisplayRole)
            return  m_nodes[rw]->state();
    }
    if(col == Ftp_Column_Memory){
        if(role == Qt::DisplayRole)
            return  m_nodes[rw]->memory();
        if(role == Qt::TextAlignmentRole)
            return Qt::AlignHCenter;
    }
    if(col == Ftp_Column_DownLoad){
        if(role == Qt::DisplayRole)
            return  m_nodes[rw]->downloaded();
    }
    if(col == Ftp_Column_SampleFrequency){
        if(role == Qt::DisplayRole)
            return  m_nodes[rw]->sampleFrequency();
        if(role == Qt::TextAlignmentRole)
            return Qt::AlignHCenter;
    }

    if(col == 5){
        if(role == Qt::DisplayRole)
            return  m_nodes[rw]->infoString();
    }

    return QVariant();
}

bool FtpTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    if(index.column() == Ftp_Column_DevNo)
    {
        if(role == Qt::EditRole){
            m_selecteds[index.row()] = value.toBool();
            emit dataChanged(index,index);
            return true;
        }
    }
    return true;
}


QVariant FtpTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            if(section == Ftp_Column_DevNo)
                return "Node";
            if(section == Ftp_Column_State)
                return "State\n(Connected)";
            if(section == Ftp_Column_Memory)
                return "Memory Capacity\n(GB)";
            if(section == Ftp_Column_DownLoad)
                return "DownLoad\n(%)";
            if(section == Ftp_Column_SampleFrequency)
                return "Sample Frequency\n(Hz)";
            if(section == Ftp_Column_Note)
                return "Note";
        }
    }

    return QVariant();
}

Qt::ItemFlags FtpTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    //    int rw = index.row();
    //    if(m_nodes[rw]->state())
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    //    return Qt::NoItemFlags;
}

//QModelIndex FtpTableModel::index(int row, int column, const QModelIndex &parent) const
//{
//parent.internalPointer()
//}

//FtpSortProxyModel----------------------------------
FtpSortProxyModel::FtpSortProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

bool FtpSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (!left.isValid() || !right.isValid())
        return false;

    int col = left.column();
    QVariant leftData = sourceModel()->data(left, Qt::DisplayRole);
    QVariant rightData = sourceModel()->data(right, Qt::DisplayRole);

    if(col == Ftp_Column_State)
    {
        int val_left = leftData.toBool()?0:1;
        int val_right = rightData.toBool()?0:1;
        return val_left<val_right;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}



FtpItemDelegate::FtpItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

void FtpItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;
    int col = index.column();

    bool enable = index.model()->data(index,Qt::UserRole).toBool();
    if(col == 0)
    {
        QString dev_no = index.model()->data(index).toString();
        bool    checked = index.model()->data(index,Qt::EditRole).toBool();
        QStyleOptionButton *checkBoxOption = new QStyleOptionButton();
        if(enable)
            checkBoxOption->state |= QStyle::State_Enabled;
        if(checked)
            checkBoxOption->state |= QStyle::State_On;
        else
            checkBoxOption->state |= QStyle::State_Off;
        checkBoxOption->text = dev_no;
        checkBoxOption->rect = option.rect;
        QApplication::style()->drawControl(QStyle::CE_CheckBox,checkBoxOption,painter);
    }
    else if(col == 1)
    {
        QColor color;
        if(enable)
            color =QColor(Qt::green);
        else
            color = QColor(Qt::lightGray);
        painter->setBrush(color);

        painter->drawEllipse(option.rect.center(),5,5);
    }
    //进度
    else if(col == 3)
    {
        int progress = index.model()->data(index).toInt();
        QStyleOptionProgressBar *progressBarOpt = new QStyleOptionProgressBar();
        progressBarOpt->minimum = 0;
        progressBarOpt->maximum = 100;
        progressBarOpt->progress = progress;
        progressBarOpt->textVisible = true;
        progressBarOpt->text        = QString("%1%").arg(progress);
        progressBarOpt->rect = option.rect;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,progressBarOpt,painter);
    }
    else
        QStyledItemDelegate::paint(painter,option,index);
}


bool FtpItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        bool enable = index.model()->data(index,Qt::UserRole).toBool();
        if(index.column() == 0 && enable){
            bool data = model->data(index,Qt::EditRole).toBool();
            model->setData(index,!data,Qt::EditRole);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event,model,option,index);
}

RNFtpManager::RNFtpManager(QWidget *parent) :
    QDialog(parent)
  ,ui(new Ui::RNFtpManager)
  ,m_ftpWork(NoneWork)
{
    ui->setupUi(this);
    this->setWindowTitle("Node Ftp Manager");
    initDlg();

    m_stoped           = true;
    m_nodeNeedSelected = true;

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableModel     = new FtpTableModel(this);
    m_sortModel      = new FtpSortProxyModel(this);
    m_sortModel->setSourceModel(m_tableModel);
    FtpItemDelegate  *itemDelegate = new FtpItemDelegate(this);
    ui->tableView->setModel(m_sortModel);

    ui->tableView->setItemDelegate(itemDelegate);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSortingEnabled(true);
    m_sortModel->sort(Ftp_Column_State);

    //resize
    ui->tableView->setColumnWidth(2,140);
    ui->tableView->setColumnWidth(3,240);
    ui->tableView->setColumnWidth(4,160);
    m_maxWorkNodes = ui->spinBox->value();
    m_dataPath  = ui->datapathEdit->text().trimmed();
    m_sampleFrequency = ui->comboBox->currentText().toInt();

    //设置下载时间
    m_fromTimeEdit->setDateTime(QDateTime::currentDateTime());
    m_toTimeEdit->setDateTime(QDateTime::currentDateTime());

    m_downloadType = DownloadAll;
}

RNFtpManager::~RNFtpManager()
{
    delete ui;
}


void RNFtpManager::initDlg()
{
    ui->TClineEdit->setReadOnly(true);
    //timePeriod-------------------
    QHBoxLayout *timePeriodLayout = new QHBoxLayout;
    timePeriodLayout->setContentsMargins(2,0,2,0);
    timePeriodLayout->setSpacing(4);
    QLabel *fromLbl = new QLabel("From:");
    m_fromTimeEdit = new QDateTimeEdit;
    m_fromTimeEdit->setDisplayFormat("MM/dd/yyyy HH:mm:ss");

    QLabel *toLbl = new QLabel("To:");
    m_toTimeEdit = new QDateTimeEdit;
    m_toTimeEdit->setDisplayFormat("MM/dd/yyyy HH:mm:ss");

    timePeriodLayout->addWidget(fromLbl);
    timePeriodLayout->addWidget(m_fromTimeEdit);
    timePeriodLayout->addWidget(toLbl);
    timePeriodLayout->addWidget(m_toTimeEdit);
    ui->timePeriodFrame->setLayout(timePeriodLayout);

    ui->timePeriodFrame->setEnabled(false);

    ui->spinBox->setValue(QThread::idealThreadCount()-1);

    ui->splitter->setStretchFactor(0,3);
    ui->splitter->setStretchFactor(1,3);
    ui->splitter->setSizes(QList<int>()<<height()*3/4<<height()*1/4);
    ui->textBrowser->resize(ui->textBrowser->width(),80);
}

void RNFtpManager::setProjectInfo(const ProjectInfo &projInfo)
{
    m_projInfo = projInfo;
    //设置文件目录
    int ok  = -1;
    QString errStr;
    QString dataPath = Project::getProjectInfo(projInfo,Json_Key_Project_Data_Path
                                               ,&ok,&errStr).toString();
    if(ok != 0)
        QMessageBox::critical(this,"Error"
                              ,QString("Get Data Path Error,ok =%1").arg(ok));
    ui->datapathEdit->setText(dataPath);
    m_dataPath = dataPath;
}

void RNFtpManager::setNodesInfo(const QVector<Node *> &nodes)
{
    m_nodes = nodes;

    //更新数据模型
    m_tableModel->setNodes(m_nodes);

    //开始查询服务器文件，查询完毕后根据本地文件设置下载进度
    refreshNodes();
}

void RNFtpManager::getMessage(Node *node,const UpdateCmd &msg)
{
    int rw = m_nodes.indexOf(node);
    //更新显示项内容------------------------------------
    //全部更新
    if(msg == Ftp_Update || msg == All_Update)
    {
        for(int i =0;i<m_tableModel->columnCount();i++)
            ui->tableView->update(m_sortModel->mapFromSource(m_tableModel->index(rw,i)));
    }
    //更新容量信息
    else if(msg == Ftp_Update_Memory)
    {
        ui->tableView->update(m_sortModel->mapFromSource(m_tableModel->index(rw,Ftp_Column_Memory)));
        //更新本地下载文件记录
        qDebug()<<"Node "<<QString("G%1").arg(node->no(),3,10,QChar('0'))
               <<"udpate memory and update local files-----";
    }
    //更新下载进度
    else if(msg == Ftp_Update_Progress)
    {
        ui->tableView->update(m_sortModel->mapFromSource(m_tableModel->index(rw,Ftp_Column_DownLoad)));
    }
    //更新采样率
    else if(msg == Ftp_Update_SampleFrequency)
    {
        ui->tableView->update(m_sortModel->mapFromSource(m_tableModel->index(rw,Ftp_Column_SampleFrequency)));
    }
    //Note 提示
    else if(msg == Ftp_Update_Note)
    {
        ui->tableView->update(m_sortModel->mapFromSource(m_tableModel->index(rw,Ftp_Column_Note)));
    }
}

void RNFtpManager::finishedFtpWork(Node *node,const int &ftpWk,const bool &err)
{
    Q_UNUSED(err);
    if(err){
        ui->textBrowser->append(QString("G%1").arg(node->no(),3,10,QChar('0'))
                                +" "+getWorkName(m_ftpWork)+" Error");
    }

    if(ftpWk == DownLoadFiles){
        if(!node->isDownloaded()){
            //下载完成，记录到本地文件
            Project::appendDownloadGOBS(m_projInfo,
                                        QString("G%1").arg(node->no(),3,10,QChar('0')));
            node->setDownloaded(true);
        }
    }

    m_working_nodes.removeOne(node);

    //执行参数
    QVariant arg;
    if(ftpWk == Updating)
        arg = m_dataPath;
    else if(ftpWk == SetSampleFrequecy)
        arg = m_sampleFrequency;
    else if(ftpWk == DownLoadFiles)
        arg = int(m_downloadType);

    if(!m_stoped) //如果没有停止，则继续执行
        startNextNode(m_ftpWork,arg);

    //全部执行完毕,或者停止
    if(m_working_nodes.isEmpty()){

        m_working_nodes.clear();
        if(m_stoped)
        {
            m_stoped = false;
            ui->textBrowser->append("stopped "+getWorkName(m_ftpWork)
                                    +" ---------------------------------------");
        }
        else
            ui->textBrowser->append("finish "+getWorkName(m_ftpWork)
                                    +"----------------------------------------");
        m_ftpWork = NoneWork;
        //setUiBtnEnable(true);
    }
}

void RNFtpManager::on_downloadBtn_clicked()
{
    //判断是否按照时间段下载
    if(ui->timePeriodCbx->isChecked())
    {
        m_downloadType  = DownloadInPeriod;
        //判断时间是否合理
        QDateTime fromTime = m_fromTimeEdit->dateTime();
        QDateTime toTime   = m_toTimeEdit->dateTime();
        if(fromTime>toTime)
        {
            QMessageBox::warning(this,"warning"
                                 ,"Download from time is later than to time.");
            return;
        }
        if(ui->TClineEdit->text().isEmpty())
        {
            QMessageBox::warning(this,"warning"
                                 ,"Input TC Value.");
            return;
        }
        qDebug()<<"start time:"<<fromTime.toString("yyyy-MM-dd HH:mm:ss")
               <<"to time:"<<toTime.toString("yyyy-MM-dd HH:mm:ss");
        setDownloadPeriod(fromTime,toTime);
        setTcValue(ui->TClineEdit->text().trimmed().toDouble());
    }else
        m_downloadType  = DownloadAll;

    //选中节点
    m_nodeNeedSelected  =true;
    startFtpWork(DownLoadFiles,(int)m_downloadType);
}

void RNFtpManager::on_clearMemBtn_clicked()
{
    //选中节点
    m_nodeNeedSelected  = true;

    QMessageBox::StandardButton button = QMessageBox::warning(this,"Confirm","Are you sure to clear the GOBS?"
                                                              ,QMessageBox::Yes|QMessageBox::Cancel);

    if(button != QMessageBox::Yes)
        return;

    startFtpWork(ClearMemory);
}

void RNFtpManager::on_setSampleFreqBtn_clicked()
{
    //选中节点
    m_nodeNeedSelected  = true;

    //采样率---------------------------
    startFtpWork(SetSampleFrequecy,m_sampleFrequency);
}

void RNFtpManager::on_refreshBtn_clicked()
{
    refreshNodes();
}

void RNFtpManager::on_closeBtn_clicked()
{
    this->close();
}



void RNFtpManager::on_brwserBtn_clicked()
{
    //查找对应路径
    QString newPath = QFileDialog::getExistingDirectory(this,"Select Data Path",
                                                        m_dataPath);
    if(newPath.isEmpty())
        return;

    ui->datapathEdit->setText(newPath);
    m_dataPath = newPath;

    //pro 文件更新 dataPath
    Project::updateProjectInfo(m_projInfo,Json_Key_Project_Data_Path,m_dataPath);
}


QString  RNFtpManager::getWorkName(const FtpWork &work) const
{
    if(work == NoneWork)
        return "None Work";
    else if(work == Updating)
        return "Updating";
    else if(work == DownLoadFiles)
        return "Downloading Files";
    else if(work == ClearMemory)
        return "Clearing Files";
    else if(work == SetSampleFrequecy)
        return "Setting Sample Frequency";
    else
        return "";
}


void RNFtpManager::refreshNodes()
{
    //选中节点
    m_nodeNeedSelected  = false;

    //更新---
    startFtpWork(Updating,m_dataPath);
}

void RNFtpManager::on_checkBox_clicked(bool checked)
{
    //全选或者清除
    for(int i = 0;i<m_tableModel->rowCount();i++)
    {
        if(m_nodes[i]->state())
            m_sortModel->setData(
                        m_sortModel->mapFromSource(m_tableModel->index(i,0))
                        ,checked);
    }
}

void RNFtpManager::on_spinBox_valueChanged(int arg1)
{
    m_maxWorkNodes = arg1;
}

void RNFtpManager::on_stopCurrentBtn_clicked()
{
    //当前无任务
    if(m_stoped || m_ftpWork == NoneWork)
        return;
    //完成当前节点则不再
    m_stoped = true;
    ui->textBrowser->append("Stopping Current Work....");

    //如果当前为下载,完成当前节点比较耗时，
    //需要手动停止下载线程
    foreach (Node *node, m_working_nodes) {
        node->stopFtpWork();
    }
}


void RNFtpManager::on_timePeriodCbx_toggled(bool checked)
{
    ui->timePeriodFrame->setEnabled(checked);
}

void RNFtpManager::on_comboBox_currentTextChanged(const QString &arg1)
{
    m_sampleFrequency = arg1.toInt();
}

QList<Node *> RNFtpManager::regetWaitingNodes(const bool &needSelected)
{
    QList<Node *> watingNodes;
    bool checked;
    bool state;
    //1 需要选中
    if(needSelected){
        for(int i=0;i<m_tableModel->rowCount();i++)
        {
            checked = m_tableModel->index(i,0).data(Qt::EditRole).toBool();
            state   = m_tableModel->index(i,0).data(Qt::UserRole).toBool();
            if(state && checked)
                watingNodes.append(m_nodes[i]);
        }
    }
    else //所有使能的节点都工作
    {
        foreach(Node *node, m_nodes) {
            if(node->state())
            {
                watingNodes.append(node);
            }
        }
    }
    return watingNodes;
}


void RNFtpManager::startFtpWork(const FtpWork &ftpWk,const QVariant &arg)
{
    if(m_ftpWork != NoneWork)
    {
        QMessageBox::warning(this,"warning","Ftp is busy for " + getWorkName(m_ftpWork));
        return;
    }
    //选中，并且状态enable 的节点,下载数据
    m_waiting_nodes = regetWaitingNodes(m_nodeNeedSelected);

    if(m_waiting_nodes.size()<1)
        return;

    //更新工作状态
    m_ftpWork = ftpWk;

    m_working_nodes.clear();
    m_stoped = false;

    ui->textBrowser->append("start "+getWorkName(m_ftpWork)+"...");

    //更新等待状态
    foreach (Node *node, m_waiting_nodes) {
        node->setInfoString("Waiting");
        getMessage(node,Ftp_Update_Note);
    }

    //取出节点到队列列
    for(int i = 0;i< m_maxWorkNodes;i++)
    {
        if(!m_waiting_nodes.isEmpty()){
            m_working_nodes.append(m_waiting_nodes.takeFirst());
        }
    }

    //    qDebug()<<"max nodes:"<<m_maxWorkNodes
    //           <<"waiting:"<<m_waiting_nodes.size()
    //          <<"working:"<<m_working_nodes.size();
    //开始下载
    foreach (Node *node, m_working_nodes) {
        node->startFtpWork(ftpWk,arg);
    }
}

void RNFtpManager::startNextNode(const FtpWork &ftpWk,const QVariant &arg)
{
    if(!m_waiting_nodes.isEmpty()){
        Node *node = m_waiting_nodes.takeFirst();
        m_working_nodes.append(node);
        node->startFtpWork(ftpWk,arg);
    }
}

void RNFtpManager::setUiBtnEnable(const bool &enable)
{
    ui->refreshBtn->setEnabled(enable);
    ui->downloadBtn->setEnabled(enable);
    ui->setSampleFreqBtn->setEnabled(enable);
    ui->clearMemBtn->setEnabled(enable);
}

void RNFtpManager::on_clearBtn_clicked()
{
    ui->textBrowser->clear();
}
