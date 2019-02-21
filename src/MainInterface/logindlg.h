#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>

namespace Ui {
class LoginDlg;
}

struct User
{
    QString usrName;
    QString password;
    bool       isRmbPwd = false;
};

class ShowErWeimDlg;
class ChangePasswordDlg;

class QPushButton;
class QLabel;
class QLineEdit;
class HintLabel;
class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

protected slots:
    void slotInformation();
    void slotTechAdv();
    void slotLogIn();
    void slotIsRmbPwd(const bool &rmbPWd);
    void slotChangePwd();

    //
    void slotPwdHintNormal();

private:
    void initDlg();
    bool initUserInfo(User &user);
    bool saveUserInfo(const User &user,const QString &fileName);

    Ui::LoginDlg *ui;
    ShowErWeimDlg        *m_showErweimDlg;
    ChangePasswordDlg  *m_changePwdDlg;
    QString m_Path;
    User      m_user;

    HintLabel *m_pwdHint;
};

//显示二维码
class ShowErWeimDlg: public QDialog
{
public:
    ShowErWeimDlg(QWidget *parent = 0);

};

//修改用户密码
class ChangePasswordDlg: public QDialog
{
    Q_OBJECT
public:
    ChangePasswordDlg(QWidget *parent = 0);

    void setPassword(const QString &pwd);

    QString passWord(){return m_password;}

protected slots:
    void slotChangePwd();
    void clearInput();


    void slotOldPwdChanged(const QString &text);
    void slotNewPwd1Changed(const QString &text);
    void slotNewPwd2Changed(const QString &text);

    bool checkPWdStr(QString &pwdStr);

private:
    void createDialog();

    QLineEdit  *m_oldPwdEdit,*m_newPwdEdit1,*m_newPwdEdit2;
    QLabel       *m_oldPwdHint,*m_newPwdHint1,*m_newPwdHint2;
    QString      m_password;
};

#endif // LOGINDLG_H
