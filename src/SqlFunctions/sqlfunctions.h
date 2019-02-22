#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H

#include <QtCore>
#include <QtSql>

#include "sqlmacro.h"
#include "sqlfunctions_global.h"

typedef QString GobsTable;
#define Node_DevInfo_TABLE   "NodeDevicesTable" //节点设备表


//条件，字段名称-字段对应的值
struct Condition
{
    QString   fieldName;
    QVariant  fieldValue;
};

typedef Condition ResultInfo;

//默认使用增删改查条件都使用 = ，多个条件之间使用 &逻辑关系
class  SQLFUNCTIONSSHARED_EXPORT SqlFunctions
{
public:
    //创建表的类型
    enum TableType
    {
        NodeDeviceTable//GOBS设备信息表，每个A
        //....
    };
    ~SqlFunctions();

    static SqlFunctions *instance();

    //创建打开项目数据库，根据数据
    bool openDataBase(const QString &projPath,const QString projName);

    QStringList tables(){return m_db.tables();}
    //创建表(默认为 设备表)
    bool createTable(const QString &tablename
                     ,const TableType &type = NodeDeviceTable);

    bool dropTable(const QString &tableName);

    bool clearTable(const QString &tableName);

    //查询表-query，返回返回对应的query结果对象
    QSqlQuery query(QString qtablename,QList<Condition> conds,bool *ok = 0);
    QSqlQuery query(QString qtablename,QStringList feildList,bool *ok= 0);
    QSqlQuery query(QString qtablename,QStringList feildList
                    ,QList<Condition> conds,bool *ok= 0);

    //修改表-update
    bool update(QString qtablename,QList<Condition> conds,QList<ResultInfo> ResultsList );

    //插入表-insert, 部分值，其他值默认为空
    bool insert(QString qtablename,QList<ResultInfo> ResultsList);

    //删除表记录-delete
    bool deleteRecord(QString qtablename,QList<Condition> conds);

    void closeDataBase();

    QString errorString() const {return m_errString;}

private:
    SqlFunctions(){}
    class DeleteFunction
    {
        ~DeleteFunction()
        {
            if(SqlFunctions::m_instance)
                delete SqlFunctions::m_instance;
        }
    };
    static DeleteFunction funcDeleter;

    QSqlDatabase  m_db;
    QString       m_errString;
    QSqlQuery     m_query;
    static SqlFunctions *m_instance;
};


#endif // SQLFUNCTIONS_H
