#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>
#include <QtCore>

#include "gpublic.h"
#include "rnmbaselib_global.h"

class RNMBASELIBSHARED_EXPORT SearchThread : public QThread
{
    Q_OBJECT
public:
    explicit SearchThread(QObject *parent = 0);
    ~SearchThread();

    void setId(const int &id){m_id = id;}

    void stop();

    void setTotalHosts(QVector<uint> hosts);

    void setIPScope(const int &start, const int &last);
    const IPScope &ipScope(){return m_IPScope;}

    void setTaskCount(const TaskCount &taskCount);

    void setWaitTime(const int &wtime){m_wTime = wtime;}

protected:
    void run();

signals:
    //搜索到IP地址则返回地址，没有返回空
    void searchedHost(quint32 ip,const bool &available = true);
    //搜索结束，返回自己的Id
    void searchFinished(const int &id);

private:
    QMutex      m_mutex;
    bool        m_stoped;
    IPScope     m_IPScope;
    TaskCount   m_taskCount;
    int         m_id;
    int         m_wTime;

    QVector<uint> m_totalHosts;
};



#endif // SEARCHTHREAD_H
