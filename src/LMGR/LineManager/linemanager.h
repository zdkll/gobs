#ifndef LINEMANAGER_H
#define LINEMANAGER_H

#include <QMainWindow>
#include "gcommmodule.h"

#include  "lmgrunit.h"
#include  "lmgrareaview.h"
#include  "gsqlfunctions.h"

namespace Ui {
class LineManager;
}

class QLabel;
class QSpinBox;
class DeploySystem;
class RNDeployments;
class AssignNodeDlg;
class LineManager : public QMainWindow ,public CommModule
{
    Q_OBJECT

public:
    explicit LineManager(QWidget *parent = 0);
    ~LineManager();

    void setAreaDataInfo(AreaDataInfo *areaDataInfo) {
        m_AreaDataInfo  = areaDataInfo;}

    bool init();

private slots:
    //菜单选项----
    void slotNewArea();
    void slotOpenArea();
    void slotUpdateCurrent(const QString &areaName = QString());
    void slotUpdateAreaSPS();
    void slotCloseCurrent();
    void slotDeleteArea();
    void slotExit();

    //编辑选项----
    void slotRNDeployments();       //显示所有节点的投放信息
    void slotAssignRNDeployments(); //编辑投放

    void slotConfigDeploySys();     //配置扫描系统
    void slotDis_ConDeploySys(const bool&);//链接和断开扫描系统

    //View-------------
    void slotShowRecvs(const bool &);
    void slotShowShots(const bool&);
    void slotShowRecvLines(const bool&);
    void slotShowShotLines(const bool&);
    void slotShowRecvText(const bool&);
    void slotShowShotText(const bool&);
    void slotPreference();

    //Help-----
    void slotAbout();

    //other-toolButton
    void slotZoomIn();
    void slotZoomOut();
    void slotZoomInFull();
    void slotZoomOutFull();

    //-----------------------------
    void slotCreateArea(const QString &areaName);
    void slotUpdateArea();

    //open_delete
    void slotOpenArea(const QString &areaName);
    void slotDeleteArea(const QString &areaName);

    void slotDepolySysConnected(const bool &connected);
    void setDepolySysCommStatus(const bool &connected);

    //接受到设备，刷新工区设备列表信息和投放设备(未投放设备信息)
    void updateRecvedDevice(const QString &areaName);

    //RNDepolyments 更新，更新显示和统计信息
    void slotUpdateRNDepolyments();

    //查看某个站点
    void slotSerachDevice();
    //编辑某个接受站点位置坐标
    void slotupdateRecvCoord(float,int,Coordinate);

private:
    void initWindow();

    void setCurrentAreaData(const QString &areaName,AreaDataInfo *areaDataInfo);

    //设置当前工区名
    void setCurrentArea(const QString &areaName);

    void updateMenuToolStatus(const bool &enable){
        m_menuManger->updateMenuStatus(enable);
        m_toolManager->updateToolBarStatus(enable);
    }

private:
    Ui::LineManager *ui;
    QLabel          *m_ComStatusLbl;
    QLineEdit       *m_searchEdit;
    //菜单管理------
    MenuManager    *m_menuManger;
    ToolManager    *m_toolManager;
    LmgrAreaView   *m_AreaView; //图像显示
    StationsInfoWg *m_StationsInfoWg;

    CreateAreaDialog *m_CreateAreaDialog;//创建工区对话框
    UpdateAreaDialog *m_UpdateAreaDialog;
    //更新工区对话框

    AreaListDlg    *m_AreaListDlg;//打开和删除工区对话框
    AreaDataInfo   *m_AreaDataInfo; //工区sps数据
    QString         m_currentArea;//当前工区名
    StationInfoBar *m_statusBar;

    //设备扫描
    DeploySystem   *m_depolySystem;
    RNDeployments  *m_RNDepolyments;
    AssignNodeDlg  *m_AssignNodeDlg;
};


#endif // LINEMANAGER_H
