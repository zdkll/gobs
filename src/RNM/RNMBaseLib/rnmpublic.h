#ifndef RNMPUBLIC_H
#define RNMPUBLIC_H

typedef int UpdateCmd;
#define State_Update         0x01    //只更新状态
#define Ftp_Update           0x02    //只更新Ftp
#define All_Update           (State_Update|Ftp_Update) //两者都更新

//更新容量
#define Ftp_Update_Memory    (0x04|Ftp_Update|State_Update)
//更新下载进度
#define Ftp_Update_Progress  (0x08|Ftp_Update)
//更新采样率
#define Ftp_Update_SampleFrequency  (0x18|Ftp_Update)   //修改采样率完成

#define Ftp_Update_Note             (0x20|Ftp_Update)

//---------------------------------------------------------------
//节点设备配置文件
#define Node_NODAL_FILE      "NODAL.CFG"

#include <QtCore>
#include <QColor>

#include "rnmbaselib_global.h"

enum LimitOperation
{
    Equal = 0,
    Big   = 1,
    Less  = 2
};

//Limit:颜色-Big/Less-Value-suffix
struct Limit
{
    QColor           color;
    LimitOperation   operation;
    float            value;
    QString          suffix;
};

struct LimitInfo
{
    LimitOperation operate;
    QList<Limit>   limits;
};

typedef QPair<float,QColor>  ValStop;
typedef QVector<ValStop>     ValStops;
struct  LimitValStops
{
    LimitOperation operate;
    ValStops       valStops;
};

struct LimitData
{
    LimitOperation operate;
    float          first;
    float          second;
};

//所有节点数据信息
struct StatisticsData
{
    StatisticsData()
        :totalNodes(0)
        ,avaliableNodes(0)
        ,nodeBoxes(0)
        ,updateInterval(5)
        ,downloadingNode(0){}
    int totalNodes;
    int avaliableNodes;
    int nodeBoxes;
    //其他
    int updateInterval;
    int downloadingNode;
};

//当前工作执行内容:查询文件，下载文件，设置采样率
enum FtpWork
{
    NoneWork           = 0,
    Updating           = 1,
    DownLoadFiles      = 2,
    ClearMemory        = 3,
    SetSampleFrequecy  = 4
};

//是否按照时间段下载
enum DownloadType
{
    DownloadAll      = 0,
    DownloadInPeriod =1
};


QString workName(const int &work);

//Node Status Color-----------------
const QColor Node_Status_Disable_Color = QColor(Qt::lightGray);
const QColor Node_Status_Normal_Color  = QColor(Qt::green);
const QColor Node_Status_Warning_Color = QColor(Qt::yellow);
const QColor Node_Status_Error_Color   = QColor(Qt::red);
const QColor Node_SelfTest_Warning_Color= QColor(Qt::red);
const QColor Node_Downloaded_Color      = QColor(Qt::blue);

#ifdef __cplusplus
extern "C"{
#endif

RNMBASELIBSHARED_EXPORT  bool isNodeIP(const uint &ipV4);
RNMBASELIBSHARED_EXPORT  uint ip2DeviceNo(const uint &ipV4);
#ifdef __cplusplus
}
#endif

class NodeItem;
class NodeMediator;
class NodeQuery;
class StatusControler;
class FtpManager;

//节点对象
class RNMBASELIBSHARED_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    enum DownloadState
    {
        NoDownload = 0,
        Downloading = 1,
        Wait        = 2,
        Complished  = 3,
        Error = 4
    };
    enum FtpError
    {
        NoError                 = 0,
        DownloadError           = 1,
        ClearMemoryError        = 2,
        SetSampleFrequencyError = 3
    };
    enum State
    {
        Disable = 0,    //gray
        Normal  = 1,    //green
        State_Warning = 2,//yellow
        State_Error   = 3,//red
        SelfTest_Ok   = Normal,
        SelfTest_Warning=4,
        UnDownload    = Normal,
        Downloaded    = 5
    };
    Node(QObject *parent = 0);
    ~Node();

    //对应显示的GraphicsItem 项
    void setItem(NodeItem *item){nodeItem = item;}

    void setNodeMediator(NodeMediator *mediator){m_nodeMediator = mediator;}

    void setStateCtrler(StatusControler *stCtrler){m_stCtrler = stCtrler;}

    //TCP/IP 状态刷新-------------------
    //刷新控制
    void startRefresh(const int &interval);
    void stopRefresh();
    void refreshState();
    bool isRunning() const;

    //刷新参数，包括Ftp 和 tcp/Ip参数
    void updateNode(const UpdateCmd &updateType = All_Update);

    //ftp 操作
    void startFtpWork(const FtpWork &ftpWk,const QVariant &arg);
    void stopFtpWork();
    void finishedFtpWork(const int &ftpWk,const bool &err);

    //id---------------------------------
    const int &id(){return m_id;}
    void setId(const int &id){m_id = id;}

    //状态查询设置---------------------------------------------------------
    //no
    uint no() const {return nodeData.no;}
    void setDevNo(const uint &no){nodeData.no = no;}

    //ip
    uint ip() const {return nodeData.ip;}
    void setIP(const uint &ip){nodeData.ip = ip;}

    //state
    bool state()const {return nodeData.state;}
    void setState(const bool &state){nodeData.state = state;}

    //sVoltage
    float voltage()const {return nodeData.sVoltage;}
    void setVoltage(const float &voltage){nodeData.sVoltage = voltage;}

    //sTemper
    float temperature()const {return nodeData.sTemper;}
    void  setTemperature(const float &temperature){nodeData.sTemper = temperature;}

    //sPress
    float pressure()const {return nodeData.sPress;}
    void  setPressure(const float &pressure){nodeData.sPress = pressure;}

    //sEleCurr
    float current()const {return nodeData.sEleCurr;}
    void  setCurrent(const float &current){nodeData.sEleCurr = current;}

    //sChargeVolt
    float chargeVolt()const {return nodeData.sChargeVolt;}
    void  setChargeVolt(const float &chargeVolt){nodeData.sChargeVolt = chargeVolt;}

    //sMemory
    float memory()const {return nodeData.sMemory;}
    void  setMemory(const float &memory){nodeData.sMemory = memory;}

    //download
    int   downloaded()const {return nodeData.downloadPercent;}
    void  setDownloadPercent(const int &percent){nodeData.downloadPercent = percent;}

    //sampleFrequency
    int   sampleFrequency()const {return nodeData.sampleFrequency;}
    void  setSampleFrequency(const int &frequency){nodeData.sampleFrequency = frequency;}

    //下载完成
    bool isDownloaded(){return nodeData.downloaded;}
    void setDownloaded(const bool &downloaded){nodeData.downloaded = downloaded;}

    QString infoString(){return m_infoString;}
    void    setInfoString(const QString &infoStr){m_infoString = infoStr;}

    State *stateValue(){return m_state;}
    State  stateById(const int &id){
        return m_state[id+1];
    }

signals:
    void signalFtpStart(const int &ftpWk,const QVariant &arg);

private:
    struct NodeData
    {
        NodeData();
        uint      no;//设备编号
        uint      ip;//IP地址
        bool      state; //true:connected;false：dis

        //设备参数
        float     sVoltage;//电压,4位小数
        float     sTemper; //温度,3位小数
        float     sPress;  //仓压,3位小数
        float     sEleCurr;//工作电流,3位小数
        float     sChargeVolt;//充电电压,3位小数
        float     sMemory;

        int       downloadPercent;//下载进度
        int       sampleFrequency;//采样频率
        bool      downloaded;//是否下载完成
    };
    //节点数据
    NodeData      nodeData;
    State         m_state[7];//参数状态
    int           m_id;
    NodeItem     *nodeItem;
    QString       m_infoString;     //提示信息

    NodeMediator    *m_nodeMediator;
    StatusControler *m_stCtrler;
    DownloadState    m_downloadState;
    FtpError         m_ftpError;

    //节点设备查询
    NodeQuery        *m_nodeQuery;
    FtpManager       *m_ftpManager;

    QColor           *m_colors;
};

//状态控制
class RNMBASELIBSHARED_EXPORT StatusControler
{
public:
    void setFilters(const int &status);
    void setLimitData(const QVector<LimitData> &limitData);

    void setNodes(QVector<Node *> nodes);

    int displayFilter() const{
        return m_filters ;
    }

    //根据节点参数计算颜色，并返回当前状态编号
    void calNodeState(Node *node) ;

    static Node::State calState(const float &value,const LimitData &limitData);

private:
    int                    m_filters;
    QVector<LimitData>     m_LimitData;

    QVector<Node *>        m_nodes;
    QColor                *m_colors;
};

class RNFtpManager;
class StatusReportDlg;
//中介者，用于Node 和FtpManager,StatusReport 之间通信
class RNMBASELIBSHARED_EXPORT NodeMediator : public QObject
{
    Q_OBJECT
public:
    NodeMediator(QObject *parent = 0);
    void setRNFtpManager(RNFtpManager   *ftpManager);
    void setStatusReport(StatusReportDlg *statusReport);
    void setNodes(const QVector<Node*> &nodes);

    //Status
    void nodeToStatusReport(const UpdateCmd &msg,Node *node);

    //Ftp
    void nodeToFtpManagerMsg(Node *node,const UpdateCmd &msg);
    void finishedFtpWork(Node *node,const int &ftpWk,const bool &err);

private:
    RNFtpManager     *m_ftpManager;
    StatusReportDlg  *m_statusReport;
    QVector<Node*>    m_nodes;
};



#endif // RNMPUBLIC_H
