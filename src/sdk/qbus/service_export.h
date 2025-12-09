#ifndef _QBUS_SERVICE_EXPORT_H_
#define _QBUS_SERVICE_EXPORT_H_

/*******************************************************************************
**                               插件服务导出定义
**
**
*******************************************************************************/

#include <qbus/micro_service.h>
#include <qbus/qbus_global.h>

#include <QList>
#include <QMetaObject>

struct PluginExport
{
  QList<const QMetaObject*> meta_objects;
};

using plugin_api_t = void (*)(void*);

#endif // _QBUS_SERVICE_EXPORT_H_