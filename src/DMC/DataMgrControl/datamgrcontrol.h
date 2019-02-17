#ifndef DATAMGRCONTROL_H
#define DATAMGRCONTROL_H

#include <QMainWindow>
#include <QDebug>

#include "gcommmodule.h"
#include "dmgrunit.h"
#include "qcplotter.h"

namespace Ui {
class DataMgrControl;
}

//数据管理主面板，控制数据处理抽取，并且提供seg-y数据显示入口
class JobManager;
class DataMgrControl : public QMainWindow, public CommModule
{
    Q_OBJECT
public:
    explicit DataMgrControl(QWidget *parent = 0);
    ~DataMgrControl();

    virtual bool init();

private slots:
    //菜单File选项槽函数
    void slotNewSegyData();
    void slotOpenSegyFile();
    void slotExit();

    //Tools 菜单
    void slotPreviousGather();
    void slotNextGather();
    void slotMoveToGather();

    void slotZoomIn(const bool &enable);
    void slotZoomOut();

    void slotTraceInform(const bool &enable);

    void slotDisplayOption();

    //Help选项槽函数
    void slotAbout();

    //---------------------------------------
    void startExportSegyData(const ExportSegyParameter &parameter);
    //打开新的文件
    void slotOpenedFile(QList<SegyFileHandle*> segyFileHandles);
    //接受到新的道集，读取显示
    void readGather(const int &gatherNo);

    void on_autoRefreshCbx_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_refreshBtn_clicked();

private:
    void initWindow();

    bool saveJobParameter(const QString &jobFile,const ExportSegyParameter &paramter);

private:
    Ui::DataMgrControl *ui;
    MenuManager        *m_menuMnager;
    ToolManager        *m_toolManager;

    ExportSegyDlg      *m_ExportSegyDlg;
    OpenSegyDlg        *m_OpenSegyDlg;
    QCPlotter          *m_QCPlotter;
    PlotParamDlg       *m_paramDlg;

    //当前文件句柄
    QList<SegyFileHandle*> m_FileHandles;
    int                 m_maxShowTraces;
    int                 m_startTrace; //起始道，规划道集后的起始，可以为负值
    GatherRecord       *m_gatherRecord;

    //道头和数据
    TraceHead          *m_headers;
    char               *m_data;

    GatherInfo         *m_gatherInfo;
    JobManager         *m_jobManager;
};

//作业信息显示
class QListWidgetItem;
class JobManager : public QObject
{
    Q_OBJECT
public:
    JobManager(QObject *parent = 0);
    ~JobManager();
    void setProjInfo(const ProjectInfo &projInfo){m_projInfo = projInfo;}
    void init(QListWidget *jobList,QTextBrowser *logBrowser);

    void timerEvent(QTimerEvent *e);

    void refreshFiles();
    void refreshLog();

    void setAutoRefreshEnable(bool enable);
    void setLogRefreshInterval(int interval);

protected slots:
    void slotJobItemClicked(QListWidgetItem*);
    void slotCustomContextMuenuReqested(QPoint);

    void slotShowLog();
    void slotResubmit();

private:
    QListWidget  *m_jobList;
    QTextBrowser *m_logBrowser;
    ProjectInfo   m_projInfo;

    QString       m_currentFile;

    //刷新日志文件和刷新日志信息 时钟
    int          m_fileTimer,m_logTimer;
    int          m_interval;

    QMenu        *menu;
};

#endif // DATAMGRCONTROL_H
