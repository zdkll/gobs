#ifndef DEPLOYSYSTEM_H
#define DEPLOYSYSTEM_H

#include <QDialog>
#include <QTcpSocket>

#include "gpublic.h"
#include "areanodemanager_global.h"

namespace Ui {
class DeploySystem;
}

//设备扫描系统
class AREANODEMANAGERSHARED_EXPORT DeploySystem : public QDialog
{
    Q_OBJECT

public:
    enum WorkStatus
    {
        Depolyment = 0,
        PickUp     = 1,
        Both       = 2
    };
    explicit DeploySystem(QWidget *parent = 0);
    ~DeploySystem();

    void setProjInfo(const ProjectInfo &projInfo);
    void setCurrentArea(const QString &area);
    void addNewArea(const QString &area);
    void deleteArea(const QString &area);

    void updateDownload();
    void connectToHost(const bool &con);

protected:
    void timerEvent(QTimerEvent *e);

signals:
    void signalDepolySysConnected(const bool &);
    void signalDepolySysStateChanged(const bool &);

    //某个接受到设备，发送刷新信号
    void signalUpdateDevice(const QString &areaName);


private slots:
    void slotSocketStateChanged(const QAbstractSocket::SocketState &state);
    void slotSocketReadyRead();

    void on_clearBtn_clicked();

    void on_closeBtn_clicked();

    void on_ConCbx_clicked(bool checked);

    void on_workStatusCbx_currentIndexChanged(int index);

    void on_areaCbx_currentTextChanged(const QString &arg1);

    void on_addBtn_clicked();

    void on_closeBtn2_clicked();

    void on_pushButton_clicked();

private:
    //更新工区列表
    void      updateAreaList();

    bool      addOrUpdateRecord(QString areaName,QString device
                               ,QString ipaddress,WorkStatus workStatus);

    Ui::DeploySystem *ui;
    QTcpSocket       *m_socket;
    WorkStatus        m_workStatus;

    ProjectInfo       m_projInfo;

    QByteArray        m_currentData;
    int               m_processTimer;
    QString           m_currentArea;
    QString           m_errString;
    QStringList       m_downloadedGOBS;
};


#endif // DEPLOYSYSTEM_H
