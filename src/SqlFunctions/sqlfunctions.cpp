#include "sqlfunctions.h"

#define SQLite_Driver_Name  "QSQLITE"


SqlFunctions *SqlFunctions::m_instance(NULL);

SqlFunctions::~SqlFunctions()
{
    this->closeDataBase();
}

SqlFunctions *SqlFunctions::instance()
{
    if(m_instance == NULL) {
        m_instance = new SqlFunctions();
    }
    return m_instance;
}

//创建项目数据库，根据数据
bool SqlFunctions::openDataBase(const QString &projPath,const QString projName)
{
    if(m_db.isOpen())
        m_db.close();
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(projPath+"/"+projName+".db");
    m_db.setUserName("root");
    m_db.setPassword("root");
    if(!m_db.open())
    {
        m_errString = m_db.lastError().text();
        return false;
    }
    m_query = QSqlQuery(m_db);
    return true;
}

//创建设备信息表
bool SqlFunctions::createTable(const QString &tablename,const TableType &type )
{
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        return false;
    }

    //创建设备表
    if(NodeDeviceTable == type){
        bool ok = m_query.exec("CREATE TABLE "+tablename +
                               "(Device CHAR(5) PRIMARY KEY NOT NULL,"
                               "IP CHAR(16) NOT NULL,"
                               "Area CHAR(36),"
                               "Line FLOAT,"
                               "Station INT,"
                               "DepolyTime TEXT,"
                               "PickupTime TEXT,"
                               "SPSX FLOAT,"
                               "SPSY FLOAT,"
                               "SPSZ FLOAT,"
                               "ActualX FLOAT,"
                               "ActualY FLOAT,"
                               "ActualZ FLOAT,"
                               "Depolyed TINYINT,"
                               "PickedUp TINYINT,"
                               "Downloaded TINYINT)");
        if(!ok)
        {
            m_errString = m_query.lastError().text();
            return false;
        }
    }
    //其他表，创建......

    return true;
}

bool SqlFunctions::dropTable(const QString &tableName)
{
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        return false;
    }
    bool ok = m_query.exec("DROP TABLE "+tableName);
    if(!ok)
    {
        m_errString = m_query.lastError().text();
    }
    return ok;
}

bool SqlFunctions::clearTable(const QString &tableName)
{
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        return false;
    }
    bool ok = m_query.exec("DELETE FROM "+tableName);
    if(!ok)
    {
        m_errString = m_query.lastError().text();
    }
    return ok;
}

//查询表-query，返回返回对应的query结果对象
QSqlQuery SqlFunctions::query(QString qtablename,QList<Condition> conds,bool *ok)
{
    if(ok)
        *ok = true;
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        if(ok)
            *ok = false;
        return QSqlQuery();
    }
    QString sqlCmd = QString("SELECT * FROM %1").arg(qtablename);

    Condition cond ;
    if(conds.size()>0){
        cond = conds.takeFirst();
        sqlCmd += QString(" where %1='%2'")
                .arg(cond.fieldName)
                .arg(cond.fieldValue.toString());
        for(int i = 0 ;i<conds.size();i++)
        {
            cond = conds[i];
            sqlCmd += QString(" AND %1='%2'")
                    .arg(cond.fieldName)
                    .arg(cond.fieldValue.toString());
        }
    }

    bool ret = m_query.exec(sqlCmd);
    if(!ret)
    {
        if(ok)
            *ok = false;
        m_errString = m_query.lastError().text();
        return m_query;
    }

    return m_query;
}

QSqlQuery SqlFunctions::query(QString qtablename,QStringList fieldList,bool *ok)
{
    if(ok)
        *ok = true;
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        if(ok)
            *ok = false;
        return QSqlQuery();
    }
    QString sqlCmd;
    //选择字段
    if(fieldList.size()<1){
        sqlCmd = QString("SELECT * FROM %1").arg(qtablename);
    }else
    {
        sqlCmd = QString("SELECT %1").arg(fieldList.takeFirst());

        while(!fieldList.isEmpty())
        {
            sqlCmd += ","+fieldList.takeFirst();
        }
        sqlCmd += QString(" FROM %1").arg(qtablename);
    }
    bool ret = m_query.exec(sqlCmd);
    if(!ret)
    {
        if(ok)
            *ok = false;
        m_errString = m_query.lastError().text();
        return QSqlQuery();
    }
    return m_query;
}

QSqlQuery SqlFunctions::query(QString qtablename,QStringList fieldList
                              ,QList<Condition> conds,bool *ok)
{
    if(ok)
        *ok = true;
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        if(ok)
            *ok = false;
        return QSqlQuery();
    }
    QString sqlCmd;
    //选择字段
    if(fieldList.size()<1){
        sqlCmd = QString("SELECT * FROM %1").arg(qtablename);
    }else
    {
        sqlCmd = QString("SELECT %1").arg(fieldList.takeFirst());

        while(!fieldList.isEmpty())
        {
            sqlCmd += ","+fieldList.takeFirst();
        }
        sqlCmd += QString(" FROM %1").arg(qtablename);
    }

    //条件
    Condition cond ;
    if(conds.size()>0){
        cond = conds.takeFirst();
        sqlCmd += QString(" where %1='%2'")
                .arg(cond.fieldName)
                .arg(cond.fieldValue.toString());
    }
    for(int i = 0 ;i<conds.size();i++)
    {
        cond = conds[i];
        sqlCmd += QString(" AND %1='%2'")
                .arg(cond.fieldName)
                .arg(cond.fieldValue.toString());
    }
//    qDebug()<<"query:"<<sqlCmd;
    bool ret = m_query.exec(sqlCmd);
    if(!ret)
    {
        if(ok)
            *ok = false;
        m_errString = m_query.lastError().text();
        return QSqlQuery();
    }

    return m_query;
}

//修改表-update
bool SqlFunctions::update(QString qtablename,QList<Condition> conds,QList<ResultInfo> ResultsList)
{
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        return false;
    }
    Q_ASSERT(ResultsList.size()>0);

    QString sqlCmd = QString("UPDATE %1 SET ").arg(qtablename);
    //Set 字段和值
    ResultInfo retInfo = ResultsList.takeFirst();
    sqlCmd += retInfo.fieldName +"='"+retInfo.fieldValue.toString()+"' ";

    for(int i = 0;i<ResultsList.size();i++)
        sqlCmd += ","+ResultsList[i].fieldName +"='"
                +ResultsList[i].fieldValue.toString()+"' ";

    //条件
    if(conds.size()>0)
    {
        Condition firstCond = conds.takeFirst();
        sqlCmd += "WHERE "+firstCond.fieldName+"='"
                +firstCond.fieldValue.toString()+"'";
        for(int i = 0 ;i<conds.size();i++)
        {
            sqlCmd += QString(" AND %1='%2'")
                    .arg(conds[i].fieldName)
                    .arg(firstCond.fieldValue.toString());
        }
    }
    bool ret = m_query.exec(sqlCmd);
    if(!ret)
    {
        m_errString = m_query.lastError().text();
        return false;
    }
    return true;
}

//插入表-insert, 部分值，其他值默认为空
bool SqlFunctions::insert(QString qtablename,QList<ResultInfo> ResultsList)
{
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        return false;
    }

    Q_ASSERT(ResultsList.size()>0);
    QString sqlCmd = QString("INSERT INTO %1 ").arg(qtablename);

    ResultInfo first = ResultsList.takeFirst();
    QString fieldnames = "("+first.fieldName;
    QString values    = "VALUES('"+first.fieldValue.toString()+"'";
    for(int i = 0;i<ResultsList.size();i++)
    {
        fieldnames += ","+ResultsList[i].fieldName;
        values += ",'"+ResultsList[i].fieldValue.toString()+"'";
    }

    fieldnames += ") ";
    values   += ")";
    sqlCmd   += fieldnames+values;
//    qDebug()<<sqlCmd;
    bool ret = m_query.exec(sqlCmd);
    if(!ret)
    {
        m_errString = m_query.lastError().text();
        return false;
    }
    return true;
}


//删除表记录-delete
bool SqlFunctions::deleteRecord(QString qtablename,QList<Condition> conds)
{
    if(!m_db.isOpen()) {
        m_errString = "database is not opened";
        return false;
    }
    QString sqlCmd = QString("DELETE FROM %1 ").arg(qtablename);

    //条件
    if(conds.size()>0)
    {
        Condition firstCond = conds.takeFirst();
        sqlCmd += "WHERE "+firstCond.fieldName+"='"
                +firstCond.fieldValue.toString()+"'";
        for(int i = 0 ;i<conds.size();i++)
        {
            sqlCmd += QString(" AND %1='%2'")
                    .arg(conds[i].fieldName)
                    .arg(firstCond.fieldValue.toString());
        }
    }
    bool ok = m_query.exec(sqlCmd);
    if(!ok)
    {
        m_errString = m_query.lastError().text();
        return false;
    }
    return true;
}

void SqlFunctions::closeDataBase()
{
    if(m_db.isOpen())
        m_db.close();
}

