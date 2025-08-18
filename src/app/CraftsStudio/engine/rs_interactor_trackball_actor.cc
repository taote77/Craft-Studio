#include "rs_interactor_trackball_actor.h"

#include <QDebug>
#include <iostream>
#include <qdebug.h>
#include <qglobal.h>
#include <vtkProperty.h>

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
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  _picker->Pick(x, y, 0, _renderer);
  _picked_actor = _picker->GetProp3D();

  if (_picked_actor)
  {
    m_isDragging = true;
    m_startPos[0] = x;
    m_startPos[1] = y;
    _startX = x;
    _startY = y;

    // 高亮选中对象
    if (auto actor = vtkActor::SafeDownCast(_picked_actor))
    {
      m_originalProperty = actor->GetProperty();
      vtkNew<vtkProperty> highlightProperty;
      highlightProperty->DeepCopy(m_originalProperty);
      highlightProperty->SetColor(m_highlightColor);
      actor->SetProperty(highlightProperty);
    }

    this->Interactor->Render(); // 立即渲染
  }

  Superclass::OnLeftButtonDown();
}

void RSInteractorTrackActor::OnMouseMove()
{
  if (m_isDragging && _picked_actor)
  {
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    double dx = x - m_startPos[0];
    double dy = y - m_startPos[1];

    TranslatePlane(dx, dy);

    m_startPos[0] = x;
    m_startPos[1] = y;
  }

  Superclass::OnMouseMove();
}

void RSInteractorTrackActor::OnMouseWheelForward()
{
  if (_picked_actor)
  {
    double scale = _picked_actor->GetScale()[0];
    _picked_actor->SetScale(scale * 1.1, scale * 1.1, scale * 1.1);
    this->Interactor->Render();
  }
  this->Superclass::OnMouseWheelForward();
}

void RSInteractorTrackActor::OnLeftButtonUp()
{
  if (m_isDragging)
  {
    m_isDragging = false;

    // 恢复原始颜色
    if (_picked_actor && m_originalProperty)
    {
      if (auto actor = vtkActor::SafeDownCast(_picked_actor))
      {
        // actor->SetProperty(m_originalProperty);
      }
    }

    this->Interactor->Render(); // 立即渲染
  }

  this->Superclass::OnLeftButtonUp();
}

void RSInteractorTrackActor::OnMouseWheelBackward()
{
  if (_picked_actor)
  {
    double scale = _picked_actor->GetScale()[0];
    _picked_actor->SetScale(scale * 0.9, scale * 0.9, scale * 0.9);
    this->Interactor->Render();
  }
  this->Superclass::OnMouseWheelBackward();
}

void RSInteractorTrackActor::OnKeyPress()
{
  auto interactor = this->GetInteractor();
  char key = interactor->GetKeyCode();

  if (_picked_actor)
  {
    switch (key)
    {
      case 'r':
        _picked_actor->RotateX(15);
        break;
      case 'g':
        if (auto prop = dynamic_cast<vtkActor*>(_picked_actor.GetPointer()))
        {
          prop->GetProperty()->SetColor(0, 1, 0);
        }
        break;
      case 'b':
        if (auto prop = dynamic_cast<vtkActor*>(_picked_actor.GetPointer()))
        {
          prop->GetProperty()->SetColor(0, 0, 1);
        }
        break;
    }
    interactor->Render();
  }

  this->Superclass::OnKeyPress();
}

RSInteractorTrackActor::RSInteractorTrackActor()
{
  m_initialMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // 显式初始化
}

RSInteractorTrackActor::~RSInteractorTrackActor()
{
  qDebug() << "RSInteractorTrackActor::~RSInteractorTrackActor()";
}

void RSInteractorTrackActor::TranslatePlane(double dx, double dy)
{
  if (!m_initialMatrix)
  {
    m_initialMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // 动态初始化
  }

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
  if (!m_initialMatrix)
  {
    m_initialMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // 动态初始化
  }

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
