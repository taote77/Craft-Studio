#include "main_window.h"
#include <application.h>

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  APP::Application app(argc, argv);

  auto& window = ApplicationWindow::getAppWindow();
  window.show();

  return app.exec();
}
