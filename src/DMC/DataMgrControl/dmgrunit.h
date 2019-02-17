#ifndef DMGRUNIT_H
#define DMGRUNIT_H

#include <QtWidgets/QWidget>
#include <QDialog>
#include <QButtonGroup>
#include <QTextBrowser>
#include <QTableWidget>
#include <QSpinBox>

#include "gpublic.h"
#include "gpublicwidgets.h"
#include "gcore.h"
#include "qcplotter.h"

namespace Ui {
class ExportSegyDlg;
}

class DataMgrControl;
class QToolBar;
class QToolButton;
class QMenuBar;
class QMenu;
class MenuManager : public QObject
{
    Q_OBJECT
public:
    MenuManager(QObject *parent = 0);

    void createMenu(DataMgrControl *,QMenuBar *bar);

private:
    QMenu   *fileMenu,*toolsMenu,*helpMenu;
};

class ToolManager : public QObject
{
    Q_OBJECT
public:
    ToolManager(QObject *parent = 0);
    void createTools(DataMgrControl *,QToolBar *toolBar);

signals:
    void signalZoomInEnable(const bool &enable);
    void signalTraceInformEnable(const bool &en);

private slots:
    void slotZoomIn(bool );
    void slotTraceInformation(bool);

private:
    DownToolButton *m_zoomInBtn;
    DownToolButton *m_traceInfBtn;
};

namespace Ui {
class OpenSegyDlg;
}
class FileDetailDlg;
class OpenSegyDlg : public QDialog
{
    Q_OBJECT
public:
    enum OpenFilesType
    {
        One_Component  = 1,
        Four_Component = 4
    };
    OpenSegyDlg(QWidget *parent = 0);
    ~OpenSegyDlg();

    int maxShowTraces() const {return m_maxShowTraces;}
    int components()const{return m_OpenFilesType;}
    int startTrace() const {return m_startTrace;}
signals:
    void signalOpenedFile(QList<SegyFileHandle *>);

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_detailBtn_clicked();

    void on_OneComRbtn_clicked();

    void on_FourComRbtn_clicked();

    void slotBrwBtnClicked(int id);

private:
    bool openSegyFile(const QStringList &fileName);
    bool getFileList(QStringList &fileList);

private:
    Ui::OpenSegyDlg  *ui;
    OpenFilesType     m_OpenFilesType;
    QList<QLineEdit *>m_fileNameEdits;
    QList<SegyFileHandle *> m_segyFileHandles;

    //文件句柄
    int               m_maxShowTraces;
    int               m_startTrace;
    FileDetailDlg    *m_fileDetailDlg;

    QButtonGroup     *m_brwserBtnGrp;

    QString           m_currentPath; //保存当前数据文件目录，方便下次选择文件

};


//segy 显示道集管理类
class GatherRecord : public QDialog
{
    Q_OBJECT
public:
    GatherRecord(QWidget *parent = 0);
    void setGatherInfo(const int &totalTraces,const int &maxTraces,const int &startTrace);

    void readNextGather();
    void readPrevGather();

    int  gatherNumer()const{return m_gatherNum;}
    int  gatherNo()const{return m_CurrentGather;}

signals:
    void signalReadGather(const int &gatherNo);

private slots:
    void slotOkClicked();
    void slotCloseClicked();

private:
    void initDlg();
    //工具栏两个前后道集功能选项，控制使能
    QAction    *m_prevAction,*m_nextAction;

    QLabel     *m_readLabel;
    QSpinBox   *m_gatherSpx;
    int         m_CurrentGather;
    int         m_gatherNum;
};


//输出Segy数据文件
class ShotLineDlg;
class ExportSegyDlg : public QDialog
{
    Q_OBJECT
public:
    ExportSegyDlg(QWidget *parent = 0);
    ~ExportSegyDlg();
    void setProjectInfo(const ProjectInfo &projInfo);

signals:
    void signalStartExport(const ExportSegyParameter &parameter);

private slots:
    void on_dataPathBrwser_clicked();

    void on_outputPathBrwser_clicked();

    //开始运行抽取道集
    void on_startBtn_clicked();

    void on_CloseBtn_clicked();

    void on_ShotLineBrwser_clicked();

    void on_areasCbx_currentIndexChanged(const QString &arg1);

    void on_recvRbtn_clicked(bool checked);

    void on_shotRbtn_clicked(bool checked);

    void on_fromLineSpx_valueChanged(int arg1);

    void on_toLineSpx_valueChanged(int arg1);

    void on_fromStaSpx_valueChanged(int arg1);

    void on_toStaSpx_valueChanged(int arg1);

private:
    //根据当前项目初始化
    void initByProject();
    void updateScopeInfo();

private:
    Ui::ExportSegyDlg  *ui;
    ProjectInfo         m_projInfo;
    ExportSegyParameter m_exportParameters;
    ShotLineDlg        *m_shotLineDlg;

    AreaDataInfo       *m_areaDataInfo;
};

//文件列表
class QListWidget;
class ShotLineDlg : public QDialog
{
    Q_OBJECT
public:
    ShotLineDlg(QDialog *parent = 0);
    void setShotLineFiles(const QStringList &fileList);

    bool saveShotLineFiles(const QString &fileName); //保存
    QStringList shotLineFiles() const{return m_shotLineFiles;}

private:
    void         initDlg();
    QListWidget *listWidget;
    QStringList  m_shotLineFiles;
};

//图像参数面板
namespace Ui {
class PlotParamDlg;
}
class PlotParamDlg: public QDialog
{
    Q_OBJECT
public:
    PlotParamDlg(QCPlotter *plotter,QWidget *parent = 0);

signals:

private slots:
    void on_positiveCbx_toggled(bool checked);

    void on_negativeCbx_toggled(bool checked);

    void on_ScaleScopeCbx_currentIndexChanged(int index);

    void on_gainEdit_textChanged(const QString &arg1);

    void on_okBtn_clicked();

    void on_horiAxesLbl_currentIndexChanged(const QString &arg1);

    void on_vertAxesLbl_currentIndexChanged(int index);

    void on_horiAxesMinLenEdit_valueChanged(int arg1);

    void on_vertAxesMinLenEdit_valueChanged(int arg1);

    void on_showHoriGridCbx_clicked(bool checked);

    void on_traceIncSpx_valueChanged(int arg1);

    //---------------------
    void slotGraphModeChanged(int mode,bool );

    void slotInterpolationChanged(int interType);

    void on_startTimeEdit_editingFinished();

    void on_endTimeEdit_editingFinished();

private:
    void initDlg();

private:
    Ui::PlotParamDlg *ui;
    QCPlotter        *m_QCPlotter;
};

//
class  FileDetailDlg : public QDialog
{
    Q_OBJECT
public:
    FileDetailDlg(QWidget *parent = 0);

    void setSegyFileInfo(char *textHeader,BinaryHead *headers,SegyInfo *segyInfo);

private:
    void initDlg();
    void getBinaryHeaderInfos(QStringList &fieldAndDesc);

private:
    QTextBrowser   *m_txtHeaderBrser;
    QTableWidget   *m_binHeaderTab;
};

#endif // DMGRUNIT_H
