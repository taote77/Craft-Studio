#include "qbus/application.h"
#include "qbus/micro_service.h"
#include <memory>

namespace qbus
{

Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
{
  init();
}

Application::~Application()
{
  //
}

void Application::init()
{
  //

  QDir work_dir{ "/home/shane/workspace/CraftStudio/build/output/plugin/Debug" };

  _service_manager.loadServices(work_dir);
}

} // namespace qbus