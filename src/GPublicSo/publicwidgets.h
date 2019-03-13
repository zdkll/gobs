#ifndef PUBLICWIDGETS_H
#define PUBLICWIDGETS_H

#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>

#include "gpublicso_global.h"


class  QFocusEvent;
class  QKeyEvent;

//公共的一些控件------
class  PublicWidgets
{

public:
    PublicWidgets();
};

/*颜色选择按钮,可设置按钮初始颜色和ID
 *点击后弹出颜色选择对话框
 *选择颜色后发送信号
 */
class GPUBLICSOSHARED_EXPORT   ColorPickButton: public QPushButton
{
    Q_OBJECT
public:
    ColorPickButton(QWidget *parent = 0);
    ColorPickButton(const QColor & color,QWidget *parent = 0);

    void setColor(const QColor &color);
    QColor color() const{return m_color;}

    void setId(const int &id){m_id = id;}
    int id() const       {return m_id;}

signals:
    void colorChanged(const int &id,const QColor &color);

private slots:
    void slotPickColor();

private:
    void initButton();

private:
    QColor   m_color;
    QLabel  *m_colorLbl;
    int      m_id;
};

/* IPV4 输入框，可以按照格式输入并且以特定格式返回
 *
 */
class GPUBLICSOSHARED_EXPORT  IpPartLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    IpPartLineEdit(QWidget *parent = 0);
    ~IpPartLineEdit(void);

    void setNextTabEdit(QLineEdit *nexttab) { next_tab_ = nexttab; }
    void setPrevTabEdit(QLineEdit *prevtab) { prev_tab_ = prevtab; }

protected:
    void focusInEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void  text_edited(const QString& text);

private:
    QLineEdit  *next_tab_;
    QLineEdit  *prev_tab_;
};

class GPUBLICSOSHARED_EXPORT IpAddrEdit : public QFrame
{
    Q_OBJECT
public:
    IpAddrEdit(QWidget* parent = 0);
    ~IpAddrEdit();

    void setText(const QString &text);
    //IP地址不合法则返回空
    QString text() const;
    void setStyleSheet(const QString &styleSheet);

signals:
    void textchanged(const QString& text);
    void textedited(const QString &text);

private slots:
    void textchangedslot(const QString& text);
    void texteditedslot(const QString &text);

private:
    IpPartLineEdit *ip_part1_;
    IpPartLineEdit *ip_part2_;
    IpPartLineEdit *ip_part3_;
    IpPartLineEdit *ip_part4_;

    QLabel *labeldot1_;
    QLabel *labeldot2_;
    QLabel *labeldot3_;
};

//--------------------------------------------

//分割线
class GPUBLICSOSHARED_EXPORT  GLine : public QFrame
{
public:
    GLine(QWidget* parent = 0);
    GLine(const Qt::Orientation &orientation,QWidget* parent = 0);

    void setOrientation(const Qt::Orientation &orientation);

private:
    Qt::Orientation m_orientation;
};


//颜色窗口 QFrame
class GPUBLICSOSHARED_EXPORT ColorFrame : public QFrame
{
public:
    ColorFrame(QWidget *parent = 0);
    void  setColor(QColor color){m_color = color;}
    QColor color() const {return m_color;}
    QSize sizeHint() const;

protected:
    void   paintEvent(QPaintEvent *e);
    QColor m_color;
};

class GPUBLICSOSHARED_EXPORT DownToolButton : public QToolButton
{
    Q_OBJECT
public:
    DownToolButton(QWidget * parent = 0);
    void setBtnDown(const bool &down);

signals:
    void btnDowned(const bool &down);
    void btnTriggerDowned(const bool &down);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool m_down;
};

//提示标签
class GPUBLICSOSHARED_EXPORT HintLabel
{
public:
    HintLabel(QLabel *label);

    void showInformation(const QString &hint);

    void showWarning(const QString &hint);

    void showError(const QString &hint);
private:
    QPalette m_palettle;
    QLabel  *m_label;
};


#endif // PUBLICWIDGETS_H
