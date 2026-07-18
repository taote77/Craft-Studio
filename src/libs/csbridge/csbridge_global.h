#ifndef CSBRIDGE_GLOBAL_H
#define CSBRIDGE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CSBRIDGE_LIBRARY)
#  define CSBRIDGE_API Q_DECL_EXPORT
#else
#  define CSBRIDGE_API Q_DECL_IMPORT
#endif

#endif // CSBRIDGE_GLOBAL_H
