#include "searchthread.h"

#include <QHostAddress>
#include <QTcpSocket>
#include <QDebug>


SearchThread::SearchThread(QObject *parent)
    : QThread(parent),m_stoped(false)
    ,m_IPScope(0,0),m_wTime(1000)
{

}

SearchThread::~SearchThread()
{
//    qDebug()<<"destory thread";
}

void SearchThread::stop()
{
    m_mutex.lock();
    m_stoped = true;
    m_mutex.unlock();
}

void SearchThread::setTotalHosts(QVector<uint> hosts)
{
    m_totalHosts = hosts;
}

void SearchThread::setIPScope(const int &start, const int &last)
{
    m_IPScope  = IPScope(start,last);
}

void SearchThread::setTaskCount(const TaskCount &taskCount)
{
    m_taskCount = taskCount;
}



void SearchThread::run()
{
    m_stoped = false;

    QHostAddress addr;

#if 0
    QString cmdStr;
    int exitCode = 0;

    QString args;
#ifdef Q_OS_WIN

    args = " -n 1 -i 1 -q";//Windows 格式,NUL 屏幕输出
#else
    args = " -c 1 -t 1 -q";//Linux 下格式
#endif

    for(uint ip = m_IPScope.first;ip<=m_IPScope.second;ip++)
    {
        m_mutex.lock();
        if(m_stoped)
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
        addr.setAddress(ip);
        //        cmdStr = "ping "+addr.toString() +args;
        //        exitCode = QProcess::execute(cmdStr);
        if (0 == exitCode)
        {
            // it's alive
            emit searchedHost(ip,true);

            //ping 通后，还要判断端口号是否能连上
            //QTcpSocket  socket;
            //socket.connectToHost(addr,5000);
            //socket.waitForConnected(1000);
            // qDebug()<<"socket connected state:"<<socket.state();
        }
        else
        {
            emit searchedHost(ip,false);
        }
    }
#endif

    for(uint ip = m_taskCount.start;ip <= m_taskCount.end;ip += m_taskCount.step)
    {
        m_mutex.lock();
        if(m_stoped)
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
        addr.setAddress(m_totalHosts[ip]);
        //cmdStr = "ping "+addr.toString() +args;
        //exitCode = QProcess::execute(cmdStr);

        QTcpSocket  socket;
        socket.connectToHost(addr,5000);
        socket.waitForConnected(m_wTime);
        //qDebug()<<"socket connected state:"<<socket.state();
        if(socket.state() == QAbstractSocket::ConnectedState) //(0 == exitCode)
        {
            socket.disconnectFromHost();
            // it's alive
            emit searchedHost(ip,true);
        }
        else
        {
            emit searchedHost(ip,false);
        }
    }

    emit searchFinished(m_id);
}


