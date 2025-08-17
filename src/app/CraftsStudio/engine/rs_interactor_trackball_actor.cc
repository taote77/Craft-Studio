#include "rs_interactor_trackball_actor.h"

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

vtkStandardNewMacro(RSInteractorTrackActor);

void RSInteractorTrackActor::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  _renderer = renderer;
}

void RSInteractorTrackActor::SetvtkPropPicker(vtkSmartPointer<vtkPropPicker> picker)
{
  _picker = picker;
}

void RSInteractorTrackActor::OnLeftButtonDown()
{
  // 记录初始位置
  // m_startPos[0] = this->Interactor->GetEventPosition()[0];
  // m_startPos[1] = this->Interactor->GetEventPosition()[1];

  // // 获取当前平面变换矩阵
  // m_initialMatrix = m_planeMatrix;

  // // 标记操作类型
  // m_isDragging = true;

  // 调用基类处理
  Superclass::OnLeftButtonDown();
}

void RSInteractorTrackActor::OnMouseMove()
{
  Superclass::OnMouseMove();
}

void RSInteractorTrackActor::OnLeftButtonUp()
{

  m_isDragging = false;
  Superclass::OnLeftButtonUp();
}

void RSInteractorTrackActor::OnMouseWheelForward()
{
  this->Superclass::OnMouseWheelForward();
}

void RSInteractorTrackActor::OnMouseWheelBackward()
{
  this->Superclass::OnMouseWheelBackward();
}

void RSInteractorTrackActor::OnKeyPress()
{
  auto interactor = this->GetInteractor();
  char key = interactor->GetKeyCode();

  qDebug() << "OnKeyPress key = " << key;

  // auto renderer = this->Interactor->FindPokedRenderer(m_startPos[0], m_startPos[1]);
  // if (!renderer)
  // {
  //   return;
  // }

  // switch (key)
  // {
  //   case 'w': // 前移视图
  //     renderer->GetActiveCamera()->Dolly(1.1);
  //     break;
  //   case 's': // 后移视图
  //     renderer->GetActiveCamera()->Dolly(0.9);
  //     break;
  //   case 'r': // 重置视图
  //     renderer->ResetCamera();
  //     break;
  //   default:
  //     break;
  // }

  // renderer->ResetCameraClippingRange(); // 更新视图参数
  // this->Interactor->Render();

  this->Superclass::OnKeyPress();
}

RSInteractorTrackActor::RSInteractorTrackActor() {}

RSInteractorTrackActor::~RSInteractorTrackActor()
{
  qDebug() << "RSInteractorTrackActor::~RSInteractorTrackActor()";
}

void RSInteractorTrackActor::TranslatePlane(double dx, double dy)
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
void RSInteractorTrackActor::RotatePlane(double dx, double dy)
{
  vtkSmartPointer<vtkMatrix4x4> rotation = vtkSmartPointer<vtkMatrix4x4>::New();

  // 计算旋转角度（根据视口比例）
  vtkSmartPointer<vtkRenderWindow> renWin = this->Interactor->GetRenderWindow();
  double scale = renWin->GetSize()[0] / 1000.0;

  double angleX = dy * scale * 0.5;  // Y轴旋转
  double angleY = -dx * scale * 0.5; // X轴旋转

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
