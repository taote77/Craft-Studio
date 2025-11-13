#include "rs_interactor_trackball_camera.h"
#include "rs_scene_manager.h"
#include "rs_transform_gizmo.h"

#include <QDebug>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
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

void RSInteractorTrackCamera::SetSceneManager(SceneManager* manager)
{
  m_sceneManager = manager;
}

void RSInteractorTrackCamera::SetTransformGizmo(TransformGizmo* gizmo)
{
  m_transformGizmo = gizmo;
}

// 拾取操作
vtkProp3D* RSInteractorTrackCamera::PickActor(int x, int y)
{
  if (!_picker || !_renderer)
    return nullptr;

  _picker->Pick(x, y, 0, _renderer);
  return vtkProp3D::SafeDownCast(_picker->GetViewProp());
}

// 手柄拾取
bool RSInteractorTrackCamera::PickGizmoHandle(int x, int y, int& handleType, int& axisIndex)
{
  if (!m_transformGizmo || !m_sceneManager || !m_sceneManager->getSelectedObject())
    return false;

  return m_transformGizmo->PickHandle(x, y, handleType, axisIndex);
}

void RSInteractorTrackCamera::OnLeftButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  // 记录初始位置
  m_startPos[0] = x;
  m_startPos[1] = y;

  // 获取当前平面变换矩阵
  if (m_planeMatrix)
  {
    m_initialMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    m_initialMatrix->DeepCopy(m_planeMatrix);
  }

  // 首先检查是否拾取到操纵器手柄
  if (m_sceneManager && m_sceneManager->getSelectedObject())
  {
    int handleType = -1, axisIndex = -1;
    if (PickGizmoHandle(x, y, handleType, axisIndex))
    {
      // 开始操纵器交互
      StartGizmoInteraction(x, y);
      m_gizmoHandleType = handleType;
      m_gizmoAxisIndex = axisIndex;
      return;
    }
  }

  // 检查是否拾取到模型
  vtkProp3D* pickedActor = PickActor(x, y);
  if (pickedActor && m_sceneManager)
  {
    SceneObject* selectedObject = m_sceneManager->findObjectByActor((vtkActor*)pickedActor);
    if (selectedObject)
    {
      // 选择模型
      m_sceneManager->setObjectSelected(selectedObject, true);
      m_interactionState = OBJECT_SELECTION;

      // 根据当前变换模式显示操纵器
      if (m_transformGizmo)
      {
        m_transformGizmo->setTarget(selectedObject->_actor);
        m_transformGizmo->setTransformMode((int)m_sceneManager->getTransformMode());
        m_transformGizmo->show();
      }

      // 标记拖动开始
      m_isDragging = true;
      return;
    }
  }

  // 如果没有拾取到任何对象，清空选择
  if (m_sceneManager)
  {
    m_sceneManager->clearSelection();
    if (m_transformGizmo)
      m_transformGizmo->Hide();
  }

  // 默认相机控制模式
  m_interactionState = CAMERA_MODE;
  m_isDragging = true;

  // 调用基类处理
  Superclass::OnLeftButtonDown();
}

void RSInteractorTrackCamera::OnMouseMove()
{
  if (!m_isDragging)
    return;

  int* pos = this->Interactor->GetEventPosition();
  int x = pos[0];
  int y = pos[1];

  // 检查操纵器交互
  if (m_isGizmoInteracting && m_transformGizmo && m_sceneManager &&
    m_sceneManager->getSelectedObject())
  {
    UpdateGizmoInteraction(x, y);
    return;
  }

  // 根据交互状态处理
  switch (m_interactionState)
  {
    case CAMERA_MODE:
      // 相机控制模式
      HandleCameraMovement(x, y);
      break;

    case OBJECT_SELECTION:
      // 对象选择模式（可以添加拖拽移动等操作）
      break;

    case GIZMO_INTERACTION:
      // 操纵器交互模式
      UpdateGizmoInteraction(x, y);
      break;
  }
}

// 处理相机移动
void RSInteractorTrackCamera::HandleCameraMovement(int x, int y)
{
  double dx = x - m_startPos[0];
  double dy = y - m_startPos[1];

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

  // 根据VTK标准实现旋转逻辑
  if (this->Interactor->GetControlKey()) // Ctrl键触发旋转
  {
    // 计算旋转角度（根据视口比例调整灵敏度）
    constexpr double rotate_scale = 0.01;
    double deltaAzimuth = -dx * rotate_scale / viewWidth * 360.0;
    double deltaElevation = -dy * rotate_scale / viewHeight * 360.0;

    // 执行旋转操作
    camera->Azimuth(deltaAzimuth);
    camera->Elevation(deltaElevation);
    camera->OrthogonalizeViewUp();
  }
  else // 默认平移操作
  {
    // 平移参数设置
    constexpr double scale = 0.01;
    double translateX = dx * scale;
    double translateY = -dy * scale;

    // 计算平移向量
    double right[3], viewUp[3];
    vtkMath::Cross(camera->GetViewPlaneNormal(), camera->GetViewUp(), right);
    vtkMath::Normalize(right);

    // 更新相机位置和焦点
    double newPosition[3], newFocalPoint[3];
    for (int i = 0; i < 3; i++)
    {
      newPosition[i] = position[i] + translateX * right[i] + translateY * camera->GetViewUp()[i];
      newFocalPoint[i] =
        focalPoint[i] + translateX * right[i] + translateY * camera->GetViewUp()[i];
    }

    camera->SetPosition(newPosition);
    camera->SetFocalPoint(newFocalPoint);
  }

  renderer->ResetCameraClippingRange();
  this->Interactor->Render();
}

void RSInteractorTrackCamera::OnLeftButtonUp()
{
  // 结束操纵器交互
  if (m_isGizmoInteracting)
  {
    EndGizmoInteraction();
  }

  m_isDragging = false;
  m_isGizmoInteracting = false;

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

// 开始操纵器交互
void RSInteractorTrackCamera::StartGizmoInteraction(int x, int y)
{
  if (!m_transformGizmo || !m_sceneManager || !m_sceneManager->getSelectedObject())
    return;

  m_isGizmoInteracting = true;
  m_interactionState = GIZMO_INTERACTION;
  m_startMousePos[0] = x;
  m_startMousePos[1] = y;

  // 保存初始变换状态
  SceneObject* selectedObject = m_sceneManager->getSelectedObject();
  if (selectedObject && selectedObject->_actor)
  {
    vtkTransform* transform = (vtkTransform*)selectedObject->_actor->GetUserTransform();
    if (transform)
    {
      vtkMatrix4x4* matrix = transform->GetMatrix();
      matrix->DeepCopy(m_startTransform);
    }
    else
    {
      // 如果没有变换，使用单位矩阵
      vtkMatrix4x4::Identity(m_startTransform);
    }
  }
}

// 更新操纵器交互
void RSInteractorTrackCamera::UpdateGizmoInteraction(int x, int y)
{
  if (!m_transformGizmo || !m_sceneManager || !m_sceneManager->getSelectedObject())
    return;

  SceneObject* selectedObject = m_sceneManager->getSelectedObject();
  if (!selectedObject || !selectedObject->_actor)
    return;

  double dx = x - m_startMousePos[0];
  double dy = y - m_startMousePos[1];

  // 根据手柄类型执行变换
  switch (m_gizmoHandleType)
  {
    case 0: // 平移
      m_transformGizmo->ApplyTranslation(dx, dy, m_gizmoAxisIndex);
      break;

    case 1: // 旋转
      m_transformGizmo->ApplyRotation(dx, dy, m_gizmoAxisIndex);
      break;

    case 2: // 缩放
      m_transformGizmo->ApplyScale(dx, dy, m_gizmoAxisIndex);
      break;
  }

  // 更新渲染
  this->Interactor->Render();
}

// 结束操纵器交互
void RSInteractorTrackCamera::EndGizmoInteraction()
{
  m_isGizmoInteracting = false;
  m_gizmoHandleType = -1;
  m_gizmoAxisIndex = -1;

  // 发出变换完成信号
  if (m_sceneManager && m_sceneManager->getSelectedObject())
  {
    emit m_sceneManager->objectGeometryChanged(m_sceneManager->getSelectedObject());
  }
}