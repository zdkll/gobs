#ifndef RNFTPMANAGER_H
#define RNFTPMANAGER_H

#include <QDialog>
#include <QTableView>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>

#include "gpublic.h"
#include "rnitems.h"
#include "gpublicfunctions.h"
#include "rnmbaselib_global.h"


namespace Ui {
class RNFtpManager;
}

class FtpTableModel : public QAbstractTableModel
{
public:
    FtpTableModel(QObject *parent = 0);
    void setNodes(QVector<Node *> &);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool    setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant  headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    //QModelIndex index(int row, int column, const QModelIndex &parent) const;
private:
    QVector<Node *>  m_nodes;
    QVector<bool>    m_selecteds;
};

class FtpSortProxyModel : public QSortFilterProxyModel
{
public:
    FtpSortProxyModel(QObject *parent = 0);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

};

class FtpItemDelegate : public QStyledItemDelegate
{
public:
    FtpItemDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:

};

class NodeMediator;
class QDateTimeEdit;
//Ftp 文件管理操作
class RNMBASELIBSHARED_EXPORT RNFtpManager : public QDialog
{
    Q_OBJECT

public:
    explicit RNFtpManager(QWidget *parent = 0);
    ~RNFtpManager();

    void setNodeMediator(NodeMediator *nodeMediator){m_nodeMediator = nodeMediator;}

    void setProjectInfo(const ProjectInfo &projInfo);
    void setNodesInfo(const QVector<Node *> &);

    //参数更新
    void getMessage(Node *node,const UpdateCmd &msg);

    //完成当前任务
    void finishedFtpWork(Node *node,const int &ftpWk,const bool &err);

private slots:
    //下载
    void on_downloadBtn_clicked();

    //停止当前任务
    void on_stopCurrentBtn_clicked();

    //清除内存
    void on_clearMemBtn_clicked();
    //设置采样率
    void on_setSampleFreqBtn_clicked();
    //更新
    void on_refreshBtn_clicked();

    //其他slot-------------------------------
    void on_closeBtn_clicked();

    void on_brwserBtn_clicked();

    void on_checkBox_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_timePeriodCbx_toggled(bool checked);

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_clearBtn_clicked();

private:
    void initDlg();
    void refreshNodes();

    //设置界面功能按钮使能，开始工作:Disable，结束或者停止后：Enable
    void setUiBtnEnable(const bool &enable);

    //-----------------------------------------------
    QString getWorkName(const FtpWork &work) const;

    //获取等待工作节点
    QList<Node *> regetWaitingNodes(const bool &needSelected = true);

    //开始任务
    void     startFtpWork(const FtpWork &ftpWk,const QVariant &arg = QVariant());
    //开始下一个节点
    void     startNextNode(const FtpWork &ftpWk,const QVariant &arg = QVariant());

private:
    Ui::RNFtpManager  *ui;
    ProjectInfo        m_projInfo;
    //----------------------------------------------------
    QVector<Node *>    m_nodes;         //所有节点

    QList<Node *>      m_working_nodes; //正在working节点
    QList<Node *>      m_waiting_nodes;  //等待调度节点

    NodeMediator      *m_nodeMediator;

    FtpTableModel     *m_tableModel;
    FtpSortProxyModel *m_sortModel;
    QString            m_dataPath;

    FtpWork            m_ftpWork;
    int                m_sampleFrequency;

    //最大同时Ftp工作节点数
    int                m_maxWorkNodes;
    //控件
    QDateTimeEdit     *m_fromTimeEdit,*m_toTimeEdit;

    bool               m_stoped;
    bool               m_nodeNeedSelected;

    //下载方式，是否按照时间段下载
    DownloadType      m_downloadType;
};
#endif // RNFTPMANAGER_H
