#include "postioncontrols.h"

#include <QPushButton>
#include <QToolButton>
#include <QBoxLayout>

PostionControls::PostionControls()
{

}


///TitleBar
TitleBar::TitleBar(QWidget *parent)
    :QWidget(parent)
{
    createTitlebar();
}

void TitleBar::slotSidebarBtnClicked(bool checked)
{
    if(checked)
        m_sideBarBtn->setText(QStringLiteral(">展开"));
    else
        m_sideBarBtn->setText(QStringLiteral("<隐藏"));
    emit sideBarHide(checked);
}

void TitleBar::createTitlebar()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    m_sideBarBtn = new QToolButton(this);
    m_sideBarBtn->setText(QStringLiteral("<隐藏"));
    m_sideBarBtn->setCheckable(true);
    m_sideBarBtn->setChecked(false);

    mainLayout->addWidget(m_sideBarBtn);
    mainLayout->addStretch(1);

    connect(m_sideBarBtn,&QToolButton::clicked,this,&TitleBar::slotSidebarBtnClicked);
    this->setLayout(mainLayout);
}
