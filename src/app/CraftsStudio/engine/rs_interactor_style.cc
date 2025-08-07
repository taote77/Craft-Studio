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
    return;
  }

  _picker->Pick(this->_startX, this->_startY, 0, _renderer);

  vtkProp3D* pickedActor = vtkProp3D::SafeDownCast(this->_picker->GetActor());
  if (_picker)
  {
    _picked_actor = vtkProp3D::SafeDownCast(this->_picker->GetActor());
    qDebug() << "Picked actor:";
    // 这里可以添加更多逻辑来处理被选中的演员
  }
  // 调用基类方法
  this->Superclass::OnLeftButtonDown();
}

void RSInteractorStyle::OnMouseMove()
{
  int* event_pos = this->Interactor->GetEventPosition();

  int x = event_pos[0];
  int y = event_pos[1];
  qDebug() << "Mouse moved to (" << x << ", " << y << ")";

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

  // this->Superclass::OnLeftButtonUp();
}

void RSInteractorStyle::OnMouseWheelForward()
{
  if (_picker)
  {
    // 缩放比例因子
    double scaleFactor = 1.2;
    // 缩放演员
    qDebug() << "Picked actor:";
    auto transform = vtkSmartPointer<vtkTransform>::New();
    transform->Scale(scaleFactor, scaleFactor, scaleFactor);
    // 应用变换
    _picked_actor->SetUserTransform(transform);
  }

  qDebug() << "OnMouseWheelForward moved to ";

  // this->_renderer->GetActiveCamera()->Zoom(1.2);
  this->_renderer->ResetCameraClippingRange();
  this->Interactor->Render();
}

void RSInteractorStyle::OnMouseWheelBackward()
{
  if (_picker)
  {
    // 缩放比例因子
    double scaleFactor = 1 / 1.2;
    // 缩放演员
    qDebug() << "Picked actor:";
    auto transform = vtkSmartPointer<vtkTransform>::New();
    transform->Scale(scaleFactor, scaleFactor, scaleFactor);
    // 应用变换
    _picked_actor->SetUserTransform(transform);
  }

  qDebug() << "OnMouseWheelBackward moved to ";

  // this->_renderer->GetActiveCamera()->Zoom(1 / 1.2);
  this->_renderer->ResetCameraClippingRange();
  this->Interactor->Render();
}

RSInteractorStyle::RSInteractorStyle() {}

RSInteractorStyle::~RSInteractorStyle()
{
  qDebug() << "RSInteractorStyle::~RSInteractorStyle()";
}
