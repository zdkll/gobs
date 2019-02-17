#include "rndeployments.h"
#include "ui_rndeployments.h"
#include "gsqlfunctions.h"
#include "gpublicwidgets.h"

#define Max_Coord_Value  1e8
#define DateTime_Format  "MM/dd/yyyy hh:mm:ss.zzz"

#define Table_Column_Device         0
#define Table_Column_DevIP          1
#define Table_Column_Area           2
#define Table_Column_Line           3
#define Table_Column_Station        4
#define Table_Column_DepolyTime     5
#define Table_Column_PickedupTime   6
#define Table_Column_SPSX           7
#define Table_Column_SPSY           8
#define Table_Column_SPSZ           9
#define Table_Column_ActualX        10
#define Table_Column_ActualY        11
#define Table_Column_ActualZ        12
#define Table_Column_Depolyed       13
#define Table_Column_Pickedup       14
#define Table_Column_Download       15

#include <QAbstractItemView>
#include <QMessageBox>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QFileDialog>

#include "gcore.h"

EditCoordDlg::EditCoordDlg(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Enter new Coordinates");
    initDlg();

}

void EditCoordDlg::initDlg()
{
    QVBoxLayout *mainLayout = new  QVBoxLayout;

    QGridLayout *gridLayout = new  QGridLayout;
    QLabel *currentXLbl = new QLabel(tr("Current Easting(X):"));
    m_xCoordEdit = new QLineEdit(this);
    m_xCoordEdit->setValidator(new QDoubleValidator(0.0,Max_Coord_Value,6,this));
    gridLayout->addWidget(currentXLbl,0,0);
    gridLayout->addWidget(m_xCoordEdit,0,1);

    QLabel *currentYLbl = new QLabel(tr("Current Northing(Y):"));
    m_yCoordEdit = new QLineEdit(this);
    m_yCoordEdit->setValidator(new QDoubleValidator(0.0,Max_Coord_Value,6,this));
    gridLayout->addWidget(currentYLbl,1,0);
    gridLayout->addWidget(m_yCoordEdit,1,1);

    QLabel *currentZLbl = new QLabel(tr("Current Depth(Z):"));
    m_zCoordEdit = new QLineEdit(this);
    m_zCoordEdit->setValidator(new QDoubleValidator(0.0,Max_Coord_Value,6,this));
    gridLayout->addWidget(currentZLbl,2,0);
    gridLayout->addWidget(m_zCoordEdit,2,1);

    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(new GLine(this));

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch(1);

    QPushButton *okBtn = new QPushButton(tr("&Ok"));
    QPushButton *cancelBtn = new QPushButton(tr("&Cancel"));
    hLayout->addWidget(okBtn);
    hLayout->addWidget(cancelBtn);

    mainLayout->addLayout(hLayout);

    this->setLayout(mainLayout);

    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(slotOkBtnClicked()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(slotCancelBtnClicked()));
}

void EditCoordDlg::editCurrentCoord(const Coordinate &currentCord)
{
    m_xCoordEdit->setText(QString::number(currentCord.x,'f'));
    m_yCoordEdit->setText(QString::number(currentCord.y,'f'));
    m_zCoordEdit->setText(QString::number(currentCord.z,'f'));
    this->exec();
}

void EditCoordDlg::slotOkBtnClicked()
{
    if(m_xCoordEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,"warning","Input X coordinate.");
        return;
    }
    if(m_yCoordEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,"warning","Input Y coordinate.");
        return;
    }
    if(m_zCoordEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,"warning","Input Z coordinate.");
        return;
    }
    //检查参数
    int pos = 0;
    QString text = m_xCoordEdit->text();
    if(m_xCoordEdit->validator()->validate(text,pos) != QValidator::Acceptable)
    {
        QMessageBox::warning(this,"warning","X coordinate is not suitable.");
        return;
    }
    text = m_yCoordEdit->text();
    if(m_yCoordEdit->validator()->validate(text,pos) != QValidator::Acceptable)
    {
        QMessageBox::warning(this,"warning","Y coordinate is not suitable.");
        return;
    }
    text = m_zCoordEdit->text();
    if(m_zCoordEdit->validator()->validate(text,pos) != QValidator::Acceptable)
    {
        QMessageBox::warning(this,"warning","Z coordinate is not suitable.");
        return;
    }

    Coordinate cord;
    cord.x = m_xCoordEdit->text().trimmed().toFloat();
    cord.y = m_yCoordEdit->text().trimmed().toFloat();
    cord.z = m_zCoordEdit->text().trimmed().toFloat();
    emit editedCurrentCoord(cord);

    this->close();
}

void EditCoordDlg::slotCancelBtnClicked()
{
    this->close();
}

//----------------------------------------------
EditDataTimeDlg::EditDataTimeDlg(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle("Enter New Depolyment Time");
    initDlg();
}

void EditDataTimeDlg::editDataTime(const NodeTimeInfo &timeInfo)
{
    m_nodeTimeInfo = timeInfo;

    m_areaLbl->setText(m_nodeTimeInfo.areaName);
    m_rnIDLbl->setText(m_nodeTimeInfo.nodeDevice);
    m_datatimeEdit->setDateTime(m_nodeTimeInfo.dateTime);

    if(m_nodeTimeInfo.timeType == DepolyTime)
    {
        this->setWindowTitle("Enter New Depolyment Time");
    }
    else
    {
        this->setWindowTitle("Enter New Pickedup Time");
    }
    this->exec();
}

void EditDataTimeDlg::slotOkBtnClicked()
{
    if(!m_datatimeEdit->dateTime().isValid())
        return;
    //保存时间
    m_nodeTimeInfo.dateTime = m_datatimeEdit->dateTime();

    emit editedDataTime(m_nodeTimeInfo);
    this->close();
}
void EditDataTimeDlg::slotCancelBtnClicked()
{
    this->close();
}

void EditDataTimeDlg::initDlg()
{
    QVBoxLayout *mainLayout = new  QVBoxLayout;

    QGridLayout *gridLayout = new  QGridLayout;

    //area
    QLabel *areaLbl = new QLabel(tr("Area:"));
    m_areaLbl = new QLabel(this);
    gridLayout->addWidget(areaLbl,0,0);
    gridLayout->addWidget(m_areaLbl,0,1);

    //RNID
    QLabel *rnIdLbl = new QLabel(tr("RNID:"));
    m_rnIDLbl = new QLabel(this);
    gridLayout->addWidget(rnIdLbl,1,0);
    gridLayout->addWidget(m_rnIDLbl,1,1);

    //UTC Time
    QLabel *utcTimeLbl = new QLabel(tr("UTC Time:"));
    m_datatimeEdit = new QDateTimeEdit(this);
    m_datatimeEdit->setDisplayFormat(DateTime_Format);
    m_datatimeEdit->setCalendarPopup(true);
    m_datatimeEdit->setMinimumWidth(300);
    gridLayout->addWidget(utcTimeLbl,2,0);
    gridLayout->addWidget(m_datatimeEdit,2,1);

    gridLayout->setColumnStretch(1,1);
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(new GLine(this));

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch(1);

    QPushButton *okBtn = new QPushButton(tr("&Ok"));
    okBtn->setDefault(true);
    QPushButton *cancelBtn = new QPushButton(tr("&Cancel"));
    hLayout->addWidget(okBtn);
    hLayout->addWidget(cancelBtn);

    mainLayout->addLayout(hLayout);

    this->setLayout(mainLayout);

    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(slotOkBtnClicked()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(slotCancelBtnClicked()));

}

RNDeployments::RNDeployments(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RNDeployments)
{
    ui->setupUi(this);
    this->setWindowTitle("RN Depolyment Information");

    //table widget set
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setShowGrid(false);
    //ui->tableWidget->setGridStyle(Qt::NoPen);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setColumnCount(TABLE_DEVICE_COLUMN_COUNT);
    //ip 列隐藏
    ui->tableWidget->setColumnHidden(Table_Column_DevIP,true);

    //Time列宽度设置
    ui->tableWidget->setColumnWidth(Table_Column_DepolyTime,180);
    ui->tableWidget->setColumnWidth(Table_Column_PickedupTime,180);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                               <<TABLE_FIELD_DEVICE
                                               <<TABLE_FIELD_IP
                                               <<TABLE_FIELD_AREA
                                               <<TABLE_FIELD_LINE
                                               <<TABLE_FIELD_STATION
                                               <<TABLE_FIELD_DEPOLYTIME
                                               <<TABLE_FIELD_PICKUPTIME
                                               <<TABLE_FIELD_SPSX
                                               <<TABLE_FIELD_SPSY
                                               <<TABLE_FIELD_SPSZ
                                               <<TABLE_FIELD_ACTUALX
                                               <<TABLE_FIELD_ACTUALY
                                               <<TABLE_FIELD_ACTUALZ
                                               <<TABLE_FIELD_DEPOLYED
                                               <<TABLE_FIELD_PICKEDUP
                                               <<TABLE_FIELD_DOWNLOADED);

    m_editCordDlg     = 0;
    m_editDateTimeDlg = 0;
}

RNDeployments::~RNDeployments()
{
    delete ui;
    qDeleteAll(m_nodeDevices);
    m_nodeDevices.clear();
}

void RNDeployments::setCurrentArea(const QString &area)
{
    m_currentArea = area;
    initDepolyments();
}

void RNDeployments::closeCurrentArea()
{
    m_currentArea.clear();
    ui->tableWidget->clear();
}

void RNDeployments::setProjInfo(ProjectInfo projInfo)
{
    m_projInfo = projInfo;

    //查询项目中GOBS下载数据完成信息
    m_downloadedGOBS = Project::finishedDownloadGOBS(m_projInfo);
}

void RNDeployments::initDepolyments(bool updateDownload)
{
    //从数据库查询信息,更新表格
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    QList<Condition>  conds;
    bool ok;
    QSqlQuery query = sqlFunc->query(m_currentArea,conds,&ok);
    if(!ok)
    {
        QMessageBox::critical(this,"error","Query DataBase Error:"+sqlFunc->errorString());
        return;
    }
    //统计行数
    int rowCount = 0;
    while(query.next())
        rowCount ++;

    ui->tableWidget->setRowCount(rowCount);
    query.seek(-1);
    int row = 0;
    m_depolyedCount = 0;
    m_pickedUpCount = 0;
    qDeleteAll(m_nodeDevices);
    m_nodeDevices.resize(rowCount);

    while(query.next())
    {
        for(int col = 0;col<TABLE_DEVICE_COLUMN_COUNT;col++){
            ui->tableWidget->setItem(row,col,new QTableWidgetItem(query.value(col).toString()));
        }
        RemoteNode *remoteNode = new RemoteNode;
        remoteNode->device = query.value(Table_Column_Device).toString();
        remoteNode->area = query.value(Table_Column_Area).toString();
        remoteNode->line = query.value(Table_Column_Line).toFloat();
        remoteNode->station = query.value(Table_Column_Station).toInt();

        remoteNode->depolyedTime = query.value(Table_Column_DepolyTime).toString();
        remoteNode->pickedupTime = query.value(Table_Column_PickedupTime).toString();
        remoteNode->actualX = query.value(Table_Column_ActualX).toFloat();
        remoteNode->actualY = query.value(Table_Column_ActualY).toFloat();
        remoteNode->actualZ = query.value(Table_Column_ActualZ).toFloat();
        remoteNode->depolyed = query.value(Table_Column_Depolyed).toInt();
        remoteNode->pickedup = query.value(Table_Column_Pickedup).toInt();
        //是否下载完成
        remoteNode->downloaded = query.value(Table_Column_Download).toInt();;

        //统计投放和回收设备的站点数
        if(remoteNode->station>0)
        {
            if(remoteNode->depolyed == 1)
                m_depolyedCount ++;
            if(remoteNode->pickedup == 1)
                m_pickedUpCount ++;
        }
        m_nodeDevices[row] = remoteNode;
        row ++;
    }

    //需要更新下载信息
    if(updateDownload)
    {
        //查询项目中GOBS下载数据完成信息
        m_downloadedGOBS = Project::finishedDownloadGOBS(m_projInfo);
        int contain = 0;
        for(int i=0;i<m_nodeDevices.size();i++)
        {
            contain = m_downloadedGOBS.contains(m_nodeDevices[i]->device)?1:0;
            if(m_nodeDevices[i]->downloaded != contain)
            {
                m_nodeDevices[i]->downloaded = contain;
                ui->tableWidget->setItem(i,Table_Column_Download,
                                         new QTableWidgetItem(QString::number(contain)));
                //更新数据库
                QList<Condition>  conds;
                Condition cond;
                cond.fieldName = TABLE_FIELD_DEVICE;
                cond.fieldValue = m_nodeDevices[i]->device;
                conds.append(cond);

                //内容
                QList<ResultInfo> retInfos;
                ResultInfo  retInfo;
                retInfo.fieldName = TABLE_FIELD_DOWNLOADED;
                retInfo.fieldValue = contain;
                retInfos.append(retInfo);
                ok = sqlFunc->update(m_currentArea,conds,retInfos);
                if(!ok)
                {
                    QMessageBox::critical(this,"error","Update Downloaded GOBS failed:"+sqlFunc->errorString());
                    return;
                }
            }
        }
    }
}

RemoteNode *RNDeployments::findRemoteNode(const float &line,const int &station)
{
    foreach (RemoteNode *rnode, m_nodeDevices) {
        if(line == rnode->line && station == rnode->station)
            return rnode;
    }
    return 0;
}
void RNDeployments::on_deleteDepolysBtn_clicked()
{
    //删除数据库记录
    QList<QTableWidgetSelectionRange> selectionRange =
            ui->tableWidget->selectedRanges();

    if(selectionRange.size()>0)
    {
        QMessageBox box(QMessageBox::Information,"Notice",
                        "The remote node device will be removed,still continue?"
                        ,QMessageBox::Yes|QMessageBox::No);
        switch (box.exec()) {
        case QMessageBox::No:
            return;
            break;
        default:
            break;
        }
    }
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    QList<Condition>  conds;
    Condition cond;
    cond.fieldName = TABLE_FIELD_DEVICE;
    conds.append(cond);
    for(int i = 0;i<selectionRange.size();i++)
    {
        for(int j=selectionRange[i].topRow();j<=selectionRange[i].bottomRow();j++)
        {
            //
            conds[0].fieldValue = ui->tableWidget->item(j,0)->text();
            sqlFunc->deleteRecord(m_currentArea,conds);
        }
    }
    //重新更新显示
    initDepolyments();
    emit signalUpdateRNDepolyments();
}

void RNDeployments::on_editPickupTimeBtn_clicked()
{
    int row =  ui->tableWidget->currentRow();

    if(row<0)
        return;

    if(!m_editDateTimeDlg){
        m_editDateTimeDlg = new EditDataTimeDlg(this);
        connect(m_editDateTimeDlg,SIGNAL(editedDataTime(NodeTimeInfo)),this,
                SLOT(slotEditedDateTime(NodeTimeInfo)));
    }

    NodeTimeInfo   nodeTimeInfo;
    nodeTimeInfo.areaName = m_currentArea;
    nodeTimeInfo.nodeDevice = ui->tableWidget->item(row,Table_Column_Device)->text();
    nodeTimeInfo.timeType = PickedupTime;

    QString pickedTimeStr = ui->tableWidget->item(row,Table_Column_PickedupTime)->text();
    QDateTime t = QDateTime::fromString(pickedTimeStr,DateTime_Format);
    if(!t.isValid())
        t = QDateTime::currentDateTime();
    nodeTimeInfo.dateTime = t;

    m_editDateTimeDlg->editDataTime(nodeTimeInfo);
}

void RNDeployments::on_editDepoyTimeBtn_clicked()
{
    int row =  ui->tableWidget->currentRow();
    if(row<0)
        return;

    if(!m_editDateTimeDlg){
        m_editDateTimeDlg = new EditDataTimeDlg(this);
        connect(m_editDateTimeDlg,SIGNAL(editedDataTime(NodeTimeInfo)),this,
                SLOT(slotEditedDateTime(NodeTimeInfo)));
    }

    NodeTimeInfo   nodeTimeInfo;
    nodeTimeInfo.areaName = m_currentArea;
    nodeTimeInfo.nodeDevice = ui->tableWidget->item(row,Table_Column_Device)->text();
    nodeTimeInfo.timeType = DepolyTime;

    QString depolyedTimeStr = ui->tableWidget->item(row,Table_Column_DepolyTime)->text();
    QDateTime t = QDateTime::fromString(depolyedTimeStr,DateTime_Format);
    if(!t.isValid())
        t = QDateTime::currentDateTime();
    nodeTimeInfo.dateTime = t;

    m_editDateTimeDlg->editDataTime(nodeTimeInfo);
}

void RNDeployments::on_editCoordBtn_clicked()
{
    if(!m_editCordDlg){
        m_editCordDlg = new EditCoordDlg(this);
        connect(m_editCordDlg,SIGNAL(editedCurrentCoord(Coordinate)),this,SLOT(slotEditedCurrentCoord(Coordinate)));
    }

    //编辑当前坐标
    qDebug()<<"current row:"<< ui->tableWidget->currentRow();
    int row =  ui->tableWidget->currentRow();
    if(row<0)
        return;

    //先判断当前行对应线号和站点号是否为0，为零无法编辑
    int station = ui->tableWidget->item(row,Table_Column_Station)->text().toInt();
     if(station == 0)
     {
         QMessageBox::warning(this,"warning","The GOBS is not related to a Reciever Station.");
         return;
     }
    Coordinate cord;

    cord.x = ui->tableWidget->item(row,Table_Column_ActualX)->text().toFloat();
    cord.y = ui->tableWidget->item(row,Table_Column_ActualY)->text().toFloat();
    cord.z = ui->tableWidget->item(row,Table_Column_ActualZ)->text().toFloat();

    m_editCordDlg->editCurrentCoord(cord);
}

void RNDeployments::on_refreshBtn_clicked()
{
    //刷新
    initDepolyments();
}

void RNDeployments::on_closeBtn_clicked()
{
    this->close();
}


void RNDeployments::on_importDepolyBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select Import File",Core::currentDirPath,tr("DAT(*.dat)"));
    if(fileName.isEmpty())
        return;

    //
    importDepolyments(fileName);
}


void RNDeployments::on_exportSheetBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Export Save File",Core::currentDirPath,tr("Text(*.txt)"));
    if(fileName.isEmpty())
        return;
#ifdef Q_OS_LINUX
    fileName += ".txt";
#endif
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this,"Error","Open output file failed.");
        return;
    }
    QTextStream out(&file);
    int columnCount = ui->tableWidget->columnCount();
    for(int column =0;column<columnCount;column++)
    {
        //qDebug()<<ui->tableWidget->horizontalHeaderItem(column)->text();
        out<<ui->tableWidget->horizontalHeaderItem(column)->text()<<"\t";
    }
    out<<endl;
    qDebug()<<"write header labels finished";
    for(int row=0;row<ui->tableWidget->rowCount();row++){
        for(int column = 0;column<ui->tableWidget->columnCount();column++)
        {
            out<<ui->tableWidget->item(row,column)->text()<<"\t";
        }
        out<<endl;
    }

    file.close();
    QMessageBox::information(this,"inform",QString("Export the table into %1.").arg(fileName));
}

void RNDeployments::slotEditedCurrentCoord(const Coordinate &cord)
{
    int row =  ui->tableWidget->currentRow();

    //1 更新数据库
    QString device = ui->tableWidget->item(row,0)->text();
    QList<Condition> conds;
    Condition cond;
    cond.fieldName =  TABLE_FIELD_DEVICE;
    cond.fieldValue = device;
    conds.append(cond);

    //结果
    QList<ResultInfo> retInfos;
    ResultInfo retInfo;
    retInfo.fieldName  = TABLE_FIELD_ACTUALX;
    retInfo.fieldValue = cord.x;
    retInfos.append(retInfo);

    retInfo.fieldName  = TABLE_FIELD_ACTUALY;
    retInfo.fieldValue = cord.y;
    retInfos.append(retInfo);

    retInfo.fieldName  = TABLE_FIELD_ACTUALZ;
    retInfo.fieldValue = cord.z;
    retInfos.append(retInfo);

    SqlFunctions *sqlFunc = SqlFunctions::instance();
    bool ok = sqlFunc->update(m_currentArea,conds,retInfos);
    if(!ok)
    {
        QMessageBox::critical(this,"error","Update Database Table Error.");
        return;
    }

    //2 更新表显示
    ui->tableWidget->item(row,Table_Column_ActualX)->setText(QString::number(cord.x));
    ui->tableWidget->item(row,Table_Column_ActualY)->setText(QString::number(cord.y));
    ui->tableWidget->item(row,Table_Column_ActualZ)->setText(QString::number(cord.z));

    float line    = ui->tableWidget->item(row,Table_Column_Line)->text().toFloat();
    int   station = ui->tableWidget->item(row,Table_Column_Station)->text().toInt();


}

void RNDeployments::slotEditedDateTime(const NodeTimeInfo &nodeTimeInfo)
{
    //qDebug()<<nodeTimeInfo.dateTime;
    int row =  ui->tableWidget->currentRow();

    //时间转换成字符串
    QString dateTimeStr = nodeTimeInfo.dateTime.toString(DateTime_Format);

    SqlFunctions *sqlFunc = SqlFunctions::instance();

    //更新数据库
    QList<Condition> conds;
    Condition cond;
    cond.fieldName = TABLE_FIELD_DEVICE;
    cond.fieldValue = ui->tableWidget->item(row,Table_Column_Device)->text();
    conds.append(cond);

    QList<ResultInfo> retInfos;

    //更新投放还是回收
    if(nodeTimeInfo.timeType == DepolyTime)
    {
        ResultInfo  retInfo;
        retInfo.fieldName =TABLE_FIELD_DEPOLYTIME;
        retInfo.fieldValue = dateTimeStr;
        retInfos.append(retInfo);
        //更新投放状态为1
        retInfo.fieldName =TABLE_FIELD_DEPOLYED;
        retInfo.fieldValue = 1;
        retInfos.append(retInfo);
    }
    else if(nodeTimeInfo.timeType == PickedupTime)
    {
        ResultInfo  retInfo;
        retInfo.fieldName =TABLE_FIELD_PICKUPTIME;
        retInfo.fieldValue = dateTimeStr;
        retInfos.append(retInfo);
        //更新投放状态为1
        retInfo.fieldName =TABLE_FIELD_PICKEDUP;
        retInfo.fieldValue = 1;
        retInfos.append(retInfo);
    }

    bool ok = sqlFunc->update(m_currentArea,conds,retInfos);
    if(!ok)
    {
        QMessageBox::critical(this,"error","Update Database Table Error.");
        return;
    }
    //更新表格数据
    if(nodeTimeInfo.timeType == DepolyTime)
    {
        ui->tableWidget->item(row,Table_Column_DepolyTime)->setText(dateTimeStr);
        ui->tableWidget->item(row,Table_Column_Depolyed)->setText("1");
    }
    else if(nodeTimeInfo.timeType == PickedupTime)
    {
        ui->tableWidget->item(row,Table_Column_PickedupTime)->setText(dateTimeStr);
        ui->tableWidget->item(row,Table_Column_Pickedup)->setText("1");
    }
}

void RNDeployments::importDepolyments(const QString &fileName)
{
    //从文件读取设备和设备投放信息，更新数据库
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"warning","Open file failed.");
        return;
    }

    qDebug()<<"finished gobs:"<<m_downloadedGOBS;
    SqlFunctions *sqlFunc = SqlFunctions::instance();

    //当前处理一条线的情况
    QFileInfo     fileInfo(fileName);
    int line_no = lineNo(fileInfo.fileName());

    QTextStream in(&file);
    QStringList strList;
    int     no = 1;
    QString device;
    QList<Condition> conds;
    Condition        cond;
    cond.fieldName = TABLE_FIELD_DEVICE;
    conds.append(cond);
    bool ok = false;
    QList<ResultInfo> retInfos;
    ResultInfo   devInfo;
    devInfo.fieldName = TABLE_FIELD_DEVICE;
    retInfos.append(devInfo);
    ResultInfo   ipInfo;
    ipInfo.fieldName = TABLE_FIELD_IP;
    retInfos.append(ipInfo);
    ResultInfo   areaInfo;
    areaInfo.fieldName = TABLE_FIELD_AREA;
    retInfos.append(areaInfo);
    ResultInfo   lineInfo;
    lineInfo.fieldName = TABLE_FIELD_LINE;
    retInfos.append(lineInfo);
    ResultInfo   staInfo;
    staInfo.fieldName = TABLE_FIELD_STATION;
    retInfos.append(staInfo);

    ResultInfo   spsxInfo;
    spsxInfo.fieldName = TABLE_FIELD_SPSX;
    retInfos.append(spsxInfo);
    ResultInfo   spsyInfo;
    spsyInfo.fieldName = TABLE_FIELD_SPSY;
    retInfos.append(spsyInfo);
    ResultInfo   spszInfo;
    spszInfo.fieldName = TABLE_FIELD_SPSZ;
    retInfos.append(spszInfo);

    ResultInfo   axInfo;
    axInfo.fieldName = TABLE_FIELD_ACTUALX;
    retInfos.append(axInfo);
    ResultInfo   ayInfo;
    ayInfo.fieldName = TABLE_FIELD_ACTUALY;
    retInfos.append(ayInfo);
    ResultInfo   azInfo;
    azInfo.fieldName = TABLE_FIELD_ACTUALZ;
    retInfos.append(azInfo);

    ResultInfo   depolyedInfo;
    depolyedInfo.fieldName = TABLE_FIELD_DEPOLYED;
    retInfos.append(depolyedInfo);
    ResultInfo   pickedInfo;
    pickedInfo.fieldName = TABLE_FIELD_PICKEDUP;
    retInfos.append(pickedInfo);

    ResultInfo   downloadInfo;
    downloadInfo.fieldName = TABLE_FIELD_DOWNLOADED;
    retInfos.append(downloadInfo);
    qDebug()<<"ret infos size:"<<retInfos.size();
    while(!in.atEnd())
    {
        strList = in.readLine().split(QRegExp("\\s+"));
        device  = QString("G%1").arg(strList[2].mid(1),3,QChar('0'));
        conds[0].fieldValue = device;

        //值域-----------------------
        retInfos[0].fieldValue = device;

        int devno = device.mid(1).toInt();
        int ip3 = devno/100+1;
        int ip4 = devno - (ip3-1)*100+100;

        QString ipaddress = QString("192.168.%1.%2")
                .arg(ip3)
                .arg(ip4);
        retInfos[1].fieldValue = ipaddress;
        retInfos[2].fieldValue = m_currentArea;

        retInfos[3].fieldValue = QString::number(line_no);
        retInfos[4].fieldValue = QString::number(no);

        retInfos[5].fieldValue = strList[1];
        retInfos[6].fieldValue = strList[0];
        retInfos[7].fieldValue = "0";

        retInfos[8].fieldValue = strList[1];
        retInfos[9].fieldValue = strList[0];

        retInfos[10].fieldValue = "0";
        retInfos[11].fieldValue = "1";
        retInfos[12].fieldValue = "1";

        retInfos[13].fieldValue = m_downloadedGOBS.contains(device)?1:0;
        //判断设备是否存在，存在更新，不存在则insert 插入
        QSqlQuery query = sqlFunc->query(m_currentArea,conds,&ok);
        if(query.next())
        {
            sqlFunc->update(m_currentArea,conds,retInfos);
        }else
            sqlFunc->insert(m_currentArea,retInfos);

        no ++;
    }

    file.close();
    //刷新
    initDepolyments();
    emit signalUpdateRNDepolyments();
}

int RNDeployments::lineNo(const QString &name)
{
    int left  = name.indexOf("SEQ");
    int right = name.indexOf(".",left);

    return name.mid(left+3,right-left-3).toInt();
}




