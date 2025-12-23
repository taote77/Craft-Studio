#include "qbus/service_export.h"

#include "clipper_service.h"
#include "clipper_widget.h"

extern "C" void getServicesMeta(void* pex)
{
  PluginExport* pexport = reinterpret_cast<PluginExport*>(pex);

  pexport->meta_objects.push_back(&ClipperService::staticMetaObject);
  pexport->meta_objects.push_back(&ClipperWidget::staticMetaObject);
}
