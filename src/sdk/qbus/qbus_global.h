#ifndef QBUS_GLOBAL_H
#define QBUS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QBUS_LIBRARY)
#define QBUS_API Q_DECL_EXPORT
#else
#define QBUS_API Q_DECL_IMPORT
#endif

#endif // QBUS_GLOBAL_H
