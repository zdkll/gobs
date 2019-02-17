#include "assignnodedlg.h"
#include "ui_assignnodedlg.h"

#include <QMessageBox>
#include "gsqlfunctions.h"


RNTableModel::RNTableModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void RNTableModel::setSourceData(QList<NodeDevice *> unassignedDevices)
{
    this->beginResetModel();
    m_unassignedDevices = unassignedDevices;
    this->endResetModel();
}

int RNTableModel::columnCount(const QModelIndex &  ) const
{
    return 1;
}

int RNTableModel::rowCount(const QModelIndex &) const
{
    return m_unassignedDevices.size();
}

QModelIndex RNTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(column == 0)
    {
        return createIndex(row,column,m_unassignedDevices[row]);
    }

    return QModelIndex();
}

QVariant RNTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        return m_unassignedDevices[index.row()]->Device;
    }

    return QVariant();
}

QVariant RNTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section == 0 && orientation == Qt::Horizontal)
        if(role == Qt::DisplayRole)
            return  QVariant("Remote Node");
    return QVariant();
}



//DepolyModel--------------------------------------------------
DepolyModel::DepolyModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void DepolyModel::setSourceData(StaLine &staline,QMap<int,NodeDevice *> assignedDevices)
{
    this->beginResetModel();
    m_staLine = staline;
    m_assignedDevices = assignedDevices;
    this->endResetModel();
}

int DepolyModel::columnCount(const QModelIndex &) const
{
    return 2;
}

int DepolyModel::rowCount(const QModelIndex &) const
{
    return m_staLine.staNum;
}

QModelIndex DepolyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    Q_UNUSED(row)
    if(column == 0)
    {

    }
    if(column == 1)
    {

    }
    return QModelIndex();
}

QVariant DepolyModel::data(const QModelIndex &index, int role ) const
{
    if(role == Qt::DisplayRole)
    {
        int sp = m_staLine.stations[index.row()].sp;
        if(index.column() == 0)
        {
            return sp;
        }
        if(index.column() == 1)
        {
            if(m_assignedDevices.value(sp,0))
                return QVariant();
        }
    }

    return QVariant();
}

QVariant DepolyModel::headerData(int section, Qt::Orientation orientation, int ) const
{
    if(orientation == Qt::Horizontal)
    {
        if(section == 0)
            return "Station";
        if(section == 1)
            return "Remote Node";
    }
    return QVariant();
}



//---------------------------------------------------
AssignNodeDlg::AssignNodeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AssignNodeDlg)
{
    ui->setupUi(this);
    this->setWindowTitle("Assign RNs to Receiver Stations");
    connect(ui->areaCbx,SIGNAL(currentTextChanged(QString)),this,SLOT(slotCurrentAreaChanged(QString)));

    initDlg();

    m_stationInfo = new StationInfo;

    connect(ui->currentDepolyTable,SIGNAL(itemSelectionChanged()),this,SLOT(slotDepolySelectionChanged()));
    connect(ui->RNTable,SIGNAL(itemSelectionChanged()),this,SLOT(slotRNSelectionChanged()));
}

AssignNodeDlg::~AssignNodeDlg()
{
    delete ui;
    if(m_stationInfo)
        m_stationInfo->clear();
    delete m_stationInfo;
}

void AssignNodeDlg::initDlg()
{
    ui->filterEdit->setInputMask("G000;");

    ui->filterEdit->setText("");
    ui->filterEdit->setCursorPosition(0);

    ui->RNTable->setColumnCount(1);
    ui->RNTable->setHorizontalHeaderLabels(QStringList()<<"Remote Node");
    ui->RNTable->verticalHeader()->setVisible(false);
    ui->RNTable->horizontalHeader()->setStretchLastSection(true);

    ui->RNTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->currentDepolyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->RNTable->setShowGrid(false);
    ui->currentDepolyTable->setShowGrid(false);
    ui->currentDepolyTable->verticalHeader()->setVisible(false);

    ui->RNTable->sortByColumn(0);

    ui->moveUpBtn->setEnabled(false);
    ui->moveDownBtn->setEnabled(false);
}

void AssignNodeDlg::initDepolyments()
{
    QString profile = m_projInfo.ProjectPath + Dir_Separator
            + m_projInfo.ProjectName+Project_File_Suffix;
    QStringList areasList = Area::areasFromProject(profile);
    ui->areaCbx->clear();
    ui->areaCbx->addItems(areasList);
}

void AssignNodeDlg::addNewArea(const QString &area)
{
    ui->areaCbx->addItem(area);
    //如果当前工区为空,需要初始化显示
    if(m_currentArea.isEmpty()){
        m_currentArea = ui->areaCbx->currentText();
        initDisplay();
    }
}

void AssignNodeDlg::deleteArea(const QString &area)
{
    for(int i = 0;i<ui->areaCbx->count();i++)
    {
        if(ui->areaCbx->itemText(i) == area){
            ui->areaCbx->removeItem(i);
            break;
        }
    }
}

void AssignNodeDlg::initDisplay()
{

    //1 初始化未分配设备
    bool ok = initUnassignedDevices();
    if(!ok)
        return;

    //2 读取sps文件，初始化线号以及对应站点号列表
    ok = initLineStation();
    if(!ok)
        return;
}

bool AssignNodeDlg::initLineStation()
{
    //读取sps,R文件---------------------------------------------
    QStringList spsFiles = Area::spsFiles(m_projInfo,m_currentArea);
    //R file
    QString  rFileName;
    foreach (QString filename, spsFiles) {
        if(filename.contains(".r",Qt::CaseInsensitive))
            rFileName = filename;
    }
    qDebug()<<"RFile:"<<rFileName;

    if(rFileName.isEmpty()){
        QMessageBox::critical(this,"error","Not Found R File.");
        return false;
    }

    if(m_stationInfo)
        m_stationInfo->clear();
    else
        m_stationInfo = new StationInfo;

    int ok = gobs_sps_file_SR_read(rFileName,m_stationInfo);
    if(ok!=0){
        QMessageBox::critical(this,"error","Read R File Error.");
        return false;
    }

    ui->lineCbx->clear();
    //添加lines
    for(int i = 0;i<m_stationInfo->lineNum;i++)
        ui->lineCbx->addItem(QString::number(m_stationInfo->staLines[i].line));

    return true;
}

bool AssignNodeDlg::initUnassignedDevices()
{

    //查询当前工区的未分配设备列表,显示-------------------------
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    QList<Condition> conds;

    Condition cond;
    cond.fieldName = TABLE_FIELD_AREA;
    cond.fieldValue = m_currentArea;
    conds.append(cond);
    //
    cond.fieldName = TABLE_FIELD_LINE;
    cond.fieldValue = 0;
    conds.append(cond);

    QStringList fieldList;
    fieldList<<TABLE_FIELD_DEVICE;

    bool ok = false;
    QSqlQuery query = sqlFunc->query(m_currentArea,fieldList,conds,&ok);
    if(!ok){
        QMessageBox::critical(this,"error","Query device table failed,error:"
                              +sqlFunc->errorString());
        return false;
    }

    //总的设备列表
    qDeleteAll(m_unAssignedDevices);
    m_unAssignedDevices.clear();

    NodeDevice *node;
    //保存所有未分配设备
    while(query.next())
    {
        node = new NodeDevice;
        node->Device  = query.value(0).toString();
        m_unAssignedDevices.append(node);
    }

    //所有设备，区分分配和为分配设备
    ui->RNTable->setRowCount(m_unAssignedDevices.size());
    for(int i = 0;i<m_unAssignedDevices.size();i++)
    {
        ui->RNTable->setItem(i,0,new QTableWidgetItem(m_unAssignedDevices[i]->Device));
    }
    ui->RNTable->sortByColumn(0,Qt::AscendingOrder);

    ui->nodeCount->setText(QString::number(m_unAssignedDevices.size()));
    return true;
}

void AssignNodeDlg::slotCurrentAreaChanged(const QString &area)
{
    qDebug()<<"current area changed:"<<area;
    m_currentArea = area;
    initDisplay();
}

void AssignNodeDlg::on_assignBtn_clicked()
{
    //投放,所有选中设备移到右侧，如果满了，则什么都不做
    QString device;
    for(int i=0;i<ui->RNTable->rowCount();i++)
    {
        if(ui->RNTable->item(i,0)->isSelected())
        {
            device = ui->RNTable->item(i,0)->text();
            //查找对应的设备
            for(int j = 0;j<m_unAssignedDevices.size();j++)
                if(m_unAssignedDevices[j]->Device == device){
                    //添加到 m_currentDepolyments
                    if(depolyDevice(m_unAssignedDevices[j])){
                        ui->RNTable->removeRow(i);
                        m_unAssignedDevices.removeAt(j);
                        i--;
                    }
                    break;
                }
        }
    }
    ui->nodeCount->setText(QString::number(m_unAssignedDevices.size()));

}

bool AssignNodeDlg::depolyDevice(NodeDevice *device)
{
    for(int i = 0;i<ui->currentDepolyTable->rowCount();i++)
    {
        //找到第一个空的站点投放
        if(!ui->currentDepolyTable->item(i,1))
        {
            QTableWidgetItem *item = new QTableWidgetItem(device->Device);
            item->setData(Qt::BackgroundRole,QColor(Qt::yellow));
            item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
            ui->currentDepolyTable->setItem(i,1,item);

            m_assignedDevices.append(device);
            //device 设置线号和站点号
            device->line    = m_currentLine;
            device->station = ui->currentDepolyTable->item(i,0)->text().toInt();
            device->eidted  = true;

            addEditedDevice(device);

            ui->assignedCount->setText(QString::number(m_assignedDevices.size()));
            ui->unassignedCount->setText(
                        QString::number(ui->currentDepolyTable->rowCount()
                                        - m_assignedDevices.size()));

            return true;
        }
    }
    return false;
}

bool AssignNodeDlg::unDepolyDevice(NodeDevice *device)
{
    //设备投放信息重置
    device->line    = 0.0;
    device->station = 0;
    device->eidted  = true;

    addEditedDevice(device);

    m_unAssignedDevices.append(device);

    ui->RNTable->setRowCount(m_unAssignedDevices.size());
    for(int i=0; i<m_unAssignedDevices.size();i++)
        ui->RNTable->setItem(i,0
                             ,new QTableWidgetItem(m_unAssignedDevices[i]->Device));
    ui->RNTable->sortItems(0);
    ui->nodeCount->setText(QString::number(m_unAssignedDevices.size()));
    return true;
}

void AssignNodeDlg::addEditedDevice(NodeDevice *device)
{
    if(m_editedDevices.indexOf(device)<0)
        m_editedDevices.append(device);
}

void AssignNodeDlg::on_unassignBtn_clicked()
{
    //与assign 相反，将投放的设备回收
    for(int i=0;i<ui->currentDepolyTable->rowCount();i++)
    {
        if(ui->currentDepolyTable->item(i,1)
                &&ui->currentDepolyTable->item(i,1)->isSelected())
        {
            QString device = ui->currentDepolyTable->item(i,1)->text();
            if(device.isEmpty())
            {
                //直接移除
                ui->currentDepolyTable->setItem(i,1,0);
            }else
            {
                //从分配的设备中查找对应设备
                for(int j = 0;j<m_assignedDevices.size();j++)
                    if(m_assignedDevices[j]->Device == device)
                        if(unDepolyDevice(m_assignedDevices[j]))
                        {
                            ui->currentDepolyTable->setItem(i,1,0);
                            m_assignedDevices.removeAt(j);
                            j--;
                        }
            }
        }
    }
    ui->assignedCount->setText(QString::number(m_assignedDevices.size()));
    ui->unassignedCount->setText(
                QString::number(ui->currentDepolyTable->rowCount() - m_assignedDevices.size()));
}

void AssignNodeDlg::on_moveUpBtn_clicked()
{
    //所有选中项上移
    QList<QTableWidgetItem *> selectedItems = ui->currentDepolyTable->selectedItems();
    int row;
    for(int i =0;i<selectedItems.count();i++)
    {
        row = selectedItems[i]->row();
        if(row<1)
            continue;

        QTableWidgetItem *previtem = ui->currentDepolyTable->takeItem(row-1,1);
        QTableWidgetItem *item = ui->currentDepolyTable->takeItem(row,1);

        ui->currentDepolyTable->setItem(row-1,1,item);
        ui->currentDepolyTable->setItem(row,1,previtem);

        //查找对应的设备，更新信息
        QString device = item->text();
        foreach (NodeDevice *dev , m_assignedDevices) {
            if(dev->Device == device){
                dev->station = ui->currentDepolyTable->item(row-1,0)->text().toInt();
                addEditedDevice(dev);
                break;
            }
        }
        if(previtem){
            device = previtem->text();
            foreach (NodeDevice *dev , m_assignedDevices) {
                if(dev->Device == device){
                    dev->station = ui->currentDepolyTable->item(row,0)->text().toInt();
                    addEditedDevice(dev);
                    break;
                }
            }
        }
    }
    ui->currentDepolyTable->reset();
    foreach (QTableWidgetItem *item, selectedItems) {
        item->setSelected(true);
    }
}

void AssignNodeDlg::on_moveDownBtn_clicked()
{
    //所有选中项下移
    QList<QTableWidgetItem *> selectedItems = ui->currentDepolyTable->selectedItems();
    int row;
    for(int i =selectedItems.count()-1;i>=0;i--)
    {
        row = selectedItems[i]->row();
        if(row>ui->currentDepolyTable->rowCount()-2)
            continue;

        QTableWidgetItem *item = ui->currentDepolyTable->takeItem(row,1);
        QTableWidgetItem *nextitem = ui->currentDepolyTable->takeItem(row+1,1);

        ui->currentDepolyTable->setItem(row+1,1,item);
        ui->currentDepolyTable->setItem(row  ,1,nextitem);

        //查找对应的设备，更新信息
        QString device = item->text();
        foreach (NodeDevice *dev , m_assignedDevices) {
            if(dev->Device == device){
                dev->station = ui->currentDepolyTable->item(row+1,0)->text().toInt();
                addEditedDevice(dev);
                break;
            }
        }
        if(nextitem){
            device = nextitem->text();
            foreach (NodeDevice *dev , m_assignedDevices) {
                if(dev->Device == device){
                    dev->station = ui->currentDepolyTable->item(row,0)->text().toInt();
                    addEditedDevice(dev);
                    break;
                }
            }
        }
    }
    ui->currentDepolyTable->reset();
    foreach (QTableWidgetItem *item, selectedItems) {
        item->setSelected(true);
    }
}

void AssignNodeDlg::on_lineCbx_currentIndexChanged(int index)
{
    if(!m_stationInfo)
        return;
    if(index<0)
        return;

    //上一条线保存
    saveEditedDevice();

    //找到该条线
    StaLine  staLine = m_stationInfo->staLines[index];
    m_currentLine = staLine.line;

    qDebug()<<"line:"<<index;
    //显示该条线所有站点，以及对应的设备
    if(!initStationDevicesByLine(staLine))
        return;
}


bool AssignNodeDlg::initStationDevicesByLine(const StaLine &staLine)
{
    //查询线号对应的设备
    SqlFunctions *sqlFunc = SqlFunctions::instance();
    QList<Condition> conds;

    Condition cond;
    cond.fieldName = TABLE_FIELD_AREA;
    cond.fieldValue = m_currentArea;
    conds.append(cond);
    //线
    cond.fieldName = TABLE_FIELD_LINE;
    cond.fieldValue = staLine.line;
    conds.append(cond);

    QStringList fieldList;
    fieldList<<TABLE_FIELD_DEVICE
            <<TABLE_FIELD_LINE
           <<TABLE_FIELD_STATION;

    bool ok = false;
    QSqlQuery query = sqlFunc->query(m_currentArea,fieldList,conds,&ok);
    if(!ok){
        QMessageBox::critical(this,"error","Query device table failed,error:"
                              +sqlFunc->errorString());
        return false;
    }
    //保存当前已经分配的设备
    qDeleteAll(m_assignedDevices);
    m_assignedDevices.clear();
    while(query.next())
    {
        NodeDevice *device = new NodeDevice;
        device->Device = query.value(0).toString();
        device->line   = query.value(1).toFloat();
        device->station= query.value(2).toInt();
        m_assignedDevices.append(device);
    }

    //reset 表
    ui->currentDepolyTable->clear();
    ui->currentDepolyTable->setColumnCount(2);
    ui->currentDepolyTable->setHorizontalHeaderLabels(QStringList()<<"Station"
                                                      <<"Remote Node");
    //添加站点
    ui->currentDepolyTable->setRowCount(staLine.staNum);
    for(int i = 0;i<staLine.staNum;i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(staLine.stations[i].sp));
        item->setFlags(Qt::ItemIsEnabled);
        ui->currentDepolyTable->setItem(i,0,item);
    }

    //给站点投放对应设备
    for(int i = 0;i<m_assignedDevices.size();i++)
        for(int j = 0; j<staLine.staNum;j++)
        {
            if(staLine.stations[j].sp == m_assignedDevices[i]->station)
            {
                QTableWidgetItem *item = new QTableWidgetItem(m_assignedDevices[i]->Device);
                item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                item->setData(Qt::BackgroundColorRole,QColor(Qt::yellow));
                ui->currentDepolyTable->setItem(j,1,item);
            }
        }


    ui->assignedCount->setText(QString::number(m_assignedDevices.size()));
    ui->unassignedCount->setText(QString::number(staLine.staNum - m_assignedDevices.size()));
    return true;
}
void AssignNodeDlg::on_filterEdit_textChanged(const QString &arg1)
{
    QTableWidgetItem *item;
    for(int i =0;i<ui->RNTable->rowCount();i++)
    {
        item = ui->RNTable->item(i,0);

        ui->RNTable->setRowHidden(i,!item->text().startsWith(arg1));
        item->setSelected(item->text().startsWith(arg1));
    }
}


void AssignNodeDlg::slotDepolySelectionChanged()
{
    if(ui->currentDepolyTable->selectedItems().size()>0){
        ui->moveUpBtn->setEnabled(true);
        ui->moveDownBtn->setEnabled(true);
    }else
    {
        ui->moveUpBtn->setEnabled(false);
        ui->moveDownBtn->setEnabled(false);
    }
}

void AssignNodeDlg::slotRNSelectionChanged()
{
    ui->selectedCount->setText(
                QString::number(ui->RNTable->selectedItems().size()));
}


void AssignNodeDlg::on_okBtn_clicked()
{
    //保存当前更改
    saveEditedDevice();
    this->close();

    emit signalUpdateAssignedDev(m_currentArea);
}

void AssignNodeDlg::on_applyBtn_clicked()
{
    //保存当前更改
    saveEditedDevice();

    emit signalUpdateAssignedDev(m_currentArea);
}

void AssignNodeDlg::on_cancelBtn_clicked()
{

    this->close();
}

bool AssignNodeDlg::saveEditedDevice()
{
    m_editedDevices =  m_editedDevices.toSet().toList();
    qDebug()<<"Edited Devices:"<<m_editedDevices.size();
    SqlFunctions *sqlFunc = SqlFunctions::instance();

    //条件-设备名称
    QList<Condition> condtions;
    Condition cond;
    cond.fieldName = TABLE_FIELD_DEVICE;
    condtions.append(cond);

    //设置字段，线号-站点号-对应的,x,y,z
    QList<ResultInfo> resultInfos;
    ResultInfo  retInfo;
    retInfo.fieldName = TABLE_FIELD_LINE;
    resultInfos.append(retInfo);

    retInfo.fieldName = TABLE_FIELD_STATION;
    resultInfos.append(retInfo);
    //sps xyz
    retInfo.fieldName = TABLE_FIELD_SPSX;
    resultInfos.append(retInfo);
    retInfo.fieldName = TABLE_FIELD_SPSY;
    resultInfos.append(retInfo);
    retInfo.fieldName = TABLE_FIELD_SPSZ;
    resultInfos.append(retInfo);

    //增加初始默认实际的坐标和sps坐标一致
    retInfo.fieldName = TABLE_FIELD_ACTUALX;
    resultInfos.append(retInfo);
    retInfo.fieldName = TABLE_FIELD_ACTUALY;
    resultInfos.append(retInfo);
    retInfo.fieldName = TABLE_FIELD_ACTUALZ;
    resultInfos.append(retInfo);

    bool ok;
    StaLine staLine;
    for(int i = 0;i<m_editedDevices.size();i++){
        // qDebug()<<m_editedDevices[i]->Device<<m_editedDevices[i]->station
        //          <<"line:"<<m_editedDevices[i]->line;
        //条件
        condtions[0].fieldValue = m_editedDevices[i]->Device;

        //更新
        resultInfos[0].fieldValue = m_editedDevices[i]->line;
        resultInfos[1].fieldValue = m_editedDevices[i]->station;

        //查找对应站点x,y,z 坐标
        for(int line = 0;line<m_stationInfo->lineNum;line++)
        {
            staLine = m_stationInfo->staLines[line];

            if(staLine.line == m_editedDevices[i]->line){

                for(int station = 0;station<staLine.staNum;station++){
                    if(staLine.stations[station].sp == m_editedDevices[i]->station)
                    {

                        resultInfos[2].fieldValue =  staLine.stations[station].x;
                        resultInfos[3].fieldValue =  staLine.stations[station].y;
                        resultInfos[4].fieldValue =  staLine.stations[station].del;

                        resultInfos[5].fieldValue =  staLine.stations[station].x;
                        resultInfos[6].fieldValue =  staLine.stations[station].y;
                        resultInfos[7].fieldValue =  staLine.stations[station].del;
                        break;
                    }
                }
                break;
            }
        }

        ok = sqlFunc->update(m_currentArea,condtions,resultInfos);
        if(!ok)
        {
            QMessageBox::critical(this,"error","Save Device Information Error.");
            return false;
        }
    }

    m_editedDevices.clear();
    return true;
}

void AssignNodeDlg::on_updateDev_clicked()
{
    if(m_editedDevices.size()>0)
    {
        QMessageBox box(QMessageBox::Information,"Note"
                        ,"Do you want to save changes?"
                        ,QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,this);

        switch (box.exec()) {
        case QMessageBox::Yes:
            saveEditedDevice();
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            return;
            break;
        default:
            break;
        }
    }

    //重新更新显示
    //1 初始化未分配设备
    initUnassignedDevices();

    //2 更新当前接收线
    //找到该条线
    int index = ui->lineCbx->currentIndex();
    StaLine  staLine = m_stationInfo->staLines[index];
    m_currentLine = staLine.line;

    //显示该条线所有站点，以及对应的设备
    if(!initStationDevicesByLine(staLine))
        return;
}
