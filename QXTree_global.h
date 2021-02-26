#ifndef QXTREE_GLOBAL_H
#define QXTREE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QXTREE_LIBRARY)
#  define QXTREE_EXPORT Q_DECL_EXPORT
#else
#  define QXTREE_EXPORT Q_DECL_IMPORT
#endif

#endif // QXTREE_GLOBAL_H
