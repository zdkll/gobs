#ifndef BOARDCONN_H
#define BOARDCONN_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class BoardConn;
}


//负责与甲板机通信数据
class BoardConn : public QWidget
{
public:
    BoardConn(QWidget *parent = Q_NULLPTR);

private:
     void createWg();

private:
    Ui::BoardConn  *ui;
    QTcpSocket   *m_socket;
};

#endif // BOARDCONN_H
