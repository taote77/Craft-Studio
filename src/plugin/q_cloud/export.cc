#include "cloud_service.h"
#include "qbus/service_export.h"

extern "C" void getServicesMeta(void* pex)
{
  PluginExport* pexport = reinterpret_cast<PluginExport*>(pex);
  pexport->meta_objects.push_back(&CloudService::staticMetaObject);
}
