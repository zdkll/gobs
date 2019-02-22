﻿#ifndef AREANODEMANAGER_GLOBAL_H
#define AREANODEMANAGER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AREANODEMANAGER_LIBRARY)
#define AREANODEMANAGERSHARED_EXPORT  Q_DECL_EXPORT
#else
#define AREANODEMANAGERSHARED_EXPORT  Q_DECL_IMPORT
#endif

#endif // AREANODEMANAGER_GLOBAL_H
