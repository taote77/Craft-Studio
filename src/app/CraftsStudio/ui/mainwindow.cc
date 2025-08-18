#include "mainwindow.h"

#include "engine/rs_interactor_switch.h"
#include "engine/rs_interactor_trackball_actor.h"
#include "engine/rs_interactor_trackball_camera.h"
#include "engine/rs_scene_manager.h"

#include "ui/menu_manager.h"
#include "ui/project_tree.h"
#include "ui/slice_panel.h"
#include "ui/transform_panel.h"

#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>

#include <vtkDebugLeaks.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupUI();
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
  return QMainWindow::contextMenuEvent(event);
  // _right_button_menu->exec(QCursor::pos());
}

void MainWindow::setupUI()
{
  this->resize(1200, 900);

  _action_factory = new ActionFactory(this);

  connect(_action_factory, &ActionFactory::sigExit, this, &MainWindow::exit);

  auto menu_mgr = new MenuManager(this);
  menu_mgr->init(*this, *_action_factory);

  // dock
  _left_dock = new QDockWidget(tr(""), this);
  _left_dock->setAllowedAreas(Qt::LeftDockWidgetArea); // 只允许左右停靠
  _left_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  _left_dock->setFloating(false);
  _left_dock->setMaximumWidth(400);
  addDockWidget(Qt::LeftDockWidgetArea, _left_dock);

  auto treeView = new ProjectTree(this);
  _left_dock->setWidget(treeView); // 将树视图放入dock

  _vtkRenderWidget = new QVTKOpenGLNativeWidget();
  this->setCentralWidget(_vtkRenderWidget);

  _vtkRenderWindow = vtkNew<vtkGenericOpenGLRenderWindow>();
  _vtkRenderWindow->SetMultiSamples(0);
  _vtkRenderWindow->SetAlphaBitPlanes(1);

  _vtkRenderWidget->setRenderWindow(_vtkRenderWindow);

  _action_factory->setVtkRenderWindow(_vtkRenderWindow);

  vtkObject::GlobalWarningDisplayOn();
  vtkSmartPointer<vtkDebugLeaks> debugLeaks = vtkSmartPointer<vtkDebugLeaks>::New();

  _vtkRenderer = vtkNew<vtkRenderer>();

  _vtkRenderWindow->AddRenderer(_vtkRenderer);

  {
    auto transform_panel = new TransformPanel(_vtkRenderWidget);
    transform_panel->move(10, 50);
    transform_panel->resize(150, 600);

    connect(transform_panel, &TransformPanel::modeChanged, this,
      [this](EditMode mode)
      {
        qDebug() << "当前模式:" << (int)mode;

        auto interactor = _vtkRenderWindow->GetInteractor();
        interactor->RemoveAllObservers();

        if (mode == EditMode::None)
        {
          auto custom_style = vtkSmartPointer<RSInteractorTrackCamera>::New();
          custom_style->SetRenderer(_vtkRenderer);
          interactor->SetInteractorStyle(custom_style);
        }
        else
        {
          auto custom_style = vtkSmartPointer<RSInteractorTrackActor>::New();
          custom_style->SetRenderer(_vtkRenderer);

          vtkSmartPointer<vtkPropPicker> picker = vtkSmartPointer<vtkPropPicker>::New();
          custom_style->SetvtkPropPicker(picker);
          interactor->SetInteractorStyle(custom_style);
        }
      });

    // modeChanged
  }

  {
    auto slice_panel = new SlicePanel(_vtkRenderWidget);
    slice_panel->resize(600, 150);
    slice_panel->move(300, 0);
  }

  // 自定义交互
  {
    auto interactor = _vtkRenderWindow->GetInteractor();

    interactor->RemoveAllObservers();

    auto custom_style = vtkSmartPointer<RSInteractorTrackCamera>::New();

    custom_style->SetRenderer(_vtkRenderer);

    interactor->SetInteractorStyle(custom_style);

    _vtkRenderWidget->installEventFilter(this);
  }

  { // 右键 context 菜单
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
}

void MainWindow::exit()
{
  qDebug() << "exiting...";

  QApplication::exit();
}
