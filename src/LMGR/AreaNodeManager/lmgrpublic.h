#ifndef LMGRPUBLIC_H
#define LMGRPUBLIC_H

//Color_Key---------------------------------------
#define Backdroud_Color       0  //"Backgroud_Color"
#define Shot_Color            1  //"Shot_Color"
#define Fired_Shot_Color      2  //"Shot_Color"
#define Recv_Color            3  //"Recv_Color"
#define Shot_Line_Color       4  //"Shot_Line_Color"
#define Recv_Line_Color       5  //"Recv_Line_Color"
#define Legend_Color          6  //"Legend_Color"
#define Deployed_Recv_Color   7  //"Deployed_Recv_Color"
#define PickedUp_Recv_Color   8  //"PickedUp_Recv_Color"
#define Downloaded_Recv_Color 9  //"Downloaded_Recv_Color"
#define Shot_Text_Color       10  //"Shot_Text_Color"
#define Recv_Text_Color       11  //"Recv_Text_Color"


#include <QtCore>
#include "gpublic.h"
#include "areanodemanager_global.h"



//公共的方法和变量
class AREANODEMANAGERSHARED_EXPORT LmgrPublic
{
public:
    LmgrPublic();

    //计算工区物理坐标范围，每条线的两端站坐标
    static void calAreaScope(AreaDataInfo *areaData,QRectF &rect);

private:

};



#endif // LMGRPUBLIC_H
