#ifndef ASSIGNNODEDLG_H
#define ASSIGNNODEDLG_H

#include <QDialog>
#include <QAbstractTableModel>

#include "gpublic.h"
#include "lmgrpublic.h"
#include "areanodemanager_global.h"

namespace Ui {
class AssignNodeDlg;
}

struct NodeDevice
{
    NodeDevice():eidted(false){}
    QString Device; //设备名
    float   line;
    int     station;
    bool    eidted; //是否编辑，如果为真，需要保存，否则不做处理
};

//设备模型
class RNTableModel : public QAbstractTableModel
{
public:
    RNTableModel(QObject *parent = 0);
    void setSourceData(QList<NodeDevice *> unassignedDevices);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    QVariant  data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation
                        , int role = Qt::DisplayRole) const;

private:
    QList<NodeDevice *>  m_unassignedDevices;

};

//投放模型
class DepolyModel : public QAbstractTableModel
{
public:
    DepolyModel(QObject *parent = 0);
    //
    void setSourceData(StaLine &stalime,QMap<int,NodeDevice *> assignedDevices);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role ) const;

    QVariant headerData(int section, Qt::Orientation orientation
                        , int role = Qt::DisplayRole) const;
private:
    //站点线
    StaLine                m_staLine;
    //该条线对应的设备列表<站点号，设备>
    QMap<int,NodeDevice *> m_assignedDevices;
};

class AREANODEMANAGERSHARED_EXPORT AssignNodeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AssignNodeDlg(QWidget *parent = 0);
    ~AssignNodeDlg();

    void setProjInfo(const ProjectInfo &projInfo){m_projInfo = projInfo;}
    void setCurrentArea(const QString &area){m_currentArea = area;}
    void addNewArea(const QString &area);
    void deleteArea(const QString &area);

    void initDepolyments();

signals:
    void signalUpdateAssignedDev(const QString &areaName);

private slots:
    void slotCurrentAreaChanged(const QString &area);

    void on_assignBtn_clicked();

    void on_unassignBtn_clicked();

    void on_moveUpBtn_clicked();//上下移动只对当前Item 一项

    void on_moveDownBtn_clicked();

    void on_lineCbx_currentIndexChanged(int index);

    void on_filterEdit_textChanged(const QString &arg1);

    void slotDepolySelectionChanged();

    void slotRNSelectionChanged();

    void on_okBtn_clicked();

    void on_applyBtn_clicked();

    void on_cancelBtn_clicked();

    void on_updateDev_clicked();

private:
    void initDlg();

    void initDisplay();

    //读取sps 文件初始化线号，并且读取站点信息
    bool initLineStation();
    //初始化未分配设备
    bool initUnassignedDevices();
    //初始化该条线对应的设备
    bool initStationDevicesByLine(const StaLine &staline);

    bool depolyDevice(NodeDevice *device);
    bool unDepolyDevice(NodeDevice *device);

    void addEditedDevice(NodeDevice *device);

    //保存更改，更新数据库
    bool saveEditedDevice();

private:
    Ui::AssignNodeDlg *ui;

    ProjectInfo        m_projInfo;

    QString            m_currentArea;
    float              m_currentLine;

    StationInfo        *m_stationInfo;
    QList<NodeDevice *> m_assignedDevices;   //当前线投放的设备
    QList<NodeDevice *> m_unAssignedDevices; //总的未投放设备
    QList<NodeDevice *> m_editedDevices;    //编辑过设备，应用时候需要保存
};

#endif // ASSIGNNODEDLG_H
