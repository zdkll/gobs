#include "spsfiles.h"

SPSFiles::SPSFiles()
{

}


int gobs_sps_file_SR_read(const QString &fileName,StationInfo *stationInfo)
{
    if(!stationInfo)
    {
        return SPS_READ_DATA_PTR_EMPTY;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return SPS_READ_FILE_OPEN_FAILED;

    QTextStream out(&file);
    QString   lineStr;
    //找到第一条数据记录--------------------
    while(!out.atEnd())
    {
        lineStr = out.readLine();

        //找到数据记录，除去头(以“H”开头)
        if(lineStr.startsWith("R"))
        {
            stationInfo->type = "R";
            break;
        }
        if(lineStr.startsWith("S"))
        {
            stationInfo->type = "S";
            break;
        }
    }

    //统计每条线----------------------
    //线列表
    QList<StaLine *> staLines;
    QList<Station> line_stations;

    float   line = lineStr.mid(1,16).toFloat();
    int     station_num = 0;
    float   temp_line = line;
    char    type = lineStr.at(0).toLatin1();
    while(!out.atEnd())
    {
        //qDebug()<<lineStr;
        if(station_num>0){
            lineStr = out.readLine();
            temp_line = lineStr.mid(1,16).toFloat();
        }

        Station station;
        bool ok;
        station.line = temp_line;
        station.Id     = lineStr.at(0).toLatin1();
        station.sp   = lineStr.mid(17,8).toInt();
        station.sidx = lineStr.mid(25,1).toShort(&ok,0);
        station.stat = lineStr.mid(28,4).toFloat();
        station.sdepth = lineStr.mid(32,4).toFloat();
        station.elev = lineStr.mid(36,4).toFloat();
        station.wtime= lineStr.mid(40,2).toShort();
        station.swdep= lineStr.mid(42,4).toFloat();
        station.x      = lineStr.mid(46,9).toFloat();
        station.y      = lineStr.mid(55,10).toFloat();
        station.del    = lineStr.mid(65,6).toFloat();

        station.day     = lineStr.mid(71,3).toShort();
        station.hour    = lineStr.mid(74,2).toShort();
        station.minute  = lineStr.mid(76,2).toShort();
        station.sec     = lineStr.mid(78,2).toShort();

        station.unass   = lineStr.mid(80,1).toShort();

        //保存线---------------------------------------
        if(line != temp_line)
        {
            StaLine   *staline = new StaLine;
            staline->staNum   = line_stations.size();
            staline->line     = line;
            staline->type     = type;
            //保存所有站点
            staline->stations = new Station[staline->staNum];

            for(int i =0 ;i<line_stations.size();i++)
            {
                staline->stations[i] = line_stations.at(i);
            }
            staLines.append(staline);

            line  = temp_line;
            //qDebug()<<"line:"<<line;
            line_stations.clear();
        }
        line_stations.append(station);
        station_num++;
    }

    //保存最后一条线-------------------------------------
    if(line_stations.size()>0)
    {
        StaLine   *staLine = new StaLine;
        staLine->staNum   = line_stations.size();
        staLine->line     = line;
        staLine->type     = type;
        //保存所有站点
        staLine->stations = new Station[staLine->staNum];

        for(int i =0 ;i<line_stations.size();i++)
        {
            staLine->stations[i] = line_stations.at(i);
        }
        staLines.append(staLine);
    }

    //所有线列表转换成StationInfo-----
    stationInfo->lineNum = staLines.size();
    stationInfo->staLines = new StaLine[stationInfo->lineNum];

    for(int i = 0;i<staLines.size();i++){
        stationInfo->staLines[i] = *staLines.at(i);
    }

    staLines.clear();

    //debug  test-------------------------------------------
    //    qDebug()<<"type:"<<stationInfo->type
    //           <<"line num:"<<stationInfo->lineNum
    //          <<" stations:"<<station_num;
    //    for(int i=0 ;i<stationInfo->lineNum;i++)
    //    {
    //        qDebug()<<"line:"<<stationInfo->staLines[i].line
    //               <<"station num:"<<stationInfo->staLines[i].staNum;
    //        for(int j=0;j<stationInfo->staLines[i].staNum;j++){
    //            qDebug()<<"line:"<<stationInfo->staLines[i].stations[j].line
    //                   <<"sp:"<<stationInfo->staLines[i].stations[j].sp
    //                  <<"sidx:"<<stationInfo->staLines[i].stations[j].sidx
    //                 <<"x:"<<stationInfo->staLines[i].stations[j].x
    //                <<"y:"<<stationInfo->staLines[i].stations[j].y;
    //        }
    //    }

    file.close();
    //-----------------------------------------
    // qDebug()<<"read file:"<<fileName<<" finished";
    return 0;
}

int gobs_sps_file_X_read(const QString &fileName,RelationInfo *relationInfo)
{
    if(!relationInfo)
    {
        return SPS_READ_DATA_PTR_EMPTY;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return SPS_READ_FILE_OPEN_FAILED;


    QTextStream out(&file);
    QString   lineStr;
    //找到第一条数据记录--------------------
    while(!out.atEnd())
    {
        lineStr = out.readLine();
        if(lineStr.startsWith("X"))
        {
            relationInfo->type = 'X';
            break;
        }
    }

    //记录保存每条关系--------------------
    QList<Relation > relations;
    int    relation_num = 0;
    while(!out.atEnd())
    {
        //qDebug()<<lineStr;
        //读取一行
        if(relation_num>0)
            lineStr = out.readLine();

        Relation relation;
        relation.tape      = lineStr.mid(1,6).toInt();
        relation.recordNo  = lineStr.mid(7,4).toInt();
        relation.recordInc = lineStr.mid(11,1).toShort();
        relation.instrNo   = lineStr.mid(12,1).toShort();

        relation.shotLine  = lineStr.mid(13,16).toFloat();
        relation.ep        = lineStr.mid(29,8).toInt();

        relation.sid       = lineStr.mid(37,1).toShort();
        relation.firstTrace= lineStr.mid(38,4).toInt();
        relation.lastTrace = lineStr.mid(42,4).toInt();
        relation.incTrace  = lineStr.mid(46,1).toShort();

        relation.recvLine  = lineStr.mid(47,16).toFloat();
        relation.firstRecv = lineStr.mid(63,8).toInt();
        relation.lastRecv  = lineStr.mid(71,8).toInt();

        relations.append(relation);
        relation_num++;
    }

    file.close();

    //保存为数组
    relationInfo->relationNum = relation_num;

    Relation *array_relations = new Relation[relation_num];
    for(int i=0;i<relation_num;i++){
        array_relations[i] = relations.at(i);
    }
    relationInfo->relations = array_relations;

    qDebug()<<"read x file:" + fileName +" finished";
    return 0;
}

int gobs_sps_files_SR_read(QStringList spsFileNames,AreaDataInfo *areaDataInfo,QString *errorStr)
{
    if(spsFileNames.size()<3)
        return -1;
    //R file read---------------------------------------
    QString fileName = spsFileNames[0];
    StationInfo *recv_stationInfo = new StationInfo;
    int ok = gobs_sps_file_SR_read(fileName,recv_stationInfo);
    if(ok != 0)
    {
        delete recv_stationInfo;
        if(errorStr)
            *errorStr = "Read R File Failed.";
        return ok;
    }

    //S file read---------------------------------------
    fileName = spsFileNames[1];
    StationInfo *shot_stationInfo = new StationInfo;
    ok = gobs_sps_file_SR_read(fileName,shot_stationInfo);
    if(ok != 0)
    {
        delete recv_stationInfo;
        delete shot_stationInfo;
        if(errorStr)
            *errorStr = "Read R File Failed.";
        return ok;
    }

    //保存数据------------------------------------------
    areaDataInfo->recvStationInfo = recv_stationInfo;
    areaDataInfo->shotStationInfo = shot_stationInfo;


    return 0;
}

int gobs_sps_files_read(QStringList spsFileNames,AreaDataInfo *areaDataInfo,QString *errorStr)
{
    if(spsFileNames.size()<3)
        return -1;
    //R file read---------------------------------------
    QString fileName = spsFileNames[0];
    StationInfo *recv_stationInfo = new StationInfo;
    int ok = gobs_sps_file_SR_read(fileName,recv_stationInfo);
    if(ok != 0)
    {
        delete recv_stationInfo;
        if(errorStr)
            *errorStr = "Read R File Failed.";
        return ok;
    }

    //S file read---------------------------------------
    fileName = spsFileNames[1];
    StationInfo *shot_stationInfo = new StationInfo;
    ok = gobs_sps_file_SR_read(fileName,shot_stationInfo);
    if(ok != 0)
    {
        delete recv_stationInfo;
        delete shot_stationInfo;
        if(errorStr)
            *errorStr = "Read R File Failed.";
        return ok;
    }

    //X file read，可选---------------------------------------
    fileName = spsFileNames[2];
    QFile file(fileName);
    if(file.exists()){
        RelationInfo *relationInfo = new RelationInfo;
        ok = gobs_sps_file_X_read(fileName,relationInfo);
        if(ok != 0)
        {
            delete recv_stationInfo;
            delete shot_stationInfo;
            delete relationInfo;
            if(errorStr)
                *errorStr = "Read X File Failed.";
            return ok;
        }
        areaDataInfo->relationInfo    = relationInfo;
    }

    //保存数据------------------------------------------
    areaDataInfo->recvStationInfo = recv_stationInfo;
    areaDataInfo->shotStationInfo = shot_stationInfo;

    return 0;
}

