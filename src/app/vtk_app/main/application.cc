#include "application.h"

namespace APP
{

Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
{
}

void Application::OpenFiles(const QString& filename) {}

void Application::CloseFiles(const QString& filename) {}

} // namespace APP
