#include "publicwidgets.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QColorDialog>
#include <QKeyEvent>
#include <QValidator>
#include <QPainter>


PublicWidgets::PublicWidgets()
{

}




//-----------------------------------------------
ColorPickButton::ColorPickButton(QWidget *parent)
    :QPushButton(parent),m_color(Qt::white),m_id(-1)
{

    initButton();
    setColor(m_color);
}

ColorPickButton::ColorPickButton(const QColor & color,QWidget *parent)
    :QPushButton(parent),m_color(color)
{

    initButton();
    setColor(m_color);
}

void ColorPickButton::setColor(const QColor &color)
{
    m_color = color;
    //    QPalette palette = m_colorLbl->palette();
    //    qDebug()<<"set Color:"<<m_color;
    //    palette.setColor(QPalette::Background,Qt::red);
    //    m_colorLbl->setPalette(palette);

    //    QPalette palette = this->palette();
    //    qDebug()<<"set Color:"<<m_color;
    //    palette.setColor(QPalette::Background,m_color);
    //    this->setPalette(palette);

    QPixmap pix(m_colorLbl->size());
    pix.fill(m_color);
    m_colorLbl->setPixmap(pix);
}

void ColorPickButton::initButton()
{
    QGridLayout *mainLayout = new QGridLayout;
    m_colorLbl = new QLabel(this);
    this->setAutoFillBackground(true);
    m_colorLbl->setAutoFillBackground(true);
    mainLayout->addWidget(m_colorLbl);

    //mainLayout->setContentsMargins();

    this->setLayout(mainLayout);

    this->setMinimumHeight(36);
    this->setMaximumWidth(80);

    connect(this,SIGNAL(clicked(bool)),this,SLOT(slotPickColor()));
}

void ColorPickButton::slotPickColor()
{
    QColor color = QColorDialog::getColor(m_color,this,"Pick a Color");

    if(!color.isValid())
        return;
    if(color == m_color)
        return;

    //设置按钮颜色
    setColor(color);

    //发送颜色改变信号
    emit colorChanged(m_id,color);
}

//IpPartLineEdit-----------------------------------------------------
IpPartLineEdit::IpPartLineEdit(QWidget *parent/* = 0*/)
    : QLineEdit(parent)
{
    next_tab_ = NULL;
    prev_tab_ = NULL;

    this->setMaxLength(3);
    this->setFrame(false);
    this->setAlignment(Qt::AlignCenter);

    QValidator *validator = new QIntValidator(0, 255, this);
    this->setValidator(validator);

    connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(text_edited(const QString&)));
}

IpPartLineEdit::~IpPartLineEdit(void)
{
}

void IpPartLineEdit::focusInEvent(QFocusEvent *e)
{
    this->selectAll();
    QLineEdit::focusInEvent(e);
}

#include <QDebug>
void IpPartLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Period)
    {
        if (next_tab_)
        {
            next_tab_->setFocus();
            next_tab_->selectAll();
        }
    }
    if(event->key() == Qt::Key_Left)
    {
        if(cursorPosition() <= 0)
            if(prev_tab_)
            {
                prev_tab_->setFocus();
                prev_tab_->selectAll();
            }

    }
    if(event->key() == Qt::Key_Right)
    {
        if(cursorPosition() >= this->text().size())
            if (next_tab_)
            {
                next_tab_->setFocus();
                next_tab_->selectAll();
            }
        qDebug()<<"cursor pos:"<<this->cursorPosition();
    }
    QLineEdit::keyPressEvent(event);
}

void IpPartLineEdit::text_edited(const QString& text)
{
    QIntValidator v(0, 255, this);
    QString ipaddr = text;
    int pos = 0;
    QValidator::State state = v.validate(ipaddr, pos);
    if (state == QValidator::Acceptable)
    {
        if (ipaddr.size() > 1)
        {
            if (ipaddr.size() == 2)
            {
                int ipnum = ipaddr.toInt();

                if (ipnum > 25)
                {
                    if (next_tab_)
                    {
                        next_tab_->setFocus();
                        next_tab_->selectAll();
                    }
                }
            }
            else
            {
                if (next_tab_)
                {
                    next_tab_->setFocus();
                    next_tab_->selectAll();
                }
            }
        }
    }
}



//--------------------------------------
IpAddrEdit::IpAddrEdit(QWidget* pParent)
    : QFrame(pParent)
{
    ip_part1_ = new IpPartLineEdit(this);
    ip_part2_ = new IpPartLineEdit(this);
    ip_part3_ = new IpPartLineEdit(this);
    ip_part4_ = new IpPartLineEdit(this);

    labeldot1_ = new QLabel(this);
    labeldot2_ = new QLabel(this);
    labeldot3_ = new QLabel(this);

    labeldot1_->setText(".");
    labeldot2_->setText(".");
    labeldot3_->setText(".");
    labeldot3_->setMaximumWidth(3);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->addWidget(ip_part1_);
    hLayout->addWidget(labeldot1_);
    hLayout->addWidget(ip_part2_);
    hLayout->addWidget(labeldot2_);
    hLayout->addWidget(ip_part3_);
    hLayout->addWidget(labeldot3_);
    hLayout->addWidget(ip_part4_);

    this->setLayout(hLayout);

    QWidget::setTabOrder(ip_part1_, ip_part2_);
    QWidget::setTabOrder(ip_part2_, ip_part3_);
    QWidget::setTabOrder(ip_part3_, ip_part4_);
    ip_part1_->setNextTabEdit(ip_part2_);
    ip_part2_->setNextTabEdit(ip_part3_);
    ip_part3_->setNextTabEdit(ip_part4_);

    ip_part2_->setPrevTabEdit(ip_part1_);
    ip_part3_->setPrevTabEdit(ip_part2_);
    ip_part4_->setPrevTabEdit(ip_part3_);

    this->setFrameShape(QFrame::Box);
    this->setFrameShadow(QFrame::Sunken);
    QPalette palette  =this->palette();
    palette.setColor(QPalette::Window,Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    this->setMaximumHeight(30);

    connect(ip_part1_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
    connect(ip_part2_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
    connect(ip_part3_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
    connect(ip_part4_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));

    connect(ip_part1_, SIGNAL(textEdited (const QString&)), this, SLOT(texteditedslot(const QString&)));
    connect(ip_part2_, SIGNAL(textEdited (const QString&)), this, SLOT(texteditedslot(const QString&)));
    connect(ip_part3_, SIGNAL(textEdited (const QString&)), this, SLOT(texteditedslot(const QString&)));
    connect(ip_part4_, SIGNAL(textEdited (const QString&)), this, SLOT(texteditedslot(const QString&)));
}

IpAddrEdit::~IpAddrEdit()
{

}

void IpAddrEdit::textchangedslot(const QString& /*text*/)
{
    QString ippart1, ippart2, ippart3, ippart4;
    ippart1 = ip_part1_->text();
    ippart2 = ip_part2_->text();
    ippart3 = ip_part3_->text();
    ippart4 = ip_part4_->text();

    QString ipaddr = QString("%1.%2.%3.%4")
            .arg(ippart1)
            .arg(ippart2)
            .arg(ippart3)
            .arg(ippart4);

    emit textchanged(ipaddr);
}

void IpAddrEdit::texteditedslot(const QString &/*text*/)
{
    QString ippart1, ippart2, ippart3, ippart4;
    ippart1 = ip_part1_->text();
    ippart2 = ip_part2_->text();
    ippart3 = ip_part3_->text();
    ippart4 = ip_part4_->text();

    QString ipaddr = QString("%1.%2.%3.%4")
            .arg(ippart1)
            .arg(ippart2)
            .arg(ippart3)
            .arg(ippart4);

    emit textedited(ipaddr);
}

void IpAddrEdit::setText(const QString &text)
{
    QString ippart1, ippart2, ippart3, ippart4;
    QString qstring_validate = text;

    // IP地址验证
    QRegExp regexp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator regexp_validator(regexp, this);
    int nPos = 0;
    QValidator::State state = regexp_validator.validate(qstring_validate, nPos);
    // IP合法
    if (state == QValidator::Acceptable)
    {
        QStringList ippartlist = text.split(".");

        int strcount = ippartlist.size();
        int index = 0;
        if (index < strcount)
        {
            ippart1 = ippartlist.at(index);
        }
        if (++index < strcount)
        {
            ippart2 = ippartlist.at(index);
        }
        if (++index < strcount)
        {
            ippart3 = ippartlist.at(index);
        }
        if (++index < strcount)
        {
            ippart4 = ippartlist.at(index);
        }
    }

    ip_part1_->setText(ippart1);
    ip_part2_->setText(ippart2);
    ip_part3_->setText(ippart3);
    ip_part4_->setText(ippart4);
}

QString IpAddrEdit::text() const
{
    QString ippart1, ippart2, ippart3, ippart4;
    ippart1 = ip_part1_->text();
    ippart2 = ip_part2_->text();
    ippart3 = ip_part3_->text();
    ippart4 = ip_part4_->text();

    if(ippart1.isEmpty() || ippart2.isEmpty()
            ||ippart3.isEmpty() || ippart4.isEmpty())
        return QString();

    return QString("%1.%2.%3.%4")
            .arg(ippart1)
            .arg(ippart2)
            .arg(ippart3)
            .arg(ippart4);
}

void IpAddrEdit::setStyleSheet(const QString &styleSheet)
{
    ip_part1_->setStyleSheet(styleSheet);
    ip_part2_->setStyleSheet(styleSheet);
    ip_part3_->setStyleSheet(styleSheet);
    ip_part4_->setStyleSheet(styleSheet);
}


//-GLine 分割线-----------------------------------------
GLine::GLine(QWidget* parent)
    :QFrame(parent),m_orientation(Qt::Horizontal)
{
    this->setFrameShadow(QFrame::Sunken);
    setOrientation(m_orientation);
}

GLine::GLine(const Qt::Orientation &orientation ,QWidget* parent)
    :QFrame(parent),m_orientation(orientation)
{
    this->setFrameShadow(QFrame::Sunken);
    setOrientation(m_orientation);
}

void GLine::setOrientation(const Qt::Orientation &orientation)
{
    if(orientation == Qt::Horizontal)
        this->setFrameShape(QFrame::HLine);
    else if(orientation == Qt::Vertical)
        this->setFrameShape(QFrame::VLine);
}


//ColorFrame --------------------------------
ColorFrame::ColorFrame(QWidget *parent)
    :QFrame(parent),m_color(Qt::red)
{
    this->setFixedSize(QSize(30,15));
}

QSize ColorFrame::sizeHint() const
{
    return  QSize(30,15);
}

void ColorFrame::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    QPen pen;
    pen.setColor(QColor(20,20,20));
    pen.setWidth(3);
    p.setPen(pen);
    p.fillRect(this->rect(),m_color);

    p.drawLine(0,0,width(),0);
    p.drawLine(0,0,0,height());
    return QFrame::paintEvent(e);
}


DownToolButton::DownToolButton(QWidget * parent)
    :QToolButton(parent),m_down(false)
{

}

void DownToolButton::setBtnDown(const bool &down)
{
    if(m_down != down)
    {
        m_down = down;
        this->setDown(m_down);
    }
    emit btnDowned(m_down);
}

void DownToolButton::mouseReleaseEvent(QMouseEvent *e)
{
    QToolButton::mouseReleaseEvent(e);

    m_down = !m_down;

    this->setDown(m_down);

    emit btnDowned(m_down);
    emit btnTriggerDowned(m_down);
}




///HintLabel---
HintLabel::HintLabel(QLabel *label)
    :m_label(label)
{
    m_palettle = m_label->palette();
    m_label->setAutoFillBackground(true);
}

void HintLabel::showInformation(const QString &hint)
{
    m_label->setPalette(m_palettle);
    m_label->setText(hint);
}

void HintLabel::showWarning(const QString &hint)
{
    QPalette palette = m_palettle;
    palette.setBrush(QPalette::Background,Qt::yellow);
    palette.setColor(QPalette::Text,Qt::yellow);
    m_label->setPalette(palette);

    m_label->setText(hint);
}

void HintLabel::showError(const QString &hint)
{
    QPalette palette = m_palettle;
    palette.setBrush(QPalette::Background,Qt::red);
    m_label->setPalette(palette);
    m_label->setText(hint);
}
