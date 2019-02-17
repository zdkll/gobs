#include "deploysystem.h"
#include "ui_deploysystem.h"

#include "gpublicwidgets.h"
#include  "gsqlfunctions.h"

#include <QHostAddress>
#include <QMessageBox>
#include <QDateTime>

DeploySystem::DeploySystem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeploySystem),m_workStatus(Depolyment)
{
    ui->setupUi(this);
    ui->ipAddrEdit->setText("192.168.1.192");
    this->setWindowTitle("Depoly System");

    ui->portEdit->setValidator(new QIntValidator(0,65536,this));

    m_socket = new QTcpSocket(this);
    connect(m_socket,SIGNAL(stateChanged(QAbstractSocket::SocketState))
            ,this,SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));

    connect(m_socket,SIGNAL(readyRead()),this,SLOT(slotSocketReadyRead()));

    ui->deviceNoEdit->setInputMask("G999; ");

    ui->tabWidget->setCurrentIndex(0);

    m_processTimer = 0;
}

DeploySystem::~DeploySystem()
{
    delete ui;
}

void DeploySystem::setProjInfo(const ProjectInfo &projInfo)
{
    m_projInfo = projInfo;

    //1 更新当前项目Area 列表
    updateAreaList();

    //2 读取工区GOBS下载信息
    updateDownload();
}
void DeploySystem::updateDownload()
{
    //读取工区GOBS下载信息
    m_downloadedGOBS = Project::finishedDownloadGOBS(m_projInfo);
}

void DeploySystem::setCurrentArea(const QString &area)
{
    m_currentArea = area;
    ui->areaCbx->setCurrentText(area);
    ui->addToAreaCbx->setCurrentText(area);
}

void DeploySystem::addNewArea(const QString &area)
{
    ui->areaCbx->addItem(area);
    ui->addToAreaCbx->addItem(area);
}

void DeploySystem::deleteArea(const QString &area)
{
    for(int i = 0;i<ui->areaCbx->count();i++)
        if(ui->areaCbx->itemText(i) == area)
        {
            ui->areaCbx->removeItem(i);
            ui->addToAreaCbx->removeItem(i);
            break;
        }
}

void DeploySystem::connectToHost(const bool &con)
{
    //链接
    if(con)
    {
        ui->ConCbx->setEnabled(false);
        ui->ConCbx->setChecked(true);
        ui->textBrowser->append("connecting...");
        m_socket->connectToHost(QHostAddress(ui->ipAddrEdit->text()),
                                ui->portEdit->text().toInt());
    }
    //断开
    else
    {
        ui->ConCbx->setChecked(false);
        if(m_socket->state() == QAbstractSocket::ConnectedState)
        {
            ui->ConCbx->setEnabled(false);
            ui->textBrowser->append("disconnect....");
            m_socket->close();
        }
    }
}

void DeploySystem::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == m_processTimer)
    {
        //定时清除扫描到的结果
        m_currentData.clear();
    }
}

void DeploySystem::slotSocketStateChanged(const QAbstractSocket::SocketState &state)
{
    //连接
    if(state == QAbstractSocket::ConnectedState)
    {
        ui->ConCbx->setEnabled(true);
        ui->textBrowser->append("connect success.");
        emit  signalDepolySysStateChanged(true);
        //启动一个定时器
        m_processTimer = this->startTimer(1000);
    }
    //断开
    if(state == QAbstractSocket::UnconnectedState)
    {
        ui->ConCbx->setEnabled(true);

        ui->textBrowser->append("not connected,error "+ m_socket->errorString());
        emit signalDepolySysStateChanged(false);

        //停止结果刷新
        if(m_processTimer>0){
            killTimer(m_processTimer);
            m_processTimer = 0;
        }
    }
}

void DeploySystem::slotSocketReadyRead()
{
    if(m_currentArea.isEmpty())
        return;
    QByteArray data = m_socket->readAll();
    if(data == m_currentData)
        return;
    m_currentData = data;
    QString  str = QString(m_currentData.toHex().toUpper());
    ui->textBrowser->append(QString("read:%1").arg(str));

    if(str.mid(4,2)!="EE")
        return;

    //获取设备的IP和名称
    QString EPCString = str.mid(9,15);
    QString devNo     = EPCString.left(3);
    QString ipaddress = EPCString.mid(3,3)+".";
    ipaddress += QString::number(EPCString.mid(6,3).toInt())+".";
    ipaddress += QString::number(EPCString.mid(9,3).toInt())+".";
    ipaddress += QString::number(EPCString.mid(12,3).toInt());

    if(!ipaddress.startsWith("192.168."))
        return;

    QString device = QString("G%1").arg(devNo);
    ui->textBrowser->append(QString("Device G%1 IP:%2")
                            .arg(devNo)
                            .arg(ipaddress));
    bool ok = addOrUpdateRecord(m_currentArea,device,ipaddress,m_workStatus);

    if(ok){
        ui->textBrowser->append(QString("Add/Update Device %1 Success!")
                                .arg(device));
        emit signalUpdateDevice(m_currentArea);
    }
    else
        ui->textBrowser->append(QString("Add/Update Device %1 Failed,Error:%2")
                                .arg(device)
                                .arg(m_errString));
}

void DeploySystem::on_clearBtn_clicked()
{
    ui->textBrowser->clear();
}

void DeploySystem::on_closeBtn_clicked()
{
    this->close();
}

void DeploySystem::on_ConCbx_clicked(bool checked)
{
    //链接
    if(checked)
    {
        ui->ConCbx->setEnabled(false);
        ui->textBrowser->append("connecting...");
        m_socket->connectToHost(QHostAddress(ui->ipAddrEdit->text()),
                                ui->portEdit->text().toInt());
    }
    //断开
    else
    {
        if(m_socket->state() == QAbstractSocket::ConnectedState)
        {
            ui->ConCbx->setEnabled(false);
            ui->textBrowser->append("disconnect....");
            m_socket->close();
        }
    }
    emit signalDepolySysConnected(checked);
}

void DeploySystem::on_workStatusCbx_currentIndexChanged(int index)
{
    if(index == 0)
        m_workStatus = Depolyment;
    else if(index == 1)
        m_workStatus = PickUp;
    else if(index == 2)
        m_workStatus = Both;
}

void DeploySystem::updateAreaList()
{
    QString profile = m_projInfo.ProjectPath +Dir_Separator
            +m_projInfo.ProjectName+Project_File_Suffix;
    QStringList areas = Area::areasFromProject(profile);

    ui->areaCbx->clear();
    if(areas.isEmpty()){
        ui->textBrowser->append("No Area");
        ui->textBrowser2->append("No Area");
    }
    else{
        ui->areaCbx->addItems(areas);
        ui->addToAreaCbx->addItems(areas);
    }
    m_currentArea = ui->areaCbx->currentText();
}

void DeploySystem::on_areaCbx_currentTextChanged(const QString &arg1)
{
    m_currentArea = arg1;
}

bool DeploySystem::addOrUpdateRecord(QString areaName,QString device
                                     ,QString ipaddress,WorkStatus workStatus)
{
    //m_workStatus ,根据工作状态和收到的信息更新数据库
    //查询记录是否存在
    SqlFunctions *sqlFunc = SqlFunctions::instance();

    QStringList fieldList;
    fieldList<<TABLE_FIELD_DEVICE;
    QList<Condition> conds;
    Condition cond;
    cond.fieldName = TABLE_FIELD_DEVICE;
    cond.fieldValue = device;
    conds.append(cond);
    bool ok = false;
    QSqlQuery query = sqlFunc->query(areaName,conds,&ok);
    if(!ok)
    {
        m_errString = sqlFunc->errorString();
        return false;
    }

    if(query.next())//update
    {
        QList<ResultInfo> resultInfos;
        ResultInfo        retInfo;
        if(workStatus == Depolyment || workStatus == Both){
            //状态
            retInfo.fieldName = TABLE_FIELD_DEPOLYED;
            retInfo.fieldValue = 1;
            resultInfos.append(retInfo);
            //DataTime
            retInfo.fieldName = TABLE_FIELD_DEPOLYTIME;
            retInfo.fieldValue = QDateTime::currentDateTime().toString(Qt::ISODate);
            resultInfos.append(retInfo);
        }
        if(workStatus == PickUp || workStatus == Both){
            retInfo.fieldName = TABLE_FIELD_PICKEDUP;
            retInfo.fieldValue = 1;
            resultInfos.append(retInfo);

            //DataTime
            retInfo.fieldName = TABLE_FIELD_PICKUPTIME;
            retInfo.fieldValue = QDateTime::currentDateTime().toString(Qt::ISODate);
            resultInfos.append(retInfo);
        }
        ok = sqlFunc->update(areaName,conds,resultInfos);
        if(!ok)
        {
            m_errString = sqlFunc->errorString();
            return false;
        }
    }
    else//insert
    {
        QList<ResultInfo> resultInfos;
        ResultInfo        retInfo;
        retInfo.fieldName = TABLE_FIELD_DEVICE;
        retInfo.fieldValue = device;
        resultInfos.append(retInfo);

        retInfo.fieldName = TABLE_FIELD_IP;
        retInfo.fieldValue = ipaddress;
        resultInfos.append(retInfo);

        retInfo.fieldName = TABLE_FIELD_AREA;
        retInfo.fieldValue = areaName;
        resultInfos.append(retInfo);

        retInfo.fieldName = TABLE_FIELD_LINE;
        retInfo.fieldValue = 0;
        resultInfos.append(retInfo);

        retInfo.fieldName = TABLE_FIELD_STATION;
        retInfo.fieldValue = 0;
        resultInfos.append(retInfo);
        if(workStatus == Depolyment || workStatus == Both){
            //状态
            retInfo.fieldName = TABLE_FIELD_DEPOLYED;
            retInfo.fieldValue = 1;
            resultInfos.append(retInfo);
            //DataTime
            retInfo.fieldName = TABLE_FIELD_DEPOLYTIME;
            retInfo.fieldValue = QDateTime::currentDateTime().toString(Qt::ISODate);
            resultInfos.append(retInfo);
        }
        if(workStatus == PickUp || workStatus == Both){
            retInfo.fieldName = TABLE_FIELD_PICKEDUP;
            retInfo.fieldValue = 1;
            resultInfos.append(retInfo);

            //DataTime
            retInfo.fieldName = TABLE_FIELD_PICKUPTIME;
            retInfo.fieldValue = QDateTime::currentDateTime().toString(Qt::ISODate);
            resultInfos.append(retInfo);
        }
        retInfo.fieldName = TABLE_FIELD_DOWNLOADED;
        retInfo.fieldValue= m_downloadedGOBS.contains(device)?1:0;
        resultInfos.append(retInfo);

        ok = sqlFunc->insert(areaName,resultInfos);
        if(!ok)
        {
            m_errString = sqlFunc->errorString();
            return false;
        }
    }
    return true;
}

//手动添加部分代码-------------------------
void DeploySystem::on_addBtn_clicked()
{
    QString areaName = ui->addToAreaCbx->currentText();
    if(areaName.isEmpty())
    {
        ui->textBrowser2->append("No Area");
        return;
    }

    //根据设备名称计算IP  地址
    QString device = ui->deviceNoEdit->text().trimmed();

    if(device =="G")
    {
        ui->textBrowser2->append("Input Device No.");
        return;
    }
    int no = device.mid(1).toInt();
    int ip3 = no/100+1;
    int ip4 = no - (ip3-1)*100+100;

    device = QString("G%1").arg(no,3,10,QChar('0'));
    //保存
    QString ipaddress = QString("192.168.%1.%2")
            .arg(ip3)
            .arg(ip4);
    //    qDebug()<<"ipaddress:"<<ipaddress;

    WorkStatus workStatus;
    if(ui->workStatusCbx2->currentIndex() == 0)
        workStatus = Depolyment;
    else if(ui->workStatusCbx2->currentIndex() == 1)
        workStatus = PickUp;
    else
        workStatus = Both;

    bool ok = addOrUpdateRecord(m_currentArea,device,ipaddress,workStatus);

    if(ok){
        ui->textBrowser2->append(QString("Add/Update Device %1 Success!")
                                 .arg(device));
        emit signalUpdateDevice(areaName);
    }
    else
        ui->textBrowser2->append(QString("Add/Update Device %1 Failed,Error:%2")
                                 .arg(device)
                                 .arg(m_errString));
}

void DeploySystem::on_closeBtn2_clicked()
{
    this->close();
}

void DeploySystem::on_pushButton_clicked()
{
    ui->textBrowser2->clear();
}
