#include "rs_interactor_style.h"

#include <QDebug>
#include <iostream>
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
  // 获取鼠标位置
  int* event_pos = this->Interactor->GetEventPosition();

  _startX = event_pos[0];
  _startY = event_pos[1];

  qDebug() << "Left button down at (" << this->_startX << ", " << this->_startY << ")";

  if (!_picker)
  {
    qInfo() << "Picker is not set, cannot pick actor.";
    // return;
  }

  // _picker->Pick(this->_startX, this->_startY, 0, _renderer);

  // vtkProp3D* pickedActor = vtkProp3D::SafeDownCast(this->_picker->GetActor());
  // if (_picker)
  // {
  //   // _picked_actor = vtkProp3D::SafeDownCast(this->_picker->GetActor());
  //   qDebug() << "Picked actor:";
  //   // 这里可以添加更多逻辑来处理被选中的演员
  // }
  // 调用基类方法
  this->Superclass::OnLeftButtonDown();
}

void RSInteractorStyle::OnMouseMove()
{
  int* event_pos = this->Interactor->GetEventPosition();

  int x = event_pos[0];
  int y = event_pos[1];
  // qDebug() << "Mouse moved to (" << x << ", " << y << ")";

  this->Superclass::OnMouseMove();
}

void RSInteractorStyle::OnLeftButtonUp()
{
  int* event_pos = this->Interactor->GetEventPosition();

  _endX = event_pos[0];
  _endY = event_pos[1];

  qDebug() << "Left button up at (" << _endX << ", " << _endY << ")";

  // 计算平移量
  int dx = this->_endX - this->_startX;
  int dy = this->_endY - this->_startY;
  // 更新图像的位置
  qDebug() << "dx = " << dx << ", dy = " << dy;
  // this->Renderer->ResetCameraClippingRange();
  // this->Renderer->GetActiveCamera()->Translate(dx, dy, 0);
  // this->Renderer->ResetCameraClippingRange();
  // this->RenderWindow->Render();

  this->Superclass::OnLeftButtonUp();
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
