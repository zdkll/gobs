#ifndef RNMSIDEBAR_H
#define RNMSIDEBAR_H

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>

#include "gpublic.h"
#include "rnmgrunit.h"


namespace Ui {
class RNMSideBar;
}


//statics Box
class StatisticsBox : public QGroupBox
{
public:
    StatisticsBox(QWidget *parent = 0);

    void setStatisticsData(const StatisticsData &data);

private:
    void initBox();

    QLabel *m_totalNodeslbl,*m_validNodeslbl,*m_nodeBoxlbl;
    QLabel *m_updateIntervallbl,*m_downloadNodeslbl;
};

//Display Filters
class FiltersBox : public QGroupBox
{
    Q_OBJECT
public:
    FiltersBox(QWidget *parent = 0);
    void setDisplayedNodes(const bool &displayed);

    void setDisplayFilters(const int &status);

signals:
    void displayFiltersChanged(const int &status);
    void reportNodeStatus();

private :
    void initBox();
    QPushButton   *reportBtn;
    QButtonGroup  *groupBox;
};

//Legend
class LegendBox : public QGroupBox
{
    Q_OBJECT
public:
    LegendBox(LimitsDialog *dlg,QWidget *parent = 0);
    void setLimitInfo(const LimitInfo &limitInfo);

    QString logicText(const LimitOperation &operate) const{
        return (operate == Big)?"GT":"LT";
    }

signals:
    void reportNodeStatus();

public slots:
    void displayFiltersChanged(const int &filters);
    void limitChanged();

private :
    void initBox();

    //两个成员：当前显示状态，当前参数Limits 对象，随时查询所显示状态的参数限制值
    int              m_status;
    LimitsDialog    *m_LimitsDlg;
    QGridLayout     *mainLayout;
    QList<QWidget *> m_childWidgets;
};

//侧边栏管理
class RNMSideBar : public QWidget
{
    Q_OBJECT
public:
    explicit RNMSideBar(LimitsDialog *dlg,QWidget *parent = 0);
    ~RNMSideBar();
    void setDisplayFilters(const int &status);

    void setLimitLegend(const LimitInfo &limitInfo);

    void setDisplayedNode(const bool &displayed);

    void setStatisticsData(const StatisticsData &data);

signals:
    void reportNodeStatus();
    void displayFiltersChanged(const int&);

private:
    void initWg();

private:
    Ui::RNMSideBar *ui;

private:
    //侧边栏所需控件
    StatisticsBox *m_statisticsBox;
    FiltersBox    *m_filtersBox;
    LegendBox     *m_legendBox;

    LimitsDialog  *m_limitDialog;
};



#endif // RNMSIDEBAR_H
