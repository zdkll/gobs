#ifndef STATUSREPORT_H
#define STATUSREPORT_H

#include <QDialog>

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QModelIndex>

#include "rnmpublic.h"
#include "rnmbaselib_global.h"


namespace Ui {
class StatusReportDlg;
}


class StatusItemDelegate : public QStyledItemDelegate
{
public:
    StatusItemDelegate(QObject *parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const ;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

//StatusReport 的数据显示模型
class StatusItemModel : public QAbstractTableModel
{
public:
    void setStatusInfo(const QVector<Node*> &nodes);
    int rowCount(const QModelIndex &parent  = QModelIndex()) const;
    int columnCount(const QModelIndex &parent  = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role  = Qt::DisplayRole) const;

private:
    QVector<Node*> m_Nodes;
};


class StatusSortProxyModel : public QSortFilterProxyModel
{
public:
    StatusSortProxyModel(QObject *parent = 0);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

};

//Status Report对话框
class  RNMBASELIBSHARED_EXPORT StatusReportDlg : public QDialog
{
    Q_OBJECT
public:
    StatusReportDlg(QWidget *parent = 0);
    void setLimitData(const QVector<LimitData> &limitData);

    void setCurrentNodeInfo(const QVector<Node*> &nodes);

    void getMessage(const UpdateCmd &msg,Node *node);


private slots:
    void on_pushButton_clicked();

    void on_exportSheetBtn_clicked();

private:
    void initDlg();

    Ui::StatusReportDlg   *ui;
    StatusItemModel       *m_dataModel;
    StatusSortProxyModel  *m_sortMdel;
    StatusItemDelegate    *m_ItemDelegate;
    QVector<Node*>         m_Nodes;
    QVector<LimitData>     m_limitData;
};

#endif // STATUSREPORT_H
