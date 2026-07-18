#ifndef CSENGINE_GLOBAL_H
#define CSENGINE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CSENGINE_LIBRARY)
#  define CSENGINE_API Q_DECL_EXPORT
#else
#  define CSENGINE_API Q_DECL_IMPORT
#endif

#endif // CSENGINE_GLOBAL_H
