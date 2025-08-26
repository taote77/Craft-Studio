#include "engine/rs_interactor_trackball_actor.h"
#include "engine/rs_interactor_trackball_camera.h"

#include "ui/model_viewer.h"
#include "ui/slice_panel.h"
#include "ui/transform_panel.h"

#include <QDebug>

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSliderRepresentation2D.h>

vtkSmartPointer<vtkPolyData> CreateGrid(double size, int divisions) {
  auto grid = vtkSmartPointer<vtkPolyData>::New();
  auto points = vtkSmartPointer<vtkPoints>::New();
  auto lines = vtkSmartPointer<vtkCellArray>::New();

  double step = size / divisions;
  double halfSize = size / 2.0;

  // 创建水平线
  for (int i = 0; i <= divisions; ++i) {
    double y = -halfSize + i * step;
    points->InsertNextPoint(-halfSize, y, 0.0);
    points->InsertNextPoint(halfSize, y, 0.0);

    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetId(0, 2 * i);
    line->GetPointIds()->SetId(1, 2 * i + 1);
    lines->InsertNextCell(line);
  }

  // 创建垂直线
  int offset = 2 * (divisions + 1);
  for (int i = 0; i <= divisions; ++i) {
    double x = -halfSize + i * step;
    points->InsertNextPoint(x, -halfSize, 0.0);
    points->InsertNextPoint(x, halfSize, 0.0);

    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetId(0, offset + 2 * i);
    line->GetPointIds()->SetId(1, offset + 2 * i + 1);
    lines->InsertNextCell(line);
  }

  grid->SetPoints(points);
  grid->SetLines(lines);

  return grid;
}

ModelViewer::ModelViewer(QWidget *parent, Qt::WindowFlags flag)
    : QVTKOpenGLNativeWidget(parent, flag) {

  setUp();
}

vtkSmartPointer<vtkGenericOpenGLRenderWindow> ModelViewer::getRenderWindow() {
  return _render_window;
}

vtkSmartPointer<vtkRenderer> ModelViewer::getVtkRenderer() {
  return _vtk_renderer;
}

void ModelViewer::setUp() {
  _render_window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

  _render_window->SetMultiSamples(0);
  _render_window->SetAlphaBitPlanes(1);

  this->setRenderWindow(_render_window);

  _vtk_renderer = vtkSmartPointer<vtkRenderer>::New();
  _vtk_renderer->SetBackground(1.0, 1.0, 1.0); // 白色背景

  _render_window->AddRenderer(_vtk_renderer);

  { // grid
    auto colors = vtkSmartPointer<vtkNamedColors>::New();

    auto grid = CreateGrid(10.0, 10);
    auto grid_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    grid_mapper->SetInputData(grid);

    auto grid_actor = vtkSmartPointer<vtkActor>::New();
    grid_actor->SetMapper(grid_mapper);
    grid_actor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());
    grid_actor->GetProperty()->SetLineWidth(1);

    // 创建坐标轴
    auto axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetTotalLength(1, 1, 1);
    // 创建坐标轴标记窗口部件
    _axes_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

    _axes_widget->SetOrientationMarker(axes);

    _vtk_renderer->AddActor(grid_actor);
  }

  _cam_widget = vtkSmartPointer<vtkCameraOrientationWidget>::New();

  {
    auto transform_panel = new TransformPanel(this);
    transform_panel->move(10, 50);
    transform_panel->resize(150, 600);

    connect(transform_panel, &TransformPanel::modeChanged, this,
            [this](EditMode mode) {
              qDebug() << "当前模式:" << (int)mode;

              auto interactor = _render_window->GetInteractor();
              interactor->RemoveAllObservers();

              if (mode == EditMode::None) {
                auto custom_style =
                    vtkSmartPointer<RSInteractorTrackCamera>::New();
                custom_style->SetRenderer(_vtk_renderer);
                interactor->SetInteractorStyle(custom_style);

              } else {
                auto custom_style =
                    vtkSmartPointer<RSInteractorTrackActor>::New();
                custom_style->SetRenderer(_vtk_renderer);

                auto picker = vtkSmartPointer<vtkPropPicker>::New();
                custom_style->SetvtkPropPicker(picker);
                interactor->SetInteractorStyle(custom_style);
              }

              _axes_widget->SetInteractor(interactor);

              _axes_widget->SetEnabled(0);
              _axes_widget->InteractiveOff();

              _axes_widget->SetEnabled(1);
              _axes_widget->InteractiveOn();

              _cam_widget->Off();
              _cam_widget->On();
            });
  }

  // 自定义交互
  {
    auto interactor = _render_window->GetInteractor();

    interactor->RemoveAllObservers();

    auto custom_style = vtkSmartPointer<RSInteractorTrackCamera>::New();
    custom_style->SetRenderer(_vtk_renderer);
    _axes_widget->SetInteractor(interactor);
    _axes_widget->SetEnabled(1);
    _axes_widget->InteractiveOn();
    interactor->SetInteractorStyle(custom_style);

    _cam_widget->SetParentRenderer(_vtk_renderer);
    _cam_widget->On();

    this->installEventFilter(this);
  }
}
