#ifndef DECKUNIT_H
#define DECKUNIT_H

#include <QWidget>
#include <QTcpSocket>

#include "gpspublic.h"

namespace Ui {
class DeckUnit;
}


class IpAddrEdit;
class QLineEdit;
class QCheckBox;
class QComboBox;
//负责与甲板机通信数据
class DeckUnit : public QWidget
{
    Q_OBJECT
public:
    DeckUnit(QWidget *parent = Q_NULLPTR);
//    void setShowTxtFunc(ShowTextFunc func);

protected slots:
    void slotConnect(bool connect);
    void startTx();
    void stopTx();

    void socketStateChanged(QAbstractSocket::SocketState state);
    void slotReadyRead();

private:
    void createWg();

private:
    Ui::DeckUnit   *ui;
    QTcpSocket    *m_socket;
//    ShowTextFunc m_showTxtFunc;

    //控件
    IpAddrEdit *m_ipAddrEdit;
    QLineEdit  *m_portEdit;
    QComboBox *m_epcCbx;

    QWidget    *m_optWidget;
};

#endif // DECKUNIT_H
