#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "gcore.h"
#include "mainwinpublic.h"
#include "mainwinunit.h"
#include "exportspsfilesdlg.h"

using namespace Core;

namespace Ui {
class MainWindow;
}

class QToolButton;
class GPositionSystem;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //Project
    int  slotOpenProject();
    int  slotOpenProject(const QString &proName);
    void slotNewProject();

    //Module
    void slotAboutModule(const QString &modName);
    void slotRunModule(const QString &modName = QString());

    //启动生成sps文件
    void slotExportSPSFiles();
    void slotGpsPosition();

    //Help
    void slotHelp();

private:
    void initWindow();
    void initMenu();

    //设置当前工区
    void setCurrentProject(ProjectInfo projInfo);
    void setCurrentProject(const QString &projPath,const QString &projName);

private:
    Ui::MainWindow *ui;
    QString m_Path;

    //项目路径和名称
    ProjectInfo     m_projInfo;
    QLabel         *m_currentProjLbl;

    //历史项目文件记录管理
    ProjectManager *m_projManager;

    //所有的进程，程序标识+进程
    QMap<QString,QProcess*>  m_processes;
    ExportSPSFilesDlg       *m_exportSpsFileDlg;

    GPositionSystem  *m_gpsPositionSystem;
};

#endif // MAINWINDOW_H
