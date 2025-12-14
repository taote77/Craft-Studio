#include "qbus/service_export.h"

#include "sys_service.h"
#include "wifi_service.h"

extern "C" void getServicesMeta(void* pex)
{
  PluginExport* pexport = reinterpret_cast<PluginExport*>(pex);

  pexport->meta_objects.push_back(&SysService::staticMetaObject);
  pexport->meta_objects.push_back(&WifiService::staticMetaObject);
}
