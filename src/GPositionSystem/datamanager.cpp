#include "datamanager.h"

DataManager::DataManager()
{

}

DataManager *DataManager::instance()
{
    static DataManager dmgr;
    return &dmgr;
}

void DataManager::addGpsCoord(const GpsCoord &cord)
{
    m_gpsCord.append(cord);

}


