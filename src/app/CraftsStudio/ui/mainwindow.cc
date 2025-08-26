#include "mainwindow.h"

#include "engine/rs_interactor_switch.h"
#include "engine/rs_interactor_trackball_actor.h"
#include "engine/rs_interactor_trackball_camera.h"
#include "engine/rs_scene_manager.h"
#include "ui/menu_manager.h"
#include "ui/project_tree.h"

#include <QApplication>
#include <QDebug>
#include <QDockWidget>

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

  // side
  _left_dock = new QDockWidget(tr(""), this);
  _left_dock->setAllowedAreas(Qt::LeftDockWidgetArea); // 只允许左右停靠
  _left_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  _left_dock->setFloating(false);
  _left_dock->setMaximumWidth(400);
  addDockWidget(Qt::LeftDockWidgetArea, _left_dock);

  auto treeView = new ProjectTree(this);
  _left_dock->setWidget(treeView); // 将树视图放入dock

  _model_viewer = new ModelViewer(this);
  this->setCentralWidget(_model_viewer);

  _action_factory->setVtkRenderWindow(_model_viewer->getRenderWindow());

  vtkObject::GlobalWarningDisplayOn();
  // vtkSmartPointer<vtkDebugLeaks> debugLeaks = vtkSmartPointer<vtkDebugLeaks>::New();
}

void MainWindow::exit()
{
  qDebug() << "exiting...";

  QApplication::exit();
}
