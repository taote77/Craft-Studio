#include "rs_interactor_style.h"

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

vtkStandardNewMacro(RSInteractorStyle);

void RSInteractorStyle::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  _renderer = renderer;
}

void RSInteractorStyle::SetvtkPropPicker(vtkSmartPointer<vtkPropPicker> picker)
{
  _picker = picker;
}

void RSInteractorStyle::OnLeftButtonDown()
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

void RSInteractorStyle::OnMouseMove()
{
  if (!m_isDragging)
  {
    qDebug() << "not drag";
    return;
  }
  int* pos = this->Interactor->GetEventPosition();

  m_endPos[0] = pos[0];
  m_endPos[1] = pos[1];

  double dx = pos[0] - m_startPos[0];
  double dy = pos[1] - m_startPos[1];

  // 检测Ctrl键状态
  bool ctrlPressed = this->Interactor->GetShiftKey() || this->Interactor->GetControlKey();

  if (ctrlPressed)
  {
    // qDebug() << "Ctrl+mouse move:" << dx << " " << dy;
    // 旋转操作
    // RotatePlane(dx, dy);
  }
  else
  {
    // qDebug() << "mouse move:" << dx << " " << dy;
    // 平移操作
    // TranslatePlane(dx, dy);
  }

  auto renderer = this->Interactor->FindPokedRenderer(m_startPos[0], m_startPos[1]);
  if (renderer)
  {

    qDebug() << "mouse move dddddddddddd:" << dx << " " << dy;
    auto camera = renderer->GetActiveCamera();

    // 获取相机当前参数
    double* focalPoint = camera->GetFocalPoint();
    double* position = camera->GetPosition();
    double viewUp[3];
    camera->GetViewUp(viewUp);

    // 计算平移向量 (根据屏幕坐标变化计算世界坐标变化)
    double scale = 0.01; // 平移灵敏度，值越大平移越快
    double translateX = -dx * scale;
    double translateY = dy * scale; // Y轴方向相反，因为屏幕坐标Y向下

    // 计算与视图平面平行的平移向量
    double right[3];
    vtkMath::Cross(camera->GetViewPlaneNormal(), viewUp, right);
    vtkMath::Normalize(right);

    // 计算新的位置和焦点
    double newPosition[3], newFocalPoint[3];
    for (int i = 0; i < 3; i++)
    {
      newPosition[i] = position[i] + translateX * right[i] + translateY * viewUp[i];
      newFocalPoint[i] = focalPoint[i] + translateX * right[i] + translateY * viewUp[i];
    }

    // 设置新的相机参数
    camera->SetPosition(newPosition);
    camera->SetFocalPoint(newFocalPoint);

    // 重新渲染
    renderer->ResetCameraClippingRange();
    this->Interactor->Render();
  }

  // 更新视图
  _renderer->Render();
}

void RSInteractorStyle::OnLeftButtonUp()
{

  m_isDragging = false;
  Superclass::OnLeftButtonUp();
}

void RSInteractorStyle::OnMouseWheelForward()
{

  qDebug() << "OnMouseWheelForward";

  if (_picker)
  {
    // double scaleFactor = 1.2;
    // qDebug() << "Picked actor:";
    // auto transform = vtkSmartPointer<vtkTransform>::New();
    // transform->Scale(scaleFactor, scaleFactor, scaleFactor);

    // _picked_actor->SetUserTransform(transform);
  }

  qDebug() << "OnMouseWheelForward moved to ";

  // this->_renderer->GetActiveCamera()->Zoom(1.2);
  // this->_renderer->ResetCameraClippingRange();
  // this->Interactor->Render();

  this->Superclass::OnMouseWheelForward();
}

void RSInteractorStyle::OnMouseWheelBackward()
{

  qDebug() << "OnMouseWheelBackward";
  if (_picker)
  {
    // double scaleFactor = 1 / 1.2;
    // qDebug() << "Picked actor:";
    // auto transform = vtkSmartPointer<vtkTransform>::New();
    // transform->Scale(scaleFactor, scaleFactor, scaleFactor);
    // // 应用变换
    // _picked_actor->SetUserTransform(transform);
  }

  qDebug() << "OnMouseWheelBackward moved to ";

  // this->_renderer->GetActiveCamera()->Zoom(1 / 1.2);
  // this->_renderer->ResetCameraClippingRange();
  // this->Interactor->Render();

  this->Superclass::OnMouseWheelBackward();
}

void RSInteractorStyle::OnKeyPress()
{
  //
  auto interactor = this->GetInteractor();

  char key = interactor->GetKeyCode();

  qDebug() << "OnKeyPress key = " << key;

  switch (key)
  {
    // case 'W': // 前移视图
    //   this->GetDefaultRenderer()->GetActiveCamera()->Dolly(1.1);
    //   break;
    // case 'S': // 后移视图
    //   this->GetDefaultRenderer()->GetActiveCamera()->Dolly(0.9);
    //   break;
    // case 'R': // 重置视图
    //   this->GetDefaultRenderer()->ResetCamera();
    //   break;
    // default:
    //   break;
  }

  interactor->Render();

  this->Superclass::OnKeyPress();
}

RSInteractorStyle::RSInteractorStyle() {}

RSInteractorStyle::~RSInteractorStyle()
{
  qDebug() << "RSInteractorStyle::~RSInteractorStyle()";
}

void RSInteractorStyle::TranslatePlane(double dx, double dy)
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
void RSInteractorStyle::RotatePlane(double dx, double dy)
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