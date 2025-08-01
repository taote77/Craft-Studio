//#include <QVTKOpenGLNativeWidget.h>
//#include <vtkActor.h>
//#include <vtkDataSetMapper.h>
//#include <vtkDoubleArray.h>
//#include <vtkGenericOpenGLRenderWindow.h>
//#include <vtkPointData.h>
//#include <vtkProperty.h>
//#include <vtkRenderer.h>
//#include <vtkSphereSource.h>

//#include <QApplication>
//#include <QDockWidget>
//#include <QGridLayout>
//#include <QLabel>
//#include <QMainWindow>
//#include <QPointer>
//#include <QPushButton>
//#include <QVBoxLayout>

//#include <cmath>
//#include <cstdlib>
//#include <random>

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  MainWindow window(nullptr);

  window.show();

  return app.exec();
}
