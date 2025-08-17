#include "rs_interactor_trackball_camera.h"

#include <QDebug>
#include <iostream>
#include <qdebug.h>
#include <qglobal.h>

#include <qnumeric.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

vtkStandardNewMacro(RSInteractorTrackCamera);

void RSInteractorTrackCamera::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  _renderer = renderer;
}

void RSInteractorTrackCamera::SetvtkPropPicker(vtkSmartPointer<vtkPropPicker> picker)
{
  _picker = picker;
}

void RSInteractorTrackCamera::OnLeftButtonDown()
{
  // 记录初始位置
  m_startPos[0] = this->Interactor->GetEventPosition()[0];
  m_startPos[1] = this->Interactor->GetEventPosition()[1];

  // 获取当前平面变换矩阵
  m_initialMatrix = m_planeMatrix;

  // 标记操作类型
  m_isDragging = true;

  // 调用基类处理
  Superclass::OnLeftButtonDown();
}

void RSInteractorTrackCamera::OnMouseMove()
{

  if (!m_isDragging)
  {
    // qDebug() << "not drag";
    return;
  }

  int* pos = this->Interactor->GetEventPosition();

  m_endPos[0] = pos[0];
  m_endPos[1] = pos[1];

  double dx = pos[0] - m_startPos[0];
  double dy = pos[1] - m_startPos[1];

  // 获取当前渲染器和相机
  auto renderer = this->Interactor->FindPokedRenderer(m_startPos[0], m_startPos[1]);
  if (!renderer)
    return;

  auto camera = renderer->GetActiveCamera();
  double* focalPoint = camera->GetFocalPoint();
  double* position = camera->GetPosition();

  // 计算视口尺寸
  int* size = renderer->GetSize();
  double viewWidth = size[0];
  double viewHeight = size[1];

  // 根据VTK标准实现旋转逻辑[1,4](@ref)
  if (this->Interactor->GetControlKey()) // Ctrl键触发旋转
  {
    // qDebug() << "Ctrl+mouse move:" << dx << " " << dy;

    // 计算旋转角度（根据视口比例调整灵敏度）
    constexpr double rotate_scale = 0.01; // 平移灵敏度

    double deltaAzimuth = -dx * rotate_scale / viewWidth * 360.0;    // 水平旋转
    double deltaElevation = -dy * rotate_scale / viewHeight * 360.0; // 垂直旋转

    // 执行旋转操作
    camera->Azimuth(deltaAzimuth);
    camera->Elevation(deltaElevation);

    // 保持视图向上方向正交化
    camera->OrthogonalizeViewUp();
  }
  else // 默认平移操作
  {
    // qDebug() << "mouse move:" << dx << " " << dy;

    // 平移参数设置
    constexpr double scale = 0.01; // 平移灵敏度
    double translateX = dx * scale;
    double translateY = -dy * scale; // Y轴方向相反

    // 计算平移向量
    double right[3], viewUp[3];
    vtkMath::Cross(camera->GetViewPlaneNormal(), camera->GetViewUp(), right);
    vtkMath::Normalize(right);

    // 更新相机位置和焦点
    double newPosition[3], newFocalPoint[3];
    for (int i = 0; i < 3; i++)
    {
      newPosition[i] = position[i] + translateX * right[i] + translateY * camera->GetViewUp()[i];
      newFocalPoint[i] = focalPoint[i] + translateX * right[i] + translateY * camera->GetViewUp()[i];
    }

    camera->SetPosition(newPosition);
    camera->SetFocalPoint(newFocalPoint);
  }

  renderer->ResetCameraClippingRange(); // 更新视图参数
  this->Interactor->Render();
}

void RSInteractorTrackCamera::OnLeftButtonUp()
{

  m_isDragging = false;
  Superclass::OnLeftButtonUp();
}

void RSInteractorTrackCamera::OnMouseWheelForward()
{
  this->Superclass::OnMouseWheelForward();
}

void RSInteractorTrackCamera::OnMouseWheelBackward()
{
  this->Superclass::OnMouseWheelBackward();
}

void RSInteractorTrackCamera::OnKeyPress()
{
  auto interactor = this->GetInteractor();
  char key = interactor->GetKeyCode();

  qDebug() << "OnKeyPress key = " << key;

  auto renderer = this->Interactor->FindPokedRenderer(m_startPos[0], m_startPos[1]);
  if (!renderer)
  {
    return;
  }

  switch (key)
  {
    case 'w': // 前移视图
      renderer->GetActiveCamera()->Dolly(1.1);
      break;
    case 's': // 后移视图
      renderer->GetActiveCamera()->Dolly(0.9);
      break;
    case 'r': // 重置视图
      renderer->ResetCamera();
      break;
    default:
      break;
  }

  renderer->ResetCameraClippingRange(); // 更新视图参数
  this->Interactor->Render();

  // this->Superclass::OnKeyPress();
}

RSInteractorTrackCamera::RSInteractorTrackCamera() {}

RSInteractorTrackCamera::~RSInteractorTrackCamera()
{
  qDebug() << "RSInteractorTrackCamera::~RSInteractorTrackCamera()";
}

void RSInteractorTrackCamera::TranslatePlane(double dx, double dy)
{
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

  matrix->DeepCopy(m_initialMatrix);

  // 根据视口比例计算平移量
  vtkSmartPointer<vtkRenderWindow> renWin = this->Interactor->GetRenderWindow();

  double scale = renWin->GetSize()[0] / 1000.0;
  matrix->SetElement(0, 3, m_initialMatrix->GetElement(0, 3) + dx * scale);
  matrix->SetElement(1, 3, m_initialMatrix->GetElement(1, 3) + dy * scale);

  m_planeMatrix = matrix;
}

// 平面旋转
void RSInteractorTrackCamera::RotatePlane(double dx, double dy)
{
  vtkSmartPointer<vtkMatrix4x4> rotation = vtkSmartPointer<vtkMatrix4x4>::New();

  // 计算旋转角度（根据视口比例）
  vtkSmartPointer<vtkRenderWindow> renWin = this->Interactor->GetRenderWindow();
  double scale = renWin->GetSize()[0] / 1000.0;

  double angleX = -dy * scale * 0.5; // Y轴旋转
  double angleY = dx * scale * 0.5;  // X轴旋转

  // 构建旋转矩阵
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

  transform->RotateX(angleX);
  transform->RotateY(angleY);

  transform->GetMatrix(rotation);

  // 组合变换矩阵
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

  vtkMatrix4x4::Multiply4x4(m_initialMatrix, rotation, matrix);

  m_planeMatrix = matrix;
}
