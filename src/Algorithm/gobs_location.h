#ifndef GOBS_LOCATION_H
#define GOBS_LOCATION_H

#include <QPointF>

//计算gobs位置函数

#include "algorithm_global.h"

#ifdef __cplusplus
extern "C"{
#endif

struct Point
{
  float x;
  float y;
  float z;
};
/*p1,p2,p3  为甲板机三个位置
 *t1,t2,t3 为信号延时
 *watervel 为信号传播数据
 * p_obs 输出，gobs定位结果
 */
ALGORITHMSHARED_EXPORT int cal_location(Point p1, Point p2, Point p3
                                        , float t1, float t2, float t3, float watervel,Point *p_obs);

#ifdef __cplusplus
}
#endif


#endif // GOBS_LOCATION_H
