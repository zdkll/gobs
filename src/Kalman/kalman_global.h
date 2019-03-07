#ifndef  KALMAN_GLOBAL_H
#define KALMAN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(KALMAN_LIBRARY)
#  define KALMANSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KALMANSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KALMAN_GLOBAL_H
