#include "qbus/application.h"
#include "qbus/micro_service.h"
#include <memory>

namespace qbus
{

Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
{

  QDir work_dir{ "/home/shane/workspace/CraftStudio/build/output/plugin/Debug" };

  init(work_dir);
}

Application::~Application()
{
  //
}

void Application::init(const QDir& work_dir)
{
  //

  _service_manager.loadServices(work_dir);
}

} // namespace qbus