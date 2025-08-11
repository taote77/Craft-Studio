#include "mainwindow.h"
#include "menu_manager.h"

#include <qdockwidget.h>
#include <vtkDebugLeaks.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

#include "engine/rs_interactor_style.h"
#include "engine/rs_interactor_switch.h"

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupUI();
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
  _right_button_menu->exec(QCursor::pos());
}

void MainWindow::setupUI()
{
  this->resize(1200, 900);

  _action_factory = new ActionFactory(this);

  connect(_action_factory, &ActionFactory::sigExit, this, &MainWindow::exit);

  auto menu_mgr = new MenuManager(this);
  menu_mgr->init(*this, *_action_factory);

  // dock
  _project_tree = new ProjectTree(this);
  addDockWidget(Qt::LeftDockWidgetArea, _project_tree);

  auto model_mgr = new ModelManager(this);
  _project_tree->addWidget(model_mgr);

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

  // 自定义交互
  {
    auto interactor = _vtkRenderWindow->GetInteractor();

    interactor->RemoveAllObservers();

    auto custom_style = vtkSmartPointer<RSInteractorStyle>::New();

    interactor->SetInteractorStyle(custom_style);

    _vtkRenderWidget->installEventFilter(this);
  }

  _right_button_menu = new QMenu(this);

  auto style_menu = _right_button_menu->addMenu("3D Interactor Style");

  auto action = new QAction("Interact Style");
  connect(action, &QAction::triggered, this,
    [this]
    {
      auto style = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
      auto interactor = _vtkRenderWindow->GetInteractor();
      interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);

  action = style_menu->addAction("Camera Track");
  connect(action, &QAction::triggered, this,
    [this]
    {
      auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
      auto interactor = _vtkRenderWindow->GetInteractor();
      interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);

  action = style_menu->addAction("Rubber Band Zoom");
  connect(action, &QAction::triggered, this,
    [this]
    {
      auto style = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
      auto interactor = _vtkRenderWindow->GetInteractor();
      interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);
}

void MainWindow::exit()
{
  qDebug() << "exiting...";

  QApplication::exit();
}
