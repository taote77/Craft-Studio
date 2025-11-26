#ifndef ULTRABUS_GLOBAL_H
#define ULTRABUS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PDK_LIBRARY)
#  define PDK_API Q_DECL_EXPORT
#else
#  define PDK_API Q_DECL_IMPORT
#endif


#endif // ULTRABUS_GLOBAL_H
