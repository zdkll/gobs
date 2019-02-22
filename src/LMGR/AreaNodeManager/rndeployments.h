#ifndef RNDEPLOYMENTS_H
#define RNDEPLOYMENTS_H

#include <QDialog>
#include <QDateTime>

#include "gpublic.h"
#include "areanodemanager_global.h"

namespace Ui {
class RNDeployments;
}

class QLineEdit;
class QLabel;
class QDateTimeEdit;
enum NodeTime
{
    DepolyTime = 0,
    PickedupTime = 1
};

struct Coordinate
{
    float x;
    float y;
    float z;
};

struct NodeTimeInfo
{
    QString   areaName;
    QString   nodeDevice;
    NodeTime  timeType;
    QDateTime dateTime;
};

//坐标编辑对话框
class AREANODEMANAGERSHARED_EXPORT EditCoordDlg : public QDialog
{
    Q_OBJECT
public:
    EditCoordDlg(QWidget *parent = 0);

    void editCurrentCoord(const Coordinate &currentCord);

signals:
    void editedCurrentCoord(const Coordinate &Cord);

private slots:
    void slotOkBtnClicked();
    void slotCancelBtnClicked();

private:
    void initDlg();

    QLineEdit *m_xCoordEdit,*m_yCoordEdit,*m_zCoordEdit;
};


//时间编辑对话框
class AREANODEMANAGERSHARED_EXPORT EditDataTimeDlg : public QDialog
{
    Q_OBJECT
public:
    EditDataTimeDlg(QWidget *parent = 0);

    void editDataTime(const NodeTimeInfo &timeInfo);

signals:
    void editedDataTime(const NodeTimeInfo &timeInfo);

private slots:
    void slotOkBtnClicked();
    void slotCancelBtnClicked();

private:
    void         initDlg();
    NodeTimeInfo m_nodeTimeInfo;

    QLabel          *m_areaLbl,*m_rnIDLbl;
    QDateTimeEdit   *m_datatimeEdit;
};

struct RemoteNode
{
    QString device;

    QString area;
    float   line;
    int     station;

    QString depolyedTime;
    QString pickedupTime;
    float  actualX;
    float  actualY;
    float  actualZ;
    int    depolyed;
    int    pickedup;
    int    downloaded;
};

//显示设备总体信息，统一的映射信息管理，包括界面显示的设备信息来自这里
class AREANODEMANAGERSHARED_EXPORT RNDeployments : public QDialog
{
    Q_OBJECT

public:
    explicit RNDeployments(QWidget *parent = 0);
    ~RNDeployments();

    void setCurrentArea(const QString &area);
    void closeCurrentArea();
    void setProjInfo(ProjectInfo projInfo);

    //初始化显示信息
    void initDepolyments(bool updateDownload = false);
    RemoteNode *findRemoteNode(const float &line,const int &station);
    int pickedupCount()const{return m_pickedUpCount;}
    int depolyedCount()const{return m_depolyedCount;}

signals:
    void signalUpdateRNDepolyments();
    //更新接收点坐标
    void signalUpdateRecvCoord(float line ,int staion,Coordinate cord);

private slots:
    void on_deleteDepolysBtn_clicked();

    void on_editPickupTimeBtn_clicked();

    void on_editDepoyTimeBtn_clicked();

    void on_editCoordBtn_clicked();

    void on_refreshBtn_clicked();

    void on_closeBtn_clicked();

    void on_importDepolyBtn_clicked();

    void on_exportSheetBtn_clicked();

    void slotEditedCurrentCoord(const Coordinate &);
    void slotEditedDateTime(const NodeTimeInfo &);
    void importDepolyments(const QString &fileName);

    int  lineNo(const QString &name);

private:
    Ui::RNDeployments *ui;

    QString        m_currentArea;
    //投放和回收节点数
    int            m_depolyedCount;
    int            m_pickedUpCount;
    ProjectInfo    m_projInfo;

    //编辑xyz 坐标
    EditCoordDlg         *m_editCordDlg;
    //编辑投放回收时间
    EditDataTimeDlg      *m_editDateTimeDlg;
    QVector<RemoteNode *> m_nodeDevices;
    //项目下载数据完成的GOBS
    QStringList           m_downloadedGOBS;
};

#endif // RNDEPLOYMENTS_H
