#include <QCoreApplication>
#include "sqlfunctions.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SqlFunctions *sqlfunc = SqlFunctions::instance();
    bool ok = sqlfunc->openDataBase("/home/zdk/test","test");
    if(ok)
        qDebug()<<"ok";
    //创建表
    if(!sqlfunc->tables().contains(Node_DevInfo_TABLE)){
        ok = sqlfunc->createTable(Node_DevInfo_TABLE);
        if(ok)
            qDebug()<<" create table ok";
        else{
            qDebug()<<sqlfunc->errorString();
            return -1;
        }

    }

    //insert
    //    QList<ResultInfo> results;
    //    ResultInfo retInfo;
    //    retInfo.fieldName = TABLE_FIELD_DEVICE;
    //    retInfo.fieldValue = "G302";
    //    results.append(retInfo);

    //    retInfo.fieldName = TABLE_FIELD_IP;
    //    retInfo.fieldValue = "192.168.4.100";
    //    results.append(retInfo);

    //    retInfo.fieldName = TABLE_FIELD_DEPOLYED;
    //    retInfo.fieldValue = "1";
    //    results.append(retInfo);

    //    ok = sqlfunc->insert(Node_DevInfo_TABLE,results);
    //    if(ok)
    //        qDebug()<<" insert table ok";
    //    else{
    //        qDebug()<<"insert table error:"<<sqlfunc->errorString();
    //        return -1;
    //    }

    //query
    QList<Condition> conds;
    QSqlQuery query = sqlfunc->query(Node_DevInfo_TABLE,conds,&ok);
    if(ok){
        qDebug()<<" query table ok";
        while(query.next())
            qDebug()<<query.value(TABLE_FIELD_DEVICE)
                   <<query.value(TABLE_FIELD_IP)
                  <<query.value(TABLE_FIELD_AREA)
                 <<query.value(TABLE_FIELD_LINE)
                <<query.value(TABLE_FIELD_STATION)
               <<query.value(TABLE_FIELD_DEPOLYTIME)
              <<query.value(TABLE_FIELD_PICKUPTIME)
             <<query.value(TABLE_FIELD_SPSX)
            <<query.value(TABLE_FIELD_SPSY)
            <<query.value(TABLE_FIELD_SPSZ)
            <<query.value(TABLE_FIELD_DEPOLYED)
            <<query.value(TABLE_FIELD_PICKEDUP);
    }
    else{
        qDebug()<<sqlfunc->errorString();
        return -1;
    }

    //delete
    Condition cond1;
    cond1.fieldName = TABLE_FIELD_DEVICE;
    cond1.fieldValue = "G302";
    conds.append(cond1);
    ok = sqlfunc->deleteRecord(Node_DevInfo_TABLE,conds);
    if(ok)
        qDebug()<<" delete  table ok";
    else
    {
        qDebug()<<" delete  table failed";
        return -1;
    }
    //update
    cond1.fieldName = TABLE_FIELD_DEVICE;
    cond1.fieldValue = "G301";
    conds.append(cond1);
    QList<ResultInfo> resultList;
    ResultInfo retInfo1;
    retInfo1.fieldName = TABLE_FIELD_PICKEDUP;
    retInfo1.fieldValue = 1;
    resultList.append(retInfo1);
    ok = sqlfunc->update(Node_DevInfo_TABLE,conds,resultList);

    conds.clear();
    query = sqlfunc->query(Node_DevInfo_TABLE,conds,&ok);
    if(ok){
        qDebug()<<" query table ok";
        while(query.next())
            qDebug()<<query.value(TABLE_FIELD_DEVICE)
                   <<query.value(TABLE_FIELD_IP)
                  <<query.value(TABLE_FIELD_AREA)
                 <<query.value(TABLE_FIELD_LINE)
                <<query.value(TABLE_FIELD_STATION)
               <<query.value(TABLE_FIELD_DEPOLYTIME)
              <<query.value(TABLE_FIELD_PICKUPTIME)
             <<query.value(TABLE_FIELD_SPSX)
            <<query.value(TABLE_FIELD_SPSY)
            <<query.value(TABLE_FIELD_SPSZ)
            <<query.value(TABLE_FIELD_DEPOLYED)
            <<query.value(TABLE_FIELD_PICKEDUP);
    }
    else{
        qDebug()<<sqlfunc->errorString();
        return -1;
    }

    return a.exec();
}
