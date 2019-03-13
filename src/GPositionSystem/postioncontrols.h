#ifndef POSTIONCONTROLS_H
#define POSTIONCONTROLS_H

#include <QWidget>

class PostionControls
{
public:
    PostionControls();
};

class QPushButton;
class QToolButton;
class TitleBar : public QWidget
{
    Q_OBJECT
public:
    TitleBar(QWidget *parent = 0);

signals:
    void sideBarHide(bool hide);

protected slots:
    void slotSidebarBtnClicked(bool checked);

private:
    void createTitlebar();

    QToolButton  *m_sideBarBtn;
};

#endif // POSTIONCONTROLS_H
