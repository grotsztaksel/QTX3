#ifndef QTX3_GLOBAL_H
#define QTX3_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTX3_MSVC)
#if defined(QTX3_LIBRARY)
#  define QTX3_EXPORT Q_DECL_EXPORT
#else
#  define QTX3_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QTX3_EXPORT
#endif

#endif // QTX3_GLOBAL_H
