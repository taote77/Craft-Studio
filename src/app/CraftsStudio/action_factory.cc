#include "action_factory.h"
#include "engine/rs_scene_manager.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QTemporaryFile>
#include <qapplication.h>
#include <qdebug.h>
#include <qfileinfo.h>

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
#include <vtkImageAppend.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMarchingCubes.h>
#include <vtkMemoryResourceStream.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPNGReader.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkQuadricDecimation.h>
#include <vtkRendererCollection.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
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
  mapper->SetInputConnection(transform_filter->GetOutputPort());

  auto actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  auto manager = SceneManager::getInstance();

  QFileInfo finfo(path);
  manager->addObject(new SceneObject(finfo.fileName(), actor));

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();
  if (!renderer)
  {
    return;
  }

  renderer->AddActor(actor);

  auto camera = renderer->GetActiveCamera();
  camera->SetFocalPoint(0, 0, 0); // # 焦点置于原点
  camera->SetPosition(0, 0, 500); // # 调整摄像机位置（避免模型过近）
  camera->SetViewUp(0, 1, 0);     //  # 设置垂直方向

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

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();
  if (!renderer)
  {
    return;
  }

  renderer->AddActor(actor);

  _vtkRenderWindow->Render();
}

void ActionFactory::openGeneralModelFile()
{
  //
}

void ActionFactory::onConstructionFile()
{
  QString dir = "/home/heygears/workstation/MasterWay/build/target/bin/model_img/";

  const int numSlices = 30;           // 切片总数
  const double sliceThickness = 0.01; // 层厚0.01mm

  // ========== 1. 读取并拼接切片 ==========
  vtkSmartPointer<vtkImageAppend> appendFilter = vtkSmartPointer<vtkImageAppend>::New();
  appendFilter->SetAppendAxis(2); // 沿Z轴拼接

  for (int i = 1; i <= numSlices; ++i)
  {
    // 生成标准化文件名 (S000001_P1.png 格式)
    std::ostringstream filenameStream;
    filenameStream << "S" << std::setfill('0') << std::setw(6) << i << "_P1.png";
    std::string filename = dir.toStdString() + filenameStream.str();

    // 读取PNG图像
    vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    qDebug() << "filename: " << filename.c_str();

    // 添加到拼接器
    appendFilter->AddInputData(reader->GetOutput());
  }

  QApplication::processEvents();

  appendFilter->Update();

  // ========== 2. 设置三维体数据结构 ==========
  vtkSmartPointer<vtkImageData> volumeData = appendFilter->GetOutput();
  volumeData->SetSpacing(1.0, 1.0, sliceThickness); // Z轴层厚0.01mm [1,4](@ref)

  // ========== 3. 三维重建（等值面提取） ==========
  vtkSmartPointer<vtkMarchingCubes> mcFilter = vtkSmartPointer<vtkMarchingCubes>::New();
  mcFilter->SetInputData(volumeData);
  mcFilter->SetValue(0, 128); // 灰度阈值（根据实际图像调整）
  mcFilter->ComputeNormalsOn();
  mcFilter->Update();

  // ========== 4. 法向量优化 ==========
  vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
  normalsFilter->SetInputConnection(mcFilter->GetOutputPort());
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->Update();

  // 简化网格（缩减70%面片）
  // auto decimator = vtkSmartPointer<vtkQuadricDecimation>::New();
  // decimator->SetInputConnection(normalsFilter->GetOutputPort());
  // decimator->SetTargetReduction(0.7);  // 缩减比例（0.7=70%）
  // decimator->AttributeErrorMetricOn(); // 保留属性特征
  // decimator->Update();

  // ========== 创建可视化管线 ==========

  // ========== 5. 导出为单一STL文件 ==========
  vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
  stlWriter->SetFileName("merged_model.stl");
  stlWriter->SetInputConnection(normalsFilter->GetOutputPort());
  stlWriter->SetFileTypeToBinary(); // 二进制格式减小文件体积
  stlWriter->Write();

  vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
  reader->SetFileName("merged_model.stl"); // 替换为你的STL文件路径
  reader->Update();

  // ========== 6. 可选：可视化模型（验证结果） ==========
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(reader->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // 演员
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.9, 0.7, 0.6); // 设置模型颜色（肉色）

  auto renderer = _vtkRenderWindow->GetRenderers()->GetFirstRenderer();
  if (!renderer)
  {
    return;
  }

  renderer->AddActor(actor);

  auto camera = renderer->GetActiveCamera();
  camera->SetFocalPoint(0, 0, 0); // # 焦点置于原点
  camera->SetPosition(0, 0, 500); // # 调整摄像机位置（避免模型过近）
  camera->SetViewUp(0, 1, 0);     //  # 设置垂直方向

  _vtkRenderWindow->Render();
}

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
