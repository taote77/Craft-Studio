#include "actionfactory.h"

#include <QDebug>
#include <QFileDialog>
#include <QTemporaryFile>

#include <qapplication.h>
#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkCamera.h>
#include <vtkCenterOfMass.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMemoryResourceStream.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStringArray.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

ActionFactory::ActionFactory(QObject* parent)
  : QObject(parent)
{
}

void ActionFactory::exitApp()
{
  Q_EMIT sigExit();
}

void ActionFactory::addSphere()
{
  auto sphere = vtkNew<vtkSphereSource>();
  sphere->SetCenter(0, 0, 0);
  sphere->SetRadius(0.5);

  auto mapper = vtkNew<vtkDataSetMapper>();
  mapper->SetInputConnection(sphere->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.0, 1.0, 0.0); // Green color
  actor->GetProperty()->SetRepresentationToSurface();

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();

  renderer->AddActor(actor);

  if (_vtkRenderWindow)
  {
    _vtkRenderWindow->AddRenderer(renderer);
  }
}

void ActionFactory::addCylinder()
{
  auto cylinder = vtkNew<vtkCylinderSource>();
  cylinder->SetCenter(0, 0, 0);
  cylinder->SetRadius(0.5);
  cylinder->SetHeight(1);

  auto mapper = vtkNew<vtkDataSetMapper>();
  mapper->SetInputConnection(cylinder->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.0, 0.0, 1.0); // Blue color
  actor->GetProperty()->SetRepresentationToSurface();

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();

  renderer->AddActor(actor);

  if (_vtkRenderWindow)
  {
    _vtkRenderWindow->AddRenderer(renderer);
  }
}

void ActionFactory::addCube()
{
  // 实现添加立方体模型的逻辑

  auto cube = vtkNew<vtkCubeSource>();
  cube->SetCenter(0, 0, 0);
  cube->SetXLength(0.5);
  cube->SetYLength(0.5);
  cube->SetZLength(0.5);

  auto mapper = vtkNew<vtkDataSetMapper>();
  mapper->SetInputConnection(cube->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  // actor->GetProperty()->SetEdgeVisibility(true);
  actor->GetProperty()->SetRepresentationToSurface();

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();

  renderer->AddActor(actor);

  if (_vtkRenderWindow)
  {
    _vtkRenderWindow->AddRenderer(renderer);
  }
}

void ActionFactory::addPlane()
{

  auto plane = vtkNew<vtkPlaneSource>();
  plane->SetCenter(0, 0, 0);
  plane->SetNormal(0, 0, 1);
  plane->SetXResolution(0.1);
  plane->SetYResolution(0.2);

  auto mapper = vtkNew<vtkDataSetMapper>();
  mapper->SetInputConnection(plane->GetOutputPort());

  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  actor->GetProperty()->SetRepresentationToSurface();

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();
  renderer->AddActor(actor);

  if (_vtkRenderWindow)
  {
    _vtkRenderWindow->AddRenderer(renderer);

    // update
    _vtkRenderWindow->Render();
  }
}

void ActionFactory::addCone()
{
  auto cone = vtkNew<vtkConeSource>();
  cone->SetCenter(0, 0, 0);
  cone->SetRadius(0.5);
  cone->SetHeight(0.6);

  auto mapper = vtkNew<vtkDataSetMapper>();
  mapper->SetInputConnection(cone->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.0, 0.0, 1.0);
  actor->GetProperty()->SetRepresentationToSurface();

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();

  renderer->AddActor(actor);

  if (_vtkRenderWindow)
  {
    _vtkRenderWindow->AddRenderer(renderer);
  }
}

void ActionFactory::openSTLFile()
{

  // select stl file
  const QString& path = QFileDialog::getOpenFileName(
    nullptr, "Open STL File", QApplication::applicationDirPath(), "STL Files (*.stl)");
  if (path.isEmpty())
  {
    return;
  }

  auto reader = vtkSmartPointer<vtkSTLReader>::New();
  reader->SetFileName(path.toStdString().c_str());
  reader->Update();

  auto center_calculator = vtkSmartPointer<vtkCenterOfMass>::New();
  center_calculator->SetInputData(reader->GetOutput());
  center_calculator->SetUseScalarsAsWeights(false); //  # 几何中心（非质量加权）
  center_calculator->Update();
  auto model_center = center_calculator->GetCenter(); //  # 获取质心坐标 [x,y,z]

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->Translate(-model_center[0], -model_center[1], -model_center[2]); //  # 反向平移

  auto transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transform_filter->SetInputConnection(reader->GetOutputPort());
  transform_filter->SetTransform(transform);
  transform_filter->Update();

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //  mapper->SetInputConnection(reader->GetOutputPort());
  mapper->SetInputConnection(transform_filter->GetOutputPort());

  auto actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  _vtkRenderer->AddActor(actor);

  auto camera = _vtkRenderer->GetActiveCamera();
  camera->SetFocalPoint(0, 0, 0); // # 焦点置于原点
  camera->SetPosition(0, 0, 500); // # 调整摄像机位置（避免模型过近）
  camera->SetViewUp(0, 1, 0);     //  # 设置垂直方向

  // render
  // _vtkRenderer->Render();

  _vtkRenderWindow->Render();
}

void ActionFactory::openOBJFile()
{

  // 1. select OBJ 文件
  const QString& path = QFileDialog::getOpenFileName(
    nullptr, "Open OBJ File", QApplication::applicationDirPath(), "OBJ Files (*.obj)");
  if (path.isEmpty())
  {
    return;
  }

  vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
  reader->SetFileName(path.toStdString().c_str());
  reader->Update();

  // 2. 可选：模型变换（旋转、缩放等）
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(180, 0, 0, 1); // 绕 Z 轴旋转 180 度
  transform->Scale(0.5, 0.5, 0.5);     // 缩放 50%

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetTransform(transform);
  transformFilter->SetInputConnection(reader->GetOutputPort());
  transformFilter->Update();

  // 3. 构建渲染管线
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(transformFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // 设置红色
  actor->GetProperty()->SetOpacity(0.9);         // 设置透明度

  _vtkRenderer->AddActor(actor);

  _vtkRenderWindow->Render();
}

void ActionFactory::openGeneralModelFile() {}

void ActionFactory::clearScene()
{

  // 获取第一个渲染器
  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();

  // 遍历并移除所有演员
  vtkCollectionSimpleIterator pit;
  vtkActor* actor;
  renderer->GetActors()->InitTraversal(pit);
  while ((actor = renderer->GetActors()->GetNextActor(pit)))
  {
    renderer->RemoveActor(actor);
  }

  // 触发重新渲染
  _vtkRenderWindow->Render();
}
