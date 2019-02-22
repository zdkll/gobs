#include "logindlg.h"
#include "ui_logindlg.h"


#define JKey_UserName "user"
#define JKey_PassWord "password"
#define JKey_IsRmdPwd "IsRmdPwd"

#define Default_UserName   "admin"
#define Default_PassWord   "123456"

#include "gcore.h"
#include "gpublicwidgets.h"

#include <QFile>
#include <QJsonObject>
#include <QDesktopServices>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QToolTip>
#include <QGridLayout>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
  ,m_showErweimDlg(0)
  ,m_changePwdDlg(0)
{
    ui->setupUi(this);

    m_Path = Core::rootPath();
    initUserInfo(m_user);

    initDlg();

    this->setWindowTitle(QStringLiteral("欢迎使用GOBS系统!"));
    this->resize(890,545);

    QString styleSheet = "QWidget{background:white}";

    this->setStyleSheet(styleSheet);

    //资讯
    connect(ui->infoBtn,&QPushButton::clicked,this,&LoginDlg::slotInformation);
    //技术咨询
    connect(ui->techAdvBtn,&QPushButton::clicked,this,&LoginDlg::slotTechAdv);

    connect(ui->logInBtn,&QPushButton::clicked,this,&LoginDlg::slotLogIn);
    connect(ui->rmbPwdBtn,&QCheckBox::clicked,this,&LoginDlg::slotIsRmbPwd);

    connect(ui->changePwdBtn,&QPushButton::clicked,this,&LoginDlg::slotChangePwd);
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::initDlg()
{
    this->setAutoFillBackground(true);
    ui->logoLbl->setPixmap(QPixmap(m_Path+"/icon/ZDZ_logo.png"));

    //二维码
    QVBoxLayout *erweimLayout = new QVBoxLayout;
    erweimLayout->setMargin(0);
    QLabel *erweimLbl = new QLabel;
    erweimLayout->addWidget(erweimLbl);
    erweimLbl->setPixmap(QPixmap(m_Path+"/icon/ZDZ_erweim.png"));
    ui->erweimBox->setLayout(erweimLayout);

    ui->userNameEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z]+[a-z0-9A-Z_]*")));
    ui->userNameEdit->setText(Default_UserName);
    ui->userNameEdit->selectAll();
    ui->userNameEdit->setReadOnly(true);
    ui->pwdEdit->setEchoMode(QLineEdit::Password);
    ui->pwdEdit->setValidator(new QRegExpValidator(QRegExp("[\\x00-\\xfe]{6,15}")));

    if(m_user.isRmbPwd)
        ui->pwdEdit->setText(m_user.password);

    ui->rmbPwdBtn->setChecked(m_user.isRmbPwd);

    m_pwdHint = new HintLabel(ui->pwdHintLbl);
    connect(ui->pwdEdit,&QLineEdit::textChanged,this,&LoginDlg::slotPwdHintNormal);
}

bool LoginDlg::initUserInfo(User &user)
{
    QString fileName = m_Path+"./config/users.json";
    QFile usrFile(fileName);
    if(!usrFile.exists()){
        user.usrName = Default_UserName;
        user.password = Default_PassWord;
        user.isRmbPwd = false;
        return saveUserInfo(user,fileName);
    }else{
        QJsonObject jObj = Core::readJsonDoc(fileName);
        if(jObj.isEmpty())
            return false;
        user.usrName = jObj.value(JKey_UserName).toString();
        user.password = jObj.value(JKey_PassWord).toString();
        user.isRmbPwd = jObj.value(JKey_IsRmdPwd).toBool();
    }
    return true;
}

bool LoginDlg::saveUserInfo(const User &user,const QString &fileName)
{
    QJsonObject obj;
    obj.insert(JKey_UserName,user.usrName);
    obj.insert(JKey_PassWord,user.password);
    obj.insert(JKey_IsRmdPwd,user.isRmbPwd);
    return Core::saveJsonObject(obj,fileName);
}

void LoginDlg::slotInformation()
{
    QDesktopServices::openUrl(QUrl("www.cgif.com.cn"));
}

void LoginDlg::slotTechAdv()
{
    if(!m_showErweimDlg)
        m_showErweimDlg = new ShowErWeimDlg(this);
    m_showErweimDlg->exec();
}

void LoginDlg::slotLogIn()
{
    //判断用户名密码
    int pos = 0;
    QString pwd_txt = ui->pwdEdit->text();
    QValidator::State state =  ui->pwdEdit->validator()->validate(pwd_txt,pos);

    if(state == QValidator::Acceptable)
    {
        //登陆成功，保存登陆配置文件
        if(pwd_txt == m_user.password)
        {
            QString fileName = m_Path+"./config/users.json";
            saveUserInfo(m_user,fileName);
            this->accept();
        }
        else
            m_pwdHint->showError(QStringLiteral("密码错误"));
    }else
        m_pwdHint->showWarning(QStringLiteral("(请输入6-15位密码)"));
}

void LoginDlg::slotIsRmbPwd(const bool &rmbPWd)
{
    m_user.isRmbPwd = rmbPWd;
}

void LoginDlg::slotChangePwd()
{
    if(!m_changePwdDlg)
        m_changePwdDlg = new ChangePasswordDlg(this);
    m_changePwdDlg->setPassword(m_user.password);
    if(m_changePwdDlg->exec()){
        m_user.password = m_changePwdDlg->passWord();
        QString fileName = m_Path+"./config/users.json";
        saveUserInfo(m_user,fileName);
        ui->pwdEdit->clear();
    }
}

void LoginDlg::slotPwdHintNormal()
{
    m_pwdHint->showInformation("");
}


///ShowErWeimDlg----------------
ShowErWeimDlg::ShowErWeimDlg(QWidget *parent)
    :QDialog(parent)
{
    QString m_Path = Core::rootPath();

    this->setWindowTitle(QString("中地装仪器有限公司"));
    QVBoxLayout *erweimLayout = new QVBoxLayout;
    erweimLayout->setMargin(0);
    QLabel *erweimLbl = new QLabel;
    erweimLayout->addWidget(erweimLbl,0,Qt::AlignCenter);
    erweimLbl->setPixmap(QPixmap(m_Path+"/icon/ZDZ_erweim.png"));

    this->setLayout(erweimLayout);
    this->resize(422,348);
}



///ChangePasswordDlg----
ChangePasswordDlg::ChangePasswordDlg(QWidget *parent)
    :QDialog(parent)
{
    this->setWindowTitle(QStringLiteral("修改密码"));

    createDialog();
}


void ChangePasswordDlg::setPassword(const QString &pwd)
{
    m_password = pwd;

}

void ChangePasswordDlg::createDialog()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGridLayout *pwdGridLayout = new QGridLayout;

    //
    QLabel *oldLbl = new QLabel(QStringLiteral("请输入旧密码:"));
    m_oldPwdEdit = new QLineEdit();
    pwdGridLayout->addWidget(oldLbl,0,0);
    pwdGridLayout->addWidget(m_oldPwdEdit,0,1);
    m_oldPwdHint = new QLabel(this);
    pwdGridLayout->addWidget(m_oldPwdHint,0,2);

    QLabel *newLbl1 = new QLabel(QStringLiteral("请输入新密码:"));
    m_newPwdEdit1 = new QLineEdit();
    m_newPwdHint1 = new QLabel(this);
    pwdGridLayout->addWidget(newLbl1,1,0);
    pwdGridLayout->addWidget(m_newPwdEdit1,1,1);
    pwdGridLayout->addWidget(m_newPwdHint1,1,2);

    QLabel *newLbl2 = new QLabel(QStringLiteral("再次确认新密码:"));
    m_newPwdEdit2 = new QLineEdit();
    m_newPwdHint2 = new QLabel(this);
    pwdGridLayout->addWidget(newLbl2,2,0);
    pwdGridLayout->addWidget(m_newPwdEdit2,2,1);
    pwdGridLayout->addWidget(m_newPwdHint2,2,2);

    mainLayout->addLayout(pwdGridLayout);

    m_oldPwdEdit->setEchoMode(QLineEdit::Password);
    m_oldPwdEdit->setValidator(new QRegExpValidator(QRegExp("[\\x00-\\xfe]{6,15}")));
    m_newPwdEdit1->setEchoMode(QLineEdit::Password);
    m_newPwdEdit1->setValidator(new QRegExpValidator(QRegExp("[\\x00-\\xfe]{6,15}")));
    m_newPwdEdit2->setEchoMode(QLineEdit::Password);
    m_newPwdEdit2->setValidator(new QRegExpValidator(QRegExp("[\\x00-\\xfe]{6,15}")));

    m_oldPwdEdit->setMinimumWidth(160);
    m_oldPwdHint->setMinimumWidth(160);
    //button
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch(1);
    QPushButton *okBtn = new QPushButton(QStringLiteral("确定"));
    QPushButton *cancelBtn = new QPushButton(QStringLiteral("取消"));
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(btnLayout);

    connect(okBtn,&QPushButton::clicked,this,&ChangePasswordDlg::slotChangePwd);
    connect(cancelBtn,&QPushButton::clicked,[this]{clearInput();this->reject();});

    connect(m_oldPwdEdit,&QLineEdit::textChanged,this,&ChangePasswordDlg::slotOldPwdChanged);
    connect(m_newPwdEdit1,&QLineEdit::textChanged,this,&ChangePasswordDlg::slotNewPwd1Changed);
    connect(m_newPwdEdit2,&QLineEdit::textChanged,this,&ChangePasswordDlg::slotNewPwd2Changed);

    pwdGridLayout->setColumnStretch(0,1);
    pwdGridLayout->setColumnStretch(1,1);
    pwdGridLayout->setColumnStretch(2,1);


    this->setLayout(mainLayout);
}

void ChangePasswordDlg::slotChangePwd()
{
    //
    QString newPwd;
    bool ret = checkPWdStr(newPwd);
    if(ret){
        m_password = newPwd;
        //qDebug()<<"change pwd to:"<<m_password;
        this->accept();
        clearInput();
    }
}

bool ChangePasswordDlg::checkPWdStr(QString &pwdStr)
{
    //检查旧密码
    QString oldPwd = m_oldPwdEdit->text();
    if(oldPwd.isEmpty()){
        m_oldPwdHint->setText(QStringLiteral(" 旧密码为空！"));
        return false;
    }
    else if(oldPwd != m_password){
        m_oldPwdHint->setText(QStringLiteral(" 密码错误！"));
        return false;
    }

    //检查新密码
    QString newPwd1 = m_newPwdEdit1->text();
    if(newPwd1.isEmpty()){
        m_newPwdHint1->setText(QStringLiteral("请输入新密码"));
        return false;
    }else if(newPwd1.size()<6 || newPwd1.size()>15){
        m_newPwdHint1->setText(QStringLiteral("请输入6-15位密码"));
        return false;
    }

    QString newPwd2 = m_newPwdEdit2->text();
    if(newPwd2.isEmpty()){
        m_newPwdHint2->setText(QStringLiteral("请再次输入密码"));
    }else if(newPwd2.size()<6 || newPwd2.size()>15){
        m_newPwdHint2->setText(QStringLiteral("请输入6-15位密码"));
        return false;
    }

    //qDebug()<<"new pwd1:"<<newPwd1<<newPwd2;
    if(newPwd1 != newPwd2)
    {
        m_newPwdHint2->setText(QStringLiteral("两次输入密码不一致"));
        return false;
    }
    pwdStr = newPwd2;
    return true;
}

void ChangePasswordDlg::slotOldPwdChanged(const QString &)
{
    m_oldPwdHint->clear();
}

void ChangePasswordDlg::slotNewPwd1Changed(const QString &)
{
    m_newPwdHint1->clear();
}

void ChangePasswordDlg::slotNewPwd2Changed(const QString &)
{
    m_newPwdHint2->clear();
}

void ChangePasswordDlg::clearInput()
{
    m_oldPwdEdit->clear();
    m_newPwdEdit1->clear();
    m_newPwdEdit2->clear();
}




