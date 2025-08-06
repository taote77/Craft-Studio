#include "action_handler.h"
#include <algorithm>

#include <QDebug>
#include <QFileDialog>
#include <QObject>

#include <qapplication.h>

#include <vtkAbstractMapper.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCutter.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageExport.h>
#include <vtkImageMapper.h>
#include <vtkImageReader2.h>
#include <vtkImageReslice.h>
#include <vtkImageThreshold.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWindowToImageFilter.h>

namespace GUI
{

ActionHandler::ActionHandler(QObject* parent)
  : QObject(parent)
{
}

void ActionHandler::setVtkRenderer(vtkSmartPointer<vtkRenderer> vtk_renderer,
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> render_window)
{
  _vtk_renderer = vtk_renderer;
  _window = render_window;
}

void ActionHandler::slotAddPlane() {}

void ActionHandler::slotAddCone()
{
  vtkNew<vtkConeSource> cone;

  cone->SetHeight(70);
  cone->SetRadius(50);
  cone->SetResolution(8);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(cone->GetOutputPort());

  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(mapper);

  // 设置立方体的颜色为紫色
  vtkNew<vtkNamedColors> colors;
  coneActor->GetProperty()->SetColor(colors->GetColor3d("Purple").GetData());

  _vtk_renderer->AddActor(coneActor);

  _window->Render();

  _vtk_renderer->ResetCamera();
}

void ActionHandler::slotAddCylinder()
{
  auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();

  cylinder->SetHeight(60);
  cylinder->SetRadius(50);

  cylinder->Update();
  auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();

  mapper->SetInputConnection(cylinder->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.0, 0.0, 1.0);

  _vtk_renderer->AddActor(actor);

  _window->Render();

  _vtk_renderer->ResetCamera();
}

void ActionHandler::slotFileNewTemplate() {}

void ActionHandler::slotFileOpen() {}

void ActionHandler::slotFileSaveAs() {}

void ActionHandler::slotImageProcess(const QString& path)
{
  auto reader = vtkSmartPointer<vtkPNGReader>::New();
  reader->SetFileName(path.toStdString().c_str());

  // 获取图像尺寸
  int* dims = reader->GetOutput()->GetDimensions();
  int width = dims[0];
  int height = dims[1];

  // // threshold
  // vtkSmartPointer<vtkImageThreshold> vtk_threshold = vtkSmartPointer<vtkImageThreshold>::New();
  // vtk_threshold->SetInputConnection(0, reader->GetOutputPort());
  // vtk_threshold->ThresholdByLower(128); // 设定阈值，128 通常是灰度图像的中间值
  // vtk_threshold->ReplaceInOn();
  // vtk_threshold->SetInValue(1);
  // vtk_threshold->ReplaceOutOn();
  // vtk_threshold->SetOutValue(0);
  // vtk_threshold->Update();

  // vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  // imageData->DeepCopy(vtk_threshold->GetOutput());

  // // 扩展 Z 维度
  // int *dims = imageData->GetDimensions();
  // imageData->SetDimensions(dims[0], dims[1], 10);   // 假设沿 Z 轴有 10 层
  // imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1); // 设置数据类型
  // for (int z = 0; z < 10; ++z)
  // {
  //     for (int y = 0; y < dims[1]; ++y)
  //     {
  //         for (int x = 0; x < dims[0]; ++x)
  //         {
  //             auto *pixel = static_cast<unsigned char *>(imageData->GetScalarPointer(x, y, z));
  //             auto *origPixel = static_cast<unsigned char
  //             *>(vtk_threshold->GetOutput()->GetScalarPointer(x, y, 0)); *pixel = *origPixel; //
  //             复制像素值
  //         }
  //     }
  // }

  // // 使用 Marching Cubes 生成三维模型
  // vtkSmartPointer<vtkMarchingCubes> marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
  // marchingCubes->SetInputData(imageData);
  // marchingCubes->SetValue(0, 0.5); // 选择等值面
  // marchingCubes->Update();

  // // 拉伸模型
  // vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  // transform->Scale(1.0, 1.0, 4); // 沿 Z 轴拉伸 4 mm

  // vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
  // vtkSmartPointer<vtkTransformPolyDataFilter>::New(); transformFilter->SetTransform(transform);
  // transformFilter->SetInputConnection(marchingCubes->GetOutputPort());
  // transformFilter->Update();

  // auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  // mapper->SetInputConnection(transformFilter->GetOutputPort());

  // auto actor = vtkSmartPointer<vtkActor>::New();
  // actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // RGB值，红色

  // actor->SetMapper(mapper);

  vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
  mapper->SetInputConnection(reader->GetOutputPort());
  mapper->SetColorLevel(127.5);
  mapper->SetColorWindow(255);
  vtkSmartPointer<vtkActor2D> imgActor = vtkSmartPointer<vtkActor2D>::New();
  imgActor->SetMapper(mapper);

  _vtk_renderer->AddActor2D(imgActor);
  _vtk_renderer->SetBackground(0.0, 0.0, 0.0);

  vtkNew<vtkPNGWriter> writer;
  writer->SetFileName("/home/shane/z/craftsstudio/resources/test__1.png");
  writer->SetInputConnection(reader->GetOutputPort());

  // 写入图像
  writer->Write();

  _window->SetSize(width, height);
  _window->Render();

  _vtk_renderer->ResetCamera();
}

void ActionHandler::slotSliceSTL(const QString& stl_file)
{
  // 读取 STL 文件
  auto stlReader = vtkSmartPointer<vtkSTLReader>::New();
  stlReader->SetFileName(stl_file.toStdString().c_str());
  stlReader->Update();

  // 获取 STL 文件的边界框
  double bounds[6];
  stlReader->GetOutput()->GetBounds(bounds);

  // 切片间隔（单位：mm）
  double sliceInterval = 2.0;

  // 计算切片数量
  int numSlices = static_cast<int>((bounds[5] - bounds[4]) / sliceInterval) + 1;

  // 设置切片平面
  for (int i = 0; i < numSlices; ++i)
  {
    double zValue = bounds[4] + i * sliceInterval;

    // 创建切片平面
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    plane->SetNormal(0, 0, 1); // Z 轴切片
    plane->SetOrigin(0, 0, zValue);

    // 创建切割器
    vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
    cutter->SetInputConnection(stlReader->GetOutputPort());
    cutter->SetCutFunction(plane);
    cutter->Update();

    // 创建映射器
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cutter->GetOutputPort());

    // 创建演员
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // RGB值，红色

    // 添加演员到渲染窗口
    _vtk_renderer->AddActor(actor);
    _vtk_renderer->ResetCamera(); // auto set camera viewport

    // 渲染窗口
    _window->Render();

    // 创建窗口到图像过滤器
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
      vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(_window);
    windowToImageFilter->Update();

    // 保存为 PNG 文件

    std::string pngFileName =
      QApplication::applicationFilePath().toStdString() + "slice_" + std::to_string(i) + ".png";
    vtkSmartPointer<vtkPNGWriter> pngWriter = vtkSmartPointer<vtkPNGWriter>::New();
    pngWriter->SetFileName(pngFileName.c_str());
    pngWriter->SetInputConnection(windowToImageFilter->GetOutputPort());
    pngWriter->Write();
  }
}

} // namespace GUI