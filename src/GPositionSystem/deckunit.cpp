#include "deckunit.h"
#include  "ui_deckunit.h"

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QIntValidator>

#include "publicwidgets.h"

DeckUnit::DeckUnit(QWidget *parent)
    :QWidget(parent),m_socket(new QTcpSocket(this))
    ,ui(new Ui::DeckUnit)
{
    ui->setupUi(this);

    createWg();
}

void DeckUnit::createWg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("甲板机通信"));
    QGridLayout *gridLayout = new QGridLayout();

    QLabel *ipLabel = new QLabel(QStringLiteral("IP:"));
    m_ipAddrEdit = new IpAddrEdit(this);
    m_ipAddrEdit->setMaximumWidth(160);
    gridLayout->addWidget(ipLabel,0,0);
    gridLayout->addWidget(m_ipAddrEdit,0,1);

    QLabel *portLabel = new QLabel(QStringLiteral("Port:"));
    m_portEdit = new QLineEdit(this);
    m_portEdit->setValidator(new QIntValidator(0,10000));
    gridLayout->addWidget(portLabel,1,0);
    gridLayout->addWidget(m_portEdit,1,1);

//    QVBoxLayout

    groupBox->setLayout(gridLayout);
    mainLayout->addWidget(groupBox);

    this->setLayout(mainLayout);
}
