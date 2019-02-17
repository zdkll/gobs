#ifndef RNM_MENU_TOOL
#define RNM_MENU_TOOL

#define Menu_Name_File   "File"
#define Menu_Name_Set    "Set"
#define Menu_Name_View   "View"
#define Menu_Name_About  "About"

#define SearchIP_Button_Search  "Search"
#define SearchIP_Button_Stop    "Stop"
#define Search_Button_Canceling "Canceling"

#define SearchIP_Dialog_Width   880
#define SearchIP_Dialog_height  560
#define Node_Connected          "Connected"
#define Node_Not_Avaliable      "N/A"

#define Node_Role_IP            Qt::UserRole + 1
#define Node_Role_State         Qt::UserRole + 2
#define Node_Role_DevNo         Qt::UserRole + 3

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QDialog>
#include <QLabel>
#include <QFontComboBox>
#include <QRadioButton>
#include <QLine>
#include <QProgressBar>
#include <QCheckBox>
#include <QFrame>
#include <QBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>



#include "gcore.h"
#include "gpublic.h"
#include "gpublicwidgets.h"

#include "rnmpublic.h"

class QTableWidget;
class SearchThread;
class QSpinBox;

class RNManager;
class MenuManager : public QObject
{
    Q_OBJECT
public:
    MenuManager(QObject *parent = 0);
    void createMenu(QMenuBar *menuBar,RNManager *parent);

    void setUpdateStatus(const bool &start);

    //状态更新,已经显示节点
    void setDisplayedNode(const bool &display);

    void setDisplayFilters(const int &status);

signals:
    void signalUpdateIntervalChanged(const int &interval);

private slots:
    void slotUpdateIntervalChanged(QAbstractButton *);

private:
    QMenu         *menuFile,*menuEdit,*menuView,*menuAbout;
    QAction       *start_stop_update_act;
    QButtonGroup  *filtersGroupBox;
};

//工具栏管理
class ToolManager:public QObject
{
    Q_OBJECT
public:
    ToolManager(QObject *parent = 0);
    void createToolBar(QToolBar *toolBar,RNManager *parent);
    void setUpdateStatus(const bool &start);

    //状态更新,已经显示节点
    void setDisplayedNode(const bool &display);

signals:
    void signalUpdateIntervalChanged(const int &interval);
    void signalStartOrStopUpdate(const bool &start);

    void signalSearchNode(const uint &node);

private slots:
    void slotSetUpdateInterval(const QString &text);
    void slotStartOrStopUpdate();
    void slotSearchNode();

private:
    QToolBar  *m_toolBar;
    QAction   *updateAction;
    QString    m_Path;
    QLineEdit *m_searchEdit;
};


//File选项set search scope对话框
class SearchIpDialog : public QDialog
{
    Q_OBJECT
public:
    SearchIpDialog(QWidget *parent = 0);
    void setProjectInfo(ProjectInfo projectInfo){m_projectInfo = projectInfo;}
    ~SearchIpDialog();

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void displayNodes(const QVector<Node*> &nodes);

private slots:
    void checkLcHostIP();
    void slotSearchBtnClicked();
    void slotDisplayNodes();
    void slotSearchedHost(const quint32 &index,const bool &available);
    void slotThreadFinished(const int &id);

private:
    void initDlg();
    void searchIpScope(QVector<uint> &hosts);
    void stopSearch();

    //查找IP段中所有的设备
    QVector<uint> findNodesFromScope(const quint32 &start,const quint32 &last);

private:
    //窗体中的控件
    QLabel       *m_localHostIp,*m_totalNodesLbl,*m_SearchedNodesLbl;
    IpAddrEdit   *m_ipFromEdit,*m_ipToEdit;
    QProgressBar *m_progressBar;
    QTableWidget *m_hostsTab;
    QSpinBox     *m_wtimeSpx;

    QPushButton  *m_searchBtn,*m_displayBtn,*m_closeBtn;

    QMap<int,SearchThread *> m_threads;
    int           m_searchedNodes;
    quint32       m_startIP,m_lastIP;
    QVector<uint> m_totalIp;

    QVector<Node *>  m_Nodes;
    ProjectInfo      m_projectInfo;
};


class LimitWg : public QWidget
{

public:
    LimitWg(const Limit &limit,QWidget *parent = 0);

    static QString logicText(const LimitOperation &Opt)
    {return (Opt == Big)?">":"<";}

    void  setId(const int &id){m_id = id;}
    void  setLimit(const Limit &limit);
    void  setBuddy(LimitWg *bddyWg){m_buddyWg = bddyWg;}
    void  setValidator(QValidator *validator){m_valEdit->setValidator(validator);}

    void reset();

    Limit defultLimit() const {return m_limit;}

    Limit limit(bool *ok = 0)  ;


private:
    void initWg();

    ColorFrame  *m_colorFrame;
    QLabel      *m_logicLbl;
    QLineEdit   *m_valEdit;
    QLabel      *m_suffixLbl;
    Limit        m_limit;
    LimitWg     *m_buddyWg;
    int          m_id;
    float        m_defultVal;
};

class LimitGroupBox : public QGroupBox
{
public:
    explicit LimitGroupBox(QWidget* parent=0);
    explicit LimitGroupBox(const QString &title, QWidget* parent=0);

    int  id() const{return m_id;}
    void setId(const int&id){m_id = id;}

    void  addLimit(const Limit &limit);
    QList<LimitWg*> limitWidgets() const{return m_childWidgets;}

    QList<Limit>  limits(bool *ok = 0) ;

    LimitData limitData(bool *ok = 0) ;

    void setValidator(QValidator *validator);

    const QString &errString(){return m_errString;}

private:
    void initLayout();

    QVBoxLayout    *mainLayout;
    QList<LimitWg*> m_childWidgets;
    int             m_id;
    QString         m_errString;
    QDoubleValidator     *m_validator;
};

//参数状态显示限制编辑对话框
class LimitsDialog : public QDialog
{
    Q_OBJECT
public:
    LimitsDialog(QWidget *parent = 0);

    LimitInfo getLimitInfo(const int &status) ;

    QVector<LimitData> limitData() const {return m_limitData;}

signals:
    void limitsChanged(const QVector<LimitData> &);
    void limitsChanged();

protected:

private slots:
    void cancelBtnClicked();
    void okBtnClicked();

private:
    void initDlg();

    QList<LimitGroupBox *>  m_groupBoxs;
    QVector<LimitData>      m_limitData;
};


#endif // RNM_MENU_TOOL

