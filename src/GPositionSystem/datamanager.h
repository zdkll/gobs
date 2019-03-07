#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QVector>
#include "gpspublic.h"

//数据管理-单例模式
class DataManager
{
public:
    static DataManager *instance();

    //添加GPS坐标点
    void addGpsCoord(const GpsCoord& cord);

protected:
    DataManager();

private:
    QVector<GpsCoord> m_gpsCord;

};

#endif // DATAMANAGER_H
