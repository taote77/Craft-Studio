#include "action_factory.h"
#include "engine/rs_scene_manager.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTemporaryFile>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCenterOfMass.h>
#include <vtkCleanPolyData.h>
#include <vtkConeSource.h>
#include <vtkContourFilter.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageAppend.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkMarchingCubes.h>
#include <vtkMemoryResourceStream.h>
#include <vtkNamedColors.h>
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
#include <vtkTriangleFilter.h>

ActionFactory::ActionFactory(QObject* parent)
  : QObject(parent)
{
}

void ActionFactory::setVtkRenderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow> vtkRenderWindow)
{
  _vtkRenderWindow = vtkRenderWindow;
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

  _vtkRenderWindow->Render();

  // if (_vtkRenderWindow)
  // {
  //   _vtkRenderWindow->AddRenderer(renderer);
  // }
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

  _vtkRenderWindow->Render();

  // if (_vtkRenderWindow)
  // {
  //   _vtkRenderWindow->AddRenderer(renderer);
  // }
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

  _vtkRenderWindow->Render();

  // if (_vtkRenderWindow)
  // {
  //   _vtkRenderWindow->AddRenderer(renderer);
  // }
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

  // if (_vtkRenderWindow)
  // {
  //   _vtkRenderWindow->AddRenderer(renderer);

  // update
  _vtkRenderWindow->Render();
  // }
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

  _vtkRenderWindow->Render();

  // if (_vtkRenderWindow)
  // {
  //   _vtkRenderWindow->AddRenderer(renderer);
  // }
}

void ActionFactory::openSTLFile()
{

  // select stl file
  const QString& path =
    QFileDialog::getOpenFileName(nullptr, "Open STL File", QApplication::applicationDirPath(), "STL Files (*.stl)");

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
  const QString& path =
    QFileDialog::getOpenFileName(nullptr, "Open OBJ File", QApplication::applicationDirPath(), "OBJ Files (*.obj)");
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

  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
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
  const QString& dir = QFileDialog::getExistingDirectory(nullptr, "", QApplication::applicationDirPath());
  if (dir.isEmpty())
  {
    return;
  }

  // 计算像素尺寸
  double build_platform_width = 192.0;
  double build_platform_height = 108.0;

  double pixel_x = 1920.0;
  double pixel_y = 1080.0;
  double pixelSizeX = build_platform_width / pixel_x;  // 192mm / 1920像素 = 0.1mm/像素
  double pixelSizeY = build_platform_height / pixel_y; // 108mm / 1080像素 = 0.1mm/像素
                                                       // 50    / 500  =
  double sliceThickness = 0.5;                         // 每层厚度0.5mm

  const int numSlices = 5; // 切片总数

  // ========== 1. 读取并拼接切片 ==========
  auto appendFilter = vtkSmartPointer<vtkImageAppend>::New();
  appendFilter->SetAppendAxis(2); // 沿Z轴拼接

  for (int i = 1; i <= numSlices; ++i)
  {
    // 生成标准化文件名 (S000001_P1.png 格式)
    std::ostringstream filenameStream;
    filenameStream << "S" << std::setfill('0') << std::setw(6) << i << "_P1.png";
    std::string filename = dir.toStdString() + "/" + filenameStream.str();

    qDebug() << filename.c_str();
    // 读取PNG图像
    auto reader = vtkSmartPointer<vtkPNGReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    appendFilter->AddInputData(reader->GetOutput());
  }

  appendFilter->Update();

  // ========== 2. 设置三维体数据结构 ==========
  vtkSmartPointer<vtkImageData> volumeData = appendFilter->GetOutput();
  double spacing[3] = { pixelSizeX, pixelSizeY, sliceThickness };
  volumeData->SetSpacing(spacing);

  // ========== 3. 三维重建（等值面提取） ==========
  auto mcFilter = vtkSmartPointer<vtkMarchingCubes>::New();
  mcFilter->SetInputData(volumeData);
  mcFilter->SetValue(0, 128); // 灰度阈值（根据实际图像调整）
  mcFilter->ComputeNormalsOn();
  mcFilter->Update();

  // ========== 4. 法向量优化 ==========
  auto normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
  normalsFilter->SetInputConnection(mcFilter->GetOutputPort());
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->Update();

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

// contourBasedReconstruction
void ActionFactory::onContourConstruct()
{
  const QString& dir = QFileDialog::getExistingDirectory(nullptr, "", QApplication::applicationDirPath());
  if (dir.isEmpty())
  {
    return;
  }

  // 计算像素尺寸
  double build_platform_width = 192.0;
  double build_platform_height = 108.0;

  double pixel_x = 1920.0;
  double pixel_y = 1080.0;
  double pixelSizeX = build_platform_width / pixel_x;  // 192mm / 1920像素 = 0.1mm/像素
  double pixelSizeY = build_platform_height / pixel_y; // 108mm / 1080像素 = 0.1mm/像素
                                                       // 50    / 500  =
  double sliceThickness = 0.5;                         // 每层厚度0.5mm

  const int numSlices = 5; // 切片总数

  double zPos = 0.0;
  double zSpacing = 0.5; // 层间间距

  //
  auto appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

  for (int i = 1; i <= numSlices; ++i)
  {
    std::ostringstream filenameStream; // 生成标准化文件名 (S000001_P1.png 格式)
    filenameStream << "S" << std::setfill('0') << std::setw(6) << i << "_P1.png";
    std::string filename = dir.toStdString() + "/" + filenameStream.str();
    qDebug() << filename.c_str();
    // 读取PNG图像
    // 读取单张切片
    auto sliceReader = vtkSmartPointer<vtkPNGReader>::New();
    sliceReader->SetFileName(filename.c_str());
    sliceReader->Update();

    // 添加高斯平滑减少噪声[5,8](@ref)
    auto smoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    smoother->SetInputConnection(sliceReader->GetOutputPort());
    smoother->SetStandardDeviations(1.5, 1.5);
    smoother->Update();

    // 提取轮廓
    auto contourFilter = vtkSmartPointer<vtkContourFilter>::New();
    contourFilter->SetInputConnection(smoother->GetOutputPort());
    contourFilter->SetValue(0, 128); // 阈值
    contourFilter->Update();

    // 检查是否提取到轮廓
    if (contourFilter->GetOutput()->GetNumberOfCells() == 0)
    {
      qWarning() << "未从文件中提取到轮廓:" << filename.c_str();
      continue;
    }

    // 将轮廓平移到正确的Z位置
    auto transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(0, 0, zPos);

    transform->Scale(pixelSizeX, pixelSizeY, 1.0);
    // 再平移（居中并设置Z位置）
    transform->Translate(-build_platform_width / 2, // X方向居中
      -build_platform_height / 2,                   // Y方向居中
      zPos                                          // 当前层的Z位置
    );

    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(contourFilter->GetOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    // 添加到组合器
    appendFilter->AddInputConnection(transformFilter->GetOutputPort());

    zPos += zSpacing;
  }

  // 5. 检查是否有有效数据
  if (appendFilter->GetNumberOfInputConnections(0) == 0)
  {
    QMessageBox::critical(nullptr, "错误", "未成功处理任何切片，无法生成模型");
    return;
  }

  // 合并所有轮廓
  appendFilter->Update();

  // 清理重复点和冗余数据

  auto cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  // 三角化（STL格式要求）

  auto triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New(); // STL必须三角化[7](@ref)
  triangleFilter->SetInputConnection(cleanFilter->GetOutputPort());
  triangleFilter->Update();

  // 法向量优化 ==========
  auto normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
  normalsFilter->SetInputConnection(triangleFilter->GetOutputPort());
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->Update();

  auto stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
  stlWriter->SetFileName("contoure_model.stl");
  stlWriter->SetInputConnection(normalsFilter->GetOutputPort());
  stlWriter->SetFileTypeToBinary(); // 二进制格式减小文件体积
  stlWriter->Write();

  auto reader = vtkSmartPointer<vtkSTLReader>::New();
  reader->SetFileName("contoure_model.stl"); // 替换为你的STL文件路径
  reader->Update();

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(reader->GetOutputPort());
  // mapper->ScalarVisibilityOff();

  // 演员
  auto actor = vtkSmartPointer<vtkActor>::New();
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
