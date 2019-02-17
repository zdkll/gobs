#ifndef LMGRUNIT_H
#define LMGRUNIT_H

#define Menu_Name_File   "File"
#define Menu_Name_Edit   "Edit"
#define Menu_Name_View   "View"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QButtonGroup>
#include <QGroupBox>

#include <QListWidget>
#include <QStandardItem>
#include <QStandardItemModel>

class QPushButton;

class QCheckBox;
class QProgressBar;


#include "gcore.h"
#include "gpublic.h"
#include "lmgrpublic.h"
#include "gpublicwidgets.h"

class LmgrUnit
{
public:
    LmgrUnit();
};

//站点总体信息显示控件
class StationsInfoWg : public QWidget
{
public:
    StationsInfoWg(QWidget *parent = 0);

    void setStationNum(const int &shots,const int recvs);

    void setRNInfoNum(const int &depolyed,const int &pickedUp);

    void clearData();

private:
    void initWg();

    QLabel *m_shotsLbl,*m_recvLbl,*m_deployedLbl,*m_pickedLbl;
};

/*菜单控制管理--------------------
 *File:
 *Edit
 *View
 *Help
 */
class LineManager;
class MenuManager : public QObject
{
public:
    MenuManager(QObject *parent = 0):QObject(parent),m_enable(true){}
    void createMenu(QMenuBar *menuBar,LineManager *parent);

    // 更新菜单,根据当期是否有显示区域更新状态
    void updateMenuStatus(const bool &enable);

    void setDeploySysConnected(const bool &connected);

private:
    QMenu *menuFile,*menuEdit,*menuView;
    bool   m_enable;
    QAction *dis_con_DpySysAct;
};

/*工具栏管理----------
 *
 *New Open Close | Delete | Update | ZoomIn ZoomOut ZoomIn Full Zoom Out Full | ItemList
 */
class ToolManager : public QObject
{
public:
    ToolManager(QObject *parent = 0):QObject(parent),m_enable(true){}
    void createToolBar(QToolBar *toolBar,LineManager *parent);

    //更新状态栏，根据是否有显示区域设置选项使能
    void updateToolBarStatus(const bool &enable);

private:
    QToolBar *m_toolBar;
    bool   m_enable;
};

//工区对话框基类
class AbstractAreaDialog : public QDialog
{
    Q_OBJECT
public:
    enum  AreaOption
    {
        CreateArea = 0,
        UpdateArea = 1
    };
    enum UpdateOption
    {
        ReplaceSps = 0,
        MergeSps   = 1
    };
    AbstractAreaDialog(QWidget *parent = 0);

    ~AbstractAreaDialog();

    void setProjectInfo(const QString &projPath,const QString &projName){
        m_projInfo.ProjectPath = projPath;
        m_projInfo.ProjectName = projName;
    }
    void setProjectInfo(const ProjectInfo &info){
        m_projInfo = info;
    }

    void setAreaOption(AreaOption option);

    AreaDataInfo *areaDataInfo()const {return m_AreaDatainfo;}
    QString areaName() const {return  m_areaNameEdt->text();}

    QStringList spsFiles() const {return m_spsFiles;}

    AreaOption  areaOption(){return m_areaOpt;}
    UpdateOption updateOption(){return m_updateOpt;}

public slots:
    //进度刷新------------
    void setProgressValue(const int &value);
    //progress hint------
    void setOperateHint(const QString &hint);

    void setProgressInfo(const QString &hint,const int &value){
        setProgressValue(value);
        setOperateHint(hint);
    }

protected :
    void setOptBtnText(const QString &text);
    void setAreaEditReadOnly(const bool &);
    void setAreaName(const QString &areaName);

    //结果
    QString rFileName() const {return m_RfileEdt->text();}
    QString sFileName() const {return m_SfileEdt->text();}
    QString xFileName() const {return m_XfileEdt->text();}

protected slots:
    void slotBrowserSPSFile();
    void slotOperate();

    void slotReplaceUpdate();
    void slotMergeUpdate();

    //继承实现
    virtual void run() = 0;
    virtual void slotHelp() = 0;

signals:
    //点击对应的操作后发送信号
    void signalCreateArea(const QString &areaName);
    void signalUpdateArea();
    void signalDeleteArea(const QString &areaName);

protected:
    //当前项目信息
    ProjectInfo     m_projInfo;
    //数据-------------------
    AreaDataInfo   *m_AreaDatainfo;

    AreaOption      m_areaOpt;
    UpdateOption    m_updateOpt;

private:
    //基类创建基类的界面
    void createBaseDlg();
    void autoDetectFile(const QString &type);

    QLineEdit   *m_areaNameEdt;
    QLineEdit   *m_RfileEdt,*m_SfileEdt,*m_XfileEdt;

    QLabel       *m_progressHintLbl;
    QProgressBar *m_progressBar;
    QPushButton  *m_operateBtn;
    QCheckBox    *m_autoFileDectCbx;


    QStringList   m_spsFiles;
    bool          first_choose;
    QGroupBox    *m_updateGrpBox;
};

//创建工区
class CreateAreaDialog : public AbstractAreaDialog
{
    Q_OBJECT
public:
    CreateAreaDialog(QWidget *parent = 0);

protected slots:
    void run();
    void slotHelp();

private:

};

//更新工区--
class UpdateAreaDialog : public AbstractAreaDialog
{
    Q_OBJECT
public:
    UpdateAreaDialog(QWidget *parent = 0);

    void  updateArea(const QString &areaName);

protected slots:
    void run();
    void slotHelp();

private:

};

//项目工区列表
class AreaListDlg : public QDialog
{
    Q_OBJECT
public:
    enum AreaOpt
    {
        Open_Area   = 1,//打开工区
        Delete_Area = 2 //删除工区
    };
    AreaListDlg(QWidget *parent = 0);
    ~AreaListDlg();

    void setAreaOpt(const AreaOpt &opt);
    void setProjInfo(const ProjectInfo &projInfo){m_projInfo = projInfo;}

    //更新列表
    void updateAreaList();

    //显示工区列表
    void showAreaList();

signals:
    void signalOpenArea(const QString &areaName);
    void signalDeleteArea(const QString &areaName);

protected slots:
    void slotOk();

private:
    void initDlg();
    ProjectInfo          m_projInfo;
    AreaOpt              m_areaOpt;
    QListWidget         *m_listWidget;
};


class PreferenceDlg : public QDialog
{
    Q_OBJECT
public:
    PreferenceDlg(QWidget *parent = 0);
    QMap<int,QColor> colorMap()const{return m_colorMap;}

signals:
    void colorChanged(const int &id,const QColor &color);
    void colorMapChanged(const QMap<int,QColor> &colorMap);

private:
    void initDlg();

private slots:
    void slotOkBtn();
    void slotCancelBtn();

private:
    QButtonGroup    *m_btnGroup;
    ColorPickButton *m_bkColorBtn,*m_shotColorBtn,*m_firedShotColorBtn,*m_recvColorBtn\
    ,*m_shotLineColorBtn,*m_recvLineColorBtn,*m_legendColorBtn
    ,*m_deployedColorBtn,*m_pickedColorBtn,*m_downloadedColorBtn,*m_shotTextColorBtn
    ,*m_recvTextColorBtn;

    QMap<int,QColor> m_colorMap;
};

#endif // LMGRUNIT_H
