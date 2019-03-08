#ifndef DECKUNIT_H
#define DECKUNIT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class DeckUnit;
}


class IpAddrEdit;
class QLineEdit;
class QCheckBox;
//负责与甲板机通信数据
class DeckUnit : public QWidget
{
public:
    DeckUnit(QWidget *parent = Q_NULLPTR);

private:
    void createWg();

private:
    Ui::DeckUnit  *ui;
    QTcpSocket   *m_socket;

    //控件
    IpAddrEdit *m_ipAddrEdit;
    QLineEdit  *m_portEdit;
};

#endif // DECKUNIT_H
