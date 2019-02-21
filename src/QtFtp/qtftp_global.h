#ifndef QTFTP_GLOBAL_H
#define QTFTP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTFTP_LIBRARY)
#  define QTFTPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTFTPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTFTP_GLOBAL_H
