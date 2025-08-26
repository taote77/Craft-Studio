#include "ui/mainwindow.h"
#include <QApplication>

#ifdef emit
#undef emit  // 取消 Qt 或其他库定义的 emit 宏
#endif
#include <vtkSMPTools.h>

int main(int argc, char* argv[])
{
  vtkSMPTools::SetBackend("TBB"); // 指定并行后端为 TBB
  vtkSMPTools::Initialize(4);     // 初始化线程数（如4线程）

  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());


  QApplication app(argc, argv);

  MainWindow window(nullptr);

  window.show();

  return app.exec();
}
