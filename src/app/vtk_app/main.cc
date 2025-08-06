#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  MainWindow window(nullptr);

  window.show();

  return app.exec();
}
