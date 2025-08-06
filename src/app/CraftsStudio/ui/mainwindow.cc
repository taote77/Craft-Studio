#include "mainwindow.h"
#include "menu_manager.h"

#include <vtkDebugLeaks.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupUI();
}

void MainWindow::setupUI()
{
  this->resize(1200, 900);

  _action_factory = new ActionFactory(this);

  connect(_action_factory, &ActionFactory::sigExit, this, &MainWindow::exit);

  auto menu_mgr = new MenuManager(this);
  menu_mgr->init(*this, *_action_factory);

  // dock
  addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

  dockLayout = new QVBoxLayout();
  layoutContainer.setLayout(dockLayout);

  controlDock.setWidget(&layoutContainer);

  // randomizeButton.setText("Randomize");

  auto model_mgr = new ModelManager(this);
  dockLayout->addWidget(model_mgr);

  // Render area.
  _vtkRenderWidget = new QVTKOpenGLNativeWidget();
  this->setCentralWidget(_vtkRenderWidget);

  // VTK part.
  _vtkRenderWindow = vtkNew<vtkGenericOpenGLRenderWindow>();
  _vtkRenderWindow->SetMultiSamples(0);
  _vtkRenderWindow->SetAlphaBitPlanes(1);

  _vtkRenderWidget->setRenderWindow(_vtkRenderWindow);

  _action_factory->setVtkRenderWindow(_vtkRenderWindow);

  vtkObject::GlobalWarningDisplayOn();
  vtkSmartPointer<vtkDebugLeaks> debugLeaks = vtkSmartPointer<vtkDebugLeaks>::New();

  _vtkRenderer = vtkNew<vtkRenderer>();

  _action_factory->setVtkRenderer(_vtkRenderer);

  _vtkRenderWindow->AddRenderer(_vtkRenderer);
}

void MainWindow::exit()
{
  qDebug() << "exiout";

  QApplication::exit();
}
