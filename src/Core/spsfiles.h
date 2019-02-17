#ifndef SPSFILES_H
#define SPSFILES_H

#include <QtCore>
#define  Unit_Name_Maxlen   36


class SPSFiles
{
public:
    SPSFiles();
};


//站-----------
struct Station
{
    Station():stat(0.0)
      ,sdepth(0.0)
      ,elev(0.0)
      ,wtime(0)
      ,swdep(0.0)
      ,del(0.0)
      ,day(0)
      ,hour(0)
      ,minute(0)
      ,sec(0)
    ,unass(0){}
    float   line;//线号  2-17
    int     sp;//站点号，station poiter 18-25
    short   sidx;//点索引 26
    float   stat;//静校正量 29-32
    float   sdepth;//点深度 33-36
    float   elev;//基准面 37-40 selev gelev
    short   wtime;//井口时间 41-42 震源
    float   swdep;//water deep，水深 43- 46，震源
    float    x; //x 坐标值(E)' 47-55
    float    y; //y 坐标值(N) 56-65
    float   del;//地面高程    66-71 gdel,sdel

    short day;  //日期 72-74
    short hour;   //时 75-76
    short minute; //分 77-78
    short sec;    //秒 79-80

    //额外标识空间，如果是炮点，表示是否放炮
    short unass;
    //---------------
    char    Id; //标识 R / S /X
    char    unit[Unit_Name_Maxlen];//对应投放的设备号
};

//线-----------
struct StaLine
{
    StaLine():stations(0){}
    ~StaLine(){

    }
    void clear()
    {
        if(stations){
            delete[] stations;
            stations = 0;
        }
        staNum = 0;
    }
    char      type;
    float     line;        //线号
    int       staNum;      //站数量
    Station  *stations;    //站数组
};

//站信息结构体--
struct StationInfo
{
    StationInfo():lineNum(0),staLines(0){}
    ~StationInfo(){
    }
    void clear()
    {
        for(int i=0;i<lineNum;i++)
            staLines[i].clear();
        if(staLines){
            delete[] staLines;
            staLines = 0;
        }
        lineNum = 0;
    }
    QString      type;  //读取文件类型，'R'/'S'两种
    int       lineNum;  //线数量
    StaLine  *staLines;
};


//关系结构体
struct Relation
{
    Relation():tape(0)
      ,recordNo(0)
      ,recordInc(0)
      ,instrNo(0)
      ,shotLine(0.0)
      ,ep(0)
      ,sid(0)
      ,firstTrace(0)
      ,lastTrace(0)
      ,incTrace(1)
      ,recvLine(0.0)
      ,firstRecv(0)
      ,lastRecv(0)
      ,ridx(0){}

    int     tape;//野外磁带号  2-7
    int     recordNo;//野外记录文件号，8-11
    short   recordInc;//野外记录增量 12
    short   instrNo;//仪器代码 13

    float   shotLine;//炮点线号 14-29
    int     ep;  //炮点桩号,炮号 30-37

    short   sid; //炮点索引标识38
    int     firstTrace;//起始道号 39-42
    int     lastTrace;//终止道号 43-46
    short   incTrace; //道号增量 47

    float   recvLine;//接收线号  48-63
    int     firstRecv;//起始接收点号 64-71
    int     lastRecv; //终止接收点号 72-79

    short   ridx;   //接收点索引     80
    //----------------------------
    char    Id; //标识 R / S /X
};


struct RelationInfo
{
    RelationInfo()
        :relationNum(0)
        ,relations(0){}
    ~RelationInfo(){
        clear();
    }
    void clear(){
        if(relations)
        {
            relationNum = 0;
            delete relations;
            relations = 0;
        }
    }

    int       relationNum; //关系数
    Relation *relations;   //关系信息数组
    char      type; //标识 R / S /X
};

struct AreaDataInfo
{
    AreaDataInfo():recvStationInfo(0)
      ,shotStationInfo(0)
      ,relationInfo(0){}
    ~AreaDataInfo()
    {
        clearData();
    }

    void clearData(){
        if(recvStationInfo){
            recvStationInfo->clear();
            delete recvStationInfo;
            recvStationInfo  = 0;
        }
        if(shotStationInfo){
            shotStationInfo->clear();
            delete shotStationInfo;
            shotStationInfo = 0;
        }
        if(relationInfo){
            relationInfo->clear();
            delete relationInfo;
            relationInfo = 0;
        }
    }

    //炮检点信息
    StationInfo *recvStationInfo;
    StationInfo *shotStationInfo;

    //关系结构体信息,按照炮线号-炮号-检波线号-检波点号检索---------
    RelationInfo *relationInfo;
};

//---------------------------------------
#define   SPS_READ_DATA_PTR_EMPTY     1
#define   SPS_READ_FILE_OPEN_FAILED   2

//functions ,读取SR 文件，格式相同-------------------------------
extern "C" int gobs_sps_file_SR_read(const QString &fileName,StationInfo *stationInfo);

//读取X文件-------------------------
extern "C" int gobs_sps_file_X_read(const QString &fileName,RelationInfo *relationInfo);

//只读取SR 文件
extern "C" int gobs_sps_files_SR_read(QStringList spsFileNames,AreaDataInfo *areaDataInfo,QString *errorStr = 0);

//
extern "C" int gobs_sps_files_read(QStringList spsFileNames,AreaDataInfo *areaDataInfo,QString *errorStr = 0);

#endif // SPSFILES_H
