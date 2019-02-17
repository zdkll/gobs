#include "statusreport.h"
#include "ui_statusreport.h"

//status Report
#define  STATUS_REPORT_COLUNM_NUM 7  //9
#define  COLUMN_DEVICE_NO         0
#define  COLUMN_STATUS            1
#define  COLUMN_VOLTAGE           2
#define  COLUMN_TEMPERATURE       3
#define  COLUMN_PRESSURE          4
//#define  COLUMN_ELECURRENT        5
//#define  COLUMN_CHARGE_VOLTAGE    6
#define  COLUMN_Memory_CAPACITY   5
#define  COLUMN_DOWNLOADED        6

#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

StatusItemDelegate::StatusItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}


void StatusItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{

    if(index.column() == COLUMN_STATUS)
    {
        bool state = index.data().toBool();

        QColor color;
        if(state)
            color =QColor(Qt::green);
        else
            color = QColor(Qt::lightGray);
        painter->setBrush(color);

        painter->drawEllipse(option.rect.center(),5,5);
    }
    else
        QStyledItemDelegate::paint(painter,option,index);
}

QSize StatusItemDelegate::sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    return QSize(60,20);
}

void StatusItemModel::setStatusInfo(const QVector<Node*> &nodes)
{
    this->beginResetModel();
    m_Nodes = nodes;
    this->endResetModel();
}

int StatusItemModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_Nodes.size();
}

int StatusItemModel::columnCount(const QModelIndex &/*parent*/) const
{
    return STATUS_REPORT_COLUNM_NUM;
}

QVariant StatusItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int i = index.row();
    int col = index.column();
    if(role == Qt::DisplayRole)
    {
        if(col == COLUMN_DEVICE_NO)
            return QString("G%1").arg(m_Nodes[i]->no(),3,10,QChar('0'));
        else if(col == COLUMN_STATUS)
            return m_Nodes[i]->state();
        else if(col == COLUMN_VOLTAGE)
            return QString::number(m_Nodes[i]->voltage());
        else if(col == COLUMN_TEMPERATURE)
            return QString::number(m_Nodes[i]->temperature());
        else if(col == COLUMN_PRESSURE)
            return QString::number(m_Nodes[i]->pressure());
        //        else if(col == COLUMN_ELECURRENT)
        //            return QString::number(m_Nodes[i]->current());
        //        else if(col == COLUMN_CHARGE_VOLTAGE)
        //            return QString::number(m_Nodes[i]->chargeVolt());
        else if(col == COLUMN_Memory_CAPACITY)
            return QString::number(m_Nodes[i]->memory());
        else if(col == COLUMN_DOWNLOADED)
            return m_Nodes[i]->isDownloaded();
    }
    else if(role == Qt::BackgroundColorRole)
    {
        //COLUMN_VOLTAGE = 2
        if(col>=COLUMN_VOLTAGE &&m_Nodes[i]->state())
        {
            Node::State st = m_Nodes[i]->stateById(col-COLUMN_VOLTAGE);
            if(st == Node::State_Warning)
                return QColor(Qt::yellow);
            else if(st == Node::State_Error)
                return QColor(Qt::red);
        }
        //        if(col == COLUMN_VOLTAGE)
        //            return QString::number(m_Nodes[i]->voltage());
        //        else if(col == COLUMN_TEMPERATURE)
        //            return QString::number(m_Nodes[i]->temperature());
        //        else if(col == COLUMN_PRESSURE)
        //            return QString::number(m_Nodes[i]->pressure());
        //        else if(col == COLUMN_ELECURRENT)
        //            return QString::number(m_Nodes[i]->current());
        //        else if(col == COLUMN_CHARGE_VOLTAGE)
        //            return QString::number(m_Nodes[i]->chargeVolt());
        //        else if(col == COLUMN_Memory_CAPACITY)
        //            return QString::number(m_Nodes[i]->memory());
    }
    return QVariant();
}

QVariant StatusItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            if(section == COLUMN_DEVICE_NO)
                return QString("Device.No");
            else if(section == COLUMN_STATUS)
                return QString("State\n(Connected)");
            else if(section == COLUMN_VOLTAGE)
                return QString("Voltage\n(V)");
            else if(section == COLUMN_TEMPERATURE)
                return QString("Temperature\n(℃)");
            else if(section == COLUMN_PRESSURE)
                return QString("Pressure\n(atm)");
            //            else if(section == COLUMN_ELECURRENT)
            //                return QString("EleCurrent\n(A)");
            //            else if(section == COLUMN_CHARGE_VOLTAGE)
            //                return QString("Charge Voltage\n(V)");
            else if(section == COLUMN_Memory_CAPACITY)
                return QString("Memory Capacity\n(G,total:32G)");
            else if(section == COLUMN_DOWNLOADED)
                return QString("Downloaded");
        }
    }
    default:
        return QVariant();
    }

    return QVariant();
}


StatusSortProxyModel::StatusSortProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

bool StatusSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (!left.isValid() || !right.isValid())
        return false;

    int col = left.column();
    QVariant leftData = sourceModel()->data(left, Qt::DisplayRole);
    QVariant rightData = sourceModel()->data(right, Qt::DisplayRole);

    //编号排列
    if(col == COLUMN_DEVICE_NO)
        return leftData<rightData;

    else if(col == COLUMN_STATUS)
    {
        int val_left = leftData.toBool()?0:1;
        int val_right = rightData.toBool()?0:1;
        return val_left<val_right;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}



//Status Report对话框,clonum : 8---------------------------
StatusReportDlg::StatusReportDlg(QWidget *parent)
    :QDialog(parent),ui(new Ui::StatusReportDlg)
{
    ui->setupUi(this);

    this->setWindowTitle("Status Report");

    initDlg();
}

void StatusReportDlg::setLimitData(const QVector<LimitData> &limitData)
{
    m_limitData = limitData;
}

void StatusReportDlg::setCurrentNodeInfo(const QVector<Node*> &nodes)
{
    m_Nodes = nodes;
    m_dataModel->setStatusInfo(nodes);
    ui->tableView->sortByColumn(COLUMN_STATUS);
}

void StatusReportDlg::getMessage(const UpdateCmd &msg,Node *node)
{
    //更新对应节点
    int rw = m_Nodes.indexOf(node);
    //所有状态
    if(State_Update == msg){
        for(int j = 0;j<m_dataModel->rowCount();j++)
            ui->tableView->update(m_sortMdel->mapFromSource(m_dataModel->index(rw,j)));
    }
    //更新容量
    if(Ftp_Update_Memory == msg){
        ui->tableView->update(m_sortMdel->mapFromSource(m_dataModel->index(rw,COLUMN_Memory_CAPACITY)));
    }
}



void StatusReportDlg::on_pushButton_clicked()
{
    this->close();
}

//导出表格内容到表格文件
void StatusReportDlg::on_exportSheetBtn_clicked()
{
    QString outFile = QFileDialog::getSaveFileName(this,"Save File",
                                                   QCoreApplication::applicationDirPath(),"Text File(*txt)");
    if(outFile.isEmpty())
        return;
    if(!outFile.endsWith(".txt"))
        outFile += ".txt";
    QFile file(outFile);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::critical(this,"Error","Save file filed.");
        return ;
    }
    QString text;
    QTextStream out(&file);

    //表头
    for(int column = 0;column<m_sortMdel->columnCount();column++){
        text = m_sortMdel->headerData(column,Qt::Horizontal).toString();
        text.remove("\n");
        out<<text<<"\t";
    }
    out<<endl;

    for(int row=0;row<m_sortMdel->rowCount();row++){
        for(int column = 0;column<m_sortMdel->columnCount();column++)
        {
            text = m_sortMdel->index(row,column).data(Qt::DisplayRole).toString();
            out<<text<<"\t";
        }
        out<<endl;
    }

    file.close();
    QMessageBox::information(this,"information",QString("Already write table into %1").arg(outFile));
}

void StatusReportDlg::initDlg()
{
    m_ItemDelegate = new StatusItemDelegate(this);
    m_dataModel = new StatusItemModel;
    m_sortMdel  = new StatusSortProxyModel(this);
    m_sortMdel->setSourceModel(m_dataModel);

    ui->tableView->setItemDelegate(m_ItemDelegate);
    ui->tableView->setModel(m_sortMdel);

    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    ui->tableView->setColumnWidth(COLUMN_STATUS,120);
    ui->tableView->setColumnWidth(COLUMN_VOLTAGE,120);
    ui->tableView->setColumnWidth(COLUMN_TEMPERATURE,120);
    ui->tableView->setColumnWidth(COLUMN_PRESSURE,120);
    ui->tableView->setColumnWidth(COLUMN_Memory_CAPACITY,180);
    ui->tableView->setSortingEnabled(true);

    ui->tableView->sortByColumn(COLUMN_STATUS,Qt::AscendingOrder);
}



