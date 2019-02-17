#ifndef NODEQUERY_H
#define NODEQUERY_H

#include <QTcpSocket>
#include "gpublic.h"

#include "rnmpublic.h"

class NodeItem;
class NodeQuery : public QTcpSocket
{
    Q_OBJECT
public:
    enum QueryStep
    {
        NoQuery        = 0,
        QueryVoltage   = 1,
        QueryTemp      = 2,
        QueryPressure  = 3
    };
    NodeQuery(Node *owner);
    void start(const int &interval);
    void stop();

    void refresh();

    //重连/断开-------
    void reConnect();
    void disConnect();

    const bool &isRunning(){return m_running;}

private slots:
    void slotReadData();
    void slotStateChanged(QAbstractSocket::SocketState);

private:
    void       startQuery();

    QueryStep  m_queryStep;
    bool       m_running;
    Node       *m_node;
    int        m_count;

    int        m_interval; //刷新间隔
    QTime      t;
    bool       m_stoped;
};

#endif // NODEQUERY_H
