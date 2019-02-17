#ifndef MAINWINUNIT_H
#define MAINWINUNIT_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QtCore>
#include <QAbstractItemModel>

#include <QListWidget>
#include <QToolButton>
#include <QRadioButton>

#include "mainwinpublic.h"
#include "gcore.h"
#include "gpublic.h"

class MainWinUnit
{

public:
    MainWinUnit();
};

class LabelLink : public QLabel
{
public:
    LabelLink(const QString &text,QWidget *parent = 0);

private:
};

class ModToolButton : public QToolButton
{
    Q_OBJECT
public:
    ModToolButton(const QIcon &icon,const QString &text,QWidget *parent = 0);
    ~ModToolButton();

protected:
    void mouseDoubleClickEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);

signals:
    void signalRun(const QString& ModuleName);
    void signalAbout(const QString& ModuleName);

private slots:
    void slotCustomContextMenuRequested(QPoint);

    void slotAbout();
    void slotRun();

private:
    QMenu *menu;
};

class CreateProjDialog;
//工区管理,显示和更新/config/projects 文件
class ProjectManager : public QObject
{
    Q_OBJECT
public:
    ProjectManager(QObject *parent = 0);
    ~ProjectManager();

    //创建最近项目菜单列表
    void createRecentProjMenu(QMenu *menu);

    //工区列表
    void setProjectListWidget(QListWidget *listWidget);

    //打开某个工区
    static const QString getOpenProject();

    //创建工区
    static const ProjectInfo createProject();

    //保存或者更新最近工区
    void saveCurrentProject(const ProjectInfo &);

    void clearRecents();

signals:
    void signalOpenProject(const QString);

private slots:
    void slotRecentFiles(QAction*);

    //listWidget
    void slotDoubleClicked(QListWidgetItem *);
    void customContextMenuRequested(QPoint);
    void listWgOpenProject();
    void listWgClearRencentProject();

private:
    void updateRecentProjects();
    //历史工区列表------
    //工区名-路径 Map
    QStringList  m_projList;
    QListWidget *m_projListWg;
    QMenu       *m_menu;

    QMenu          *m_listWgMenu;
};


class CreateProjDialog: public QDialog
{
    Q_OBJECT
public:
    CreateProjDialog();
    void initDlg();
    const ProjectInfo &getProjInfo(){return m_projInfo;}

protected:


private slots:
    void slotOk();
    void slotCancel();
    void slotBrowser();

    void slotProNameChanged(const QString &proName);

private:

    //----------
    QLineEdit     *proNameEdit,*proPathEdit;
    QLabel        *proNameHint;
    QPushButton   *proPathBrseBtn;
    QRadioButton  *defultWorkPathBtn;
    int            fileIsok;       //文件名标识，1 为非法字符；2为文件名存在
    ProjectInfo    m_projInfo;
};


#endif // MAINWINUNIT_H
