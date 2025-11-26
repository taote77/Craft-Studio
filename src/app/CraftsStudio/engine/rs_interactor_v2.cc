#include "rs_interactor_v2.h"

#include "rs_scene_manager_v2.h"
#include "rs_scene_object_v2.h"
#include "rs_transform_gizmo.h"

#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkWindowToImageFilter.h>

vtkStandardNewMacro(RSInteractorV2);

RSInteractorV2::RSInteractorV2()
{
  // 初始化拾取器
  m_picker = vtkSmartPointer<vtkPropPicker>::New();
  m_picker->PickFromListOn();

  // 初始化相机状态
  for (int i = 0; i < 3; ++i)
  {
    m_startCameraPosition[i] = 0.0;
    m_startFocalPoint[i] = 0.0;
    m_startViewUp[i] = 0.0;
  }

  m_startViewUp[1] = 1.0; // 默认Y轴向上

  m_startParallelScale = 1.0;
  m_startDistance = 1.0;
}

RSInteractorV2::~RSInteractorV2()
{
  // 清理测量点
  for (auto point : m_measurePoints)
  {
    delete[] point;
  }
  m_measurePoints.clear();
}

void RSInteractorV2::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  m_renderer = renderer;
  if (renderer)
  {
    m_camera = renderer->GetActiveCamera();
  }
}

void RSInteractorV2::SetSceneManager(SceneManagerV2* manager)
{
  m_sceneManager = manager;
}

void RSInteractorV2::SetTransformGizmo(TransformGizmo* gizmo)
{
  m_transformGizmo = gizmo;
  if (m_transformGizmo && m_renderer)
  {
    m_transformGizmo->setRenderer(m_renderer);
  }
}

vtkProp3D* RSInteractorV2::PickActor(int x, int y)
{
  if (!m_renderer || !m_picker)
    return nullptr;

  m_picker->Pick(x, y, 0, m_renderer);
  return m_picker->GetActor();
}

bool RSInteractorV2::PickWorldPosition(int x, int y, double worldPos[3])
{
  if (!m_renderer || !m_picker)
    return false;

  // 使用拾取器获取世界坐标
  m_picker->Pick(x, y, 0, m_renderer);
  if (m_picker->GetActor())
  {
    m_picker->GetPickPosition(worldPos);
    return true;
  }

  // 如果没有拾取到对象，则投影到平面上
  double displayPos[3] = { static_cast<double>(x), static_cast<double>(y), 0.0 };
  vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
  coordinate->SetCoordinateSystemToDisplay();
  coordinate->SetValue(displayPos);

  double* worldCoords = coordinate->GetComputedWorldValue(m_renderer);
  worldPos[0] = worldCoords[0];
  worldPos[1] = worldCoords[1];
  worldPos[2] = worldCoords[2];

  return true;
}

void RSInteractorV2::SetViewType(ViewType type)
{
  if (m_viewType == type || !m_camera || !m_renderer)
    return;

  m_viewType = type;

  // 获取所有对象的边界
  double bounds[6];
  if (m_sceneManager)
  {
    m_sceneManager->getSceneBounds(bounds);
  }
  else
  {
    // 默认边界
    bounds[0] = bounds[2] = bounds[4] = -50.0;
    bounds[1] = bounds[3] = bounds[5] = 50.0;
  }

  double center[3] = { (bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0,
    (bounds[4] + bounds[5]) / 2.0 };

  // 计算合适的距离
  double size = qMax(qMax(bounds[1] - bounds[0], bounds[3] - bounds[2]), bounds[5] - bounds[4]);
  double distance = size * 2.5;

  // 根据视图类型设置相机参数
  switch (type)
  {
    case ViewType::Perspective:
      SwitchToPerspectiveView();
      m_camera->SetPosition(center[0], center[1], center[2] + distance);
      m_camera->SetViewUp(0, 1, 0);
      break;

    case ViewType::Orthographic:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0], center[1], center[2] + distance);
      m_camera->SetViewUp(0, 1, 0);
      break;

    case ViewType::Top:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0], center[1], center[2] + distance);
      m_camera->SetViewUp(0, 1, 0);
      break;

    case ViewType::Bottom:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0], center[1], center[2] - distance);
      m_camera->SetViewUp(0, -1, 0);
      break;

    case ViewType::Front:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0], center[1] - distance, center[2]);
      m_camera->SetViewUp(0, 0, 1);
      break;

    case ViewType::Back:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0], center[1] + distance, center[2]);
      m_camera->SetViewUp(0, 0, 1);
      break;

    case ViewType::Left:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0] - distance, center[1], center[2]);
      m_camera->SetViewUp(0, 0, 1);
      break;

    case ViewType::Right:
      SwitchToOrthographicView();
      m_camera->SetPosition(center[0] + distance, center[1], center[2]);
      m_camera->SetViewUp(0, 0, 1);
      break;
  }

  m_camera->SetFocalPoint(center);
  m_renderer->ResetCameraClippingRange();

  if (this->Interactor)
    this->Interactor->Render();
}

void RSInteractorV2::ResetCamera()
{
  if (m_camera && m_renderer)
  {
    m_renderer->ResetCamera();
    m_renderer->ResetCameraClippingRange();

    if (this->Interactor)
      this->Interactor->Render();
  }
}

void RSInteractorV2::FitAll()
{
  if (!m_renderer || !m_camera)
    return;

  // 获取所有对象的边界
  double bounds[6];
  if (m_sceneManager)
  {
    m_sceneManager->getSceneBounds(bounds);
  }
  else
  {
    // 默认边界
    bounds[0] = bounds[2] = bounds[4] = -50.0;
    bounds[1] = bounds[3] = bounds[5] = 50.0;
  }

  double center[3] = { (bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0,
    (bounds[4] + bounds[5]) / 2.0 };

  // 计算合适的距离
  double size = qMax(qMax(bounds[1] - bounds[0], bounds[3] - bounds[2]), bounds[5] - bounds[4]);
  double distance = size * 2.5;

  // 设置相机
  m_camera->SetFocalPoint(center);
  m_camera->SetPosition(center[0], center[1], center[2] + distance);
  m_renderer->ResetCameraClippingRange();

  if (this->Interactor)
    this->Interactor->Render();
}

void RSInteractorV2::FitSelected()
{
  if (!m_renderer || !m_camera)
    return;

  // 获取选中对象的边界
  double bounds[6];
  if (m_sceneManager && !m_sceneManager->selectedObjects().isEmpty())
  {
    m_sceneManager->getSelectedObjectsBounds(bounds);
  }
  else
  {
    // 如果没有选中对象，则拟合所有对象
    FitAll();
    return;
  }

  double center[3] = { (bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0,
    (bounds[4] + bounds[5]) / 2.0 };

  // 计算合适的距离
  double size = qMax(qMax(bounds[1] - bounds[0], bounds[3] - bounds[2]), bounds[5] - bounds[4]);
  double distance = size * 2.5;

  // 设置相机
  m_camera->SetFocalPoint(center);
  m_camera->SetPosition(center[0], center[1], center[2] + distance);
  m_renderer->ResetCameraClippingRange();

  if (this->Interactor)
    this->Interactor->Render();
}

void RSInteractorV2::ZoomToExtents()
{
  FitAll();
}

void RSInteractorV2::SetInteractionMode(InteractionMode mode)
{
  if (m_interactionMode == mode)
    return;

  m_interactionMode = mode;
  UpdateCursor();
  UpdateStatusBar();
}

void RSInteractorV2::SetFocalPoint(double x, double y, double z)
{
  if (m_camera)
  {
    m_camera->SetFocalPoint(x, y, z);
  }
}

void RSInteractorV2::SetFocalPoint(double focalPoint[3])
{
  SetFocalPoint(focalPoint[0], focalPoint[1], focalPoint[2]);
}

void RSInteractorV2::SetCameraPosition(double x, double y, double z)
{
  if (m_camera)
  {
    m_camera->SetPosition(x, y, z);
  }
}

void RSInteractorV2::SetCameraPosition(double position[3])
{
  SetCameraPosition(position[0], position[1], position[2]);
}

void RSInteractorV2::ProjectToPlane(
  double screenPos[2], double planeNormal[3], double planePoint[3], double worldPos[3])
{
  // 将屏幕坐标转换为世界坐标
  double displayPos[3] = { screenPos[0], screenPos[1], 0.0 };
  vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
  coordinate->SetCoordinateSystemToDisplay();
  coordinate->SetValue(displayPos);

  double* rayStart = coordinate->GetComputedWorldValue(m_renderer);

  // 获取相机方向作为射线方向
  double cameraPos[3];
  m_camera->GetPosition(cameraPos);

  double rayDir[3];
  for (int i = 0; i < 3; ++i)
  {
    rayDir[i] = rayStart[i] - cameraPos[i];
  }

  vtkMath::Normalize(rayDir);

  // 计算射线与平面的交点
  double rayToPlane[3];
  for (int i = 0; i < 3; ++i)
  {
    rayToPlane[i] = planePoint[i] - cameraPos[i];
  }

  double dotProduct = vtkMath::Dot(rayDir, planeNormal);
  if (fabs(dotProduct) < 1e-6)
  {
    // 射线与平面平行，返回平面上的点
    for (int i = 0; i < 3; ++i)
    {
      worldPos[i] = planePoint[i];
    }
    return;
  }

  double t = vtkMath::Dot(rayToPlane, planeNormal) / dotProduct;

  for (int i = 0; i < 3; ++i)
  {
    worldPos[i] = cameraPos[i] + t * rayDir[i];
  }
}

void RSInteractorV2::OnLeftButtonDown()
{
  if (!this->Interactor || !m_renderer)
    return;

  this->GetInteractor()->GetEventPosition(m_startMousePos);
  m_currentMousePos[0] = m_startMousePos[0];
  m_currentMousePos[1] = m_startMousePos[1];

  m_isDragging = true;

  // 根据当前交互模式处理
  switch (m_interactionMode)
  {
    case InteractionMode::Camera:
      StartCameraManipulation();
      break;

    case InteractionMode::Selection:
      StartSelection();
      break;

    case InteractionMode::Transform:
      StartTransform();
      break;

    case InteractionMode::Measure:
      StartMeasure();
      break;
  }

  // 调用基类方法
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void RSInteractorV2::OnMouseMove()
{
  if (!this->Interactor)
    return;

  this->GetInteractor()->GetEventPosition(m_currentMousePos);

  if (m_isDragging)
  {
    // 根据当前交互模式处理
    switch (m_interactionMode)
    {
      case InteractionMode::Camera:
        UpdateCameraManipulation();
        break;

      case InteractionMode::Selection:
        UpdateSelection();
        break;

      case InteractionMode::Transform:
        UpdateTransform();
        break;

      case InteractionMode::Measure:
        UpdateMeasure();
        break;
    }
  }

  // 更新最后鼠标位置
  m_lastMousePos[0] = m_currentMousePos[0];
  m_lastMousePos[1] = m_currentMousePos[1];

  // 调用基类方法
  vtkInteractorStyleTrackballCamera::OnMouseMove();
}

void RSInteractorV2::OnLeftButtonUp()
{
  if (!this->Interactor)
    return;

  m_isDragging = false;

  // 根据当前交互模式处理
  switch (m_interactionMode)
  {
    case InteractionMode::Camera:
      EndCameraManipulation();
      break;

    case InteractionMode::Selection:
      EndSelection();
      break;

    case InteractionMode::Transform:
      EndTransform();
      break;

    case InteractionMode::Measure:
      EndMeasure();
      break;
  }

  // 调用基类方法
  vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void RSInteractorV2::OnRightButtonDown()
{
  // 右键菜单或切换到相机模式
  if (m_interactionMode != InteractionMode::Camera)
  {
    SetInteractionMode(InteractionMode::Camera);
  }

  vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void RSInteractorV2::OnRightButtonUp()
{
  vtkInteractorStyleTrackballCamera::OnRightButtonUp();
}

void RSInteractorV2::OnMiddleButtonDown()
{
  // 中键通常用于平移视图
  vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}

void RSInteractorV2::OnMiddleButtonUp()
{
  vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
}

void RSInteractorV2::OnMouseWheelForward()
{
  // 缩放
  vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
}

void RSInteractorV2::OnMouseWheelBackward()
{
  // 缩放
  vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
}

void RSInteractorV2::OnKeyPress()
{
  if (!this->Interactor || !this->Interactor->GetKeySym())
    return;

  const char* key = this->Interactor->GetKeySym();

  // 处理快捷键
  if (strcmp(key, "Escape") == 0)
  {
    // ESC键取消当前操作
    SetInteractionMode(InteractionMode::Camera);
    if (m_sceneManager)
    {
      m_sceneManager->clearSelection();
    }
  }
  else if (strcmp(key, "Delete") == 0)
  {
    // 删除选中的对象
    if (m_sceneManager)
    {
      auto selectedObjects = m_sceneManager->selectedObjects();
      m_sceneManager->removeObjects(selectedObjects);
    }
  }
  else if (strcmp(key, "a") == 0 || strcmp(key, "A") == 0)
  {
    // 全选
    if (m_sceneManager)
    {
      m_sceneManager->selectAll();
    }
  }
  else if (strcmp(key, "1") == 0)
  {
    SetViewType(ViewType::Top);
  }
  else if (strcmp(key, "3") == 0)
  {
    SetViewType(ViewType::Front);
  }
  else if (strcmp(key, "7") == 0)
  {
    SetViewType(ViewType::Left);
  }
  else if (strcmp(key, "5") == 0)
  {
    SetViewType(ViewType::Perspective);
  }

  // 调用基类方法
  vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void RSInteractorV2::OnKeyRelease()
{
  vtkInteractorStyleTrackballCamera::OnKeyRelease();
}

void RSInteractorV2::OnPinch()
{
  // 处理捏合手势（缩放）
  vtkInteractorStyleTrackballCamera::OnPinch();
}

void RSInteractorV2::OnRotate()
{
  // 处理旋转手势
  vtkInteractorStyleTrackballCamera::OnRotate();
}

void RSInteractorV2::OnPan()
{
  // 处理平移手势
  vtkInteractorStyleTrackballCamera::OnPan();
}

bool RSInteractorV2::PickGizmoHandle(int x, int y, int& handleType, int& axisIndex)
{
  // 实现操纵器手柄拾取
  if (m_transformGizmo)
  {
    GizmoHandleType gizmoType;
    if (m_transformGizmo->rayPickHandle(x, y, gizmoType, axisIndex))
    {
      // 将GizmoHandleType转换为整数类型
      switch (gizmoType)
      {
        case GizmoHandleType::TranslateX:
        case GizmoHandleType::TranslateY:
        case GizmoHandleType::TranslateZ:
          handleType = 0;
          break;
        case GizmoHandleType::RotateX:
        case GizmoHandleType::RotateY:
        case GizmoHandleType::RotateZ:
          handleType = 1;
          break;
        case GizmoHandleType::ScaleX:
        case GizmoHandleType::ScaleY:
        case GizmoHandleType::ScaleZ:
        case GizmoHandleType::UniformScale:
          handleType = 2;
          break;
        default:
          handleType = -1;
          break;
      }

      // 设置全局的GizmoHandleType
      m_gizmoHandleType = static_cast<int>(gizmoType);

      return true;
    }
  }
  return false;
}

void RSInteractorV2::StartCameraManipulation()
{
  if (!m_camera)
    return;

  // 保存相机状态
  m_camera->GetPosition(m_startCameraPosition);
  m_camera->GetFocalPoint(m_startFocalPoint);
  m_camera->GetViewUp(m_startViewUp);
  m_startParallelScale = m_camera->GetParallelScale();

  double focalPoint[3];
  m_camera->GetFocalPoint(focalPoint);
  m_startDistance = vtkMath::Distance2BetweenPoints(m_startCameraPosition, focalPoint);
  m_startDistance = sqrt(m_startDistance);
}

void RSInteractorV2::UpdateCameraManipulation()
{
  // 相机操作由基类处理
}

void RSInteractorV2::EndCameraManipulation()
{
  // 相机操作由基类处理
}

void RSInteractorV2::StartSelection()
{
  // 开始框选
  m_selectionRect = QRectF(m_startMousePos[0], m_startMousePos[1], 0, 0);
}

void RSInteractorV2::UpdateSelection()
{
  // 更新选择框
  m_selectionRect.setLeft(qMin(m_startMousePos[0], m_currentMousePos[0]));
  m_selectionRect.setRight(qMax(m_startMousePos[0], m_currentMousePos[0]));
  m_selectionRect.setTop(qMin(m_startMousePos[1], m_currentMousePos[1]));
  m_selectionRect.setBottom(qMax(m_startMousePos[1], m_currentMousePos[1]));
}

void RSInteractorV2::EndSelection()
{
  if (!m_sceneManager)
    return;

  // 如果选择框很小，则执行点选
  if (m_selectionRect.width() < 5 && m_selectionRect.height() < 5)
  {
    vtkProp3D* actor = PickActor(m_currentMousePos[0], m_currentMousePos[1]);
    if (actor)
    {
      SceneObjectV2* obj = m_sceneManager->findObjectByActor(vtkActor::SafeDownCast(actor));
      if (obj)
      {
        // 判断是否添加到多选
        bool addToSelection = IsModifierKeyPressed(ControlKey);
        m_sceneManager->setObjectSelected(obj, true, addToSelection);
      }
    }
    else
    {
      // 点击空白区域，清除选择
      if (!IsModifierKeyPressed(ShiftKey))
      {
        m_sceneManager->clearSelection();
      }
    }
  }
  else
  {
    // 执行框选
    m_sceneManager->selectObjectsInRect(m_selectionRect);
  }
}

void RSInteractorV2::StartTransform()
{
  if (!m_transformGizmo || !m_sceneManager)
    return;

  // 检查是否点击了操纵器手柄
  GizmoHandleType handleType;
  int axisIndex;

  if (PickGizmoHandle(m_startMousePos[0], m_startMousePos[1], m_gizmoHandleType, axisIndex))
  {
    m_isGizmoInteracting = true;
    m_gizmoAxisIndex = axisIndex;

    // 高亮选中的手柄
    m_transformGizmo->highlightHandle(handleType, axisIndex);

    // 开始变换预览
    m_transformGizmo->startTransformPreview();
  }
  else
  {
    // 否则检查是否点击了对象
    vtkProp3D* actor = PickActor(m_startMousePos[0], m_startMousePos[1]);
    if (actor)
    {
      SceneObjectV2* obj = m_sceneManager->findObjectByActor(vtkActor::SafeDownCast(actor));
      if (obj)
      {
        // 选中对象并显示操纵器
        m_sceneManager->setObjectSelected(obj, true);
      }
    }
  }
}

void RSInteractorV2::UpdateTransform()
{
  if (!m_transformGizmo || !m_sceneManager)
    return;

  if (m_isGizmoInteracting)
  {
    double dx = m_currentMousePos[0] - m_startMousePos[0];
    double dy = m_currentMousePos[1] - m_startMousePos[1];

    // 更新变换预览
    m_transformGizmo->updateTransformPreview(
      dx, dy, m_gizmoAxisIndex, static_cast<GizmoHandleType>(m_gizmoHandleType));

    // 根据手柄类型执行变换预览
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
  }
}

void RSInteractorV2::EndTransform()
{
  if (m_isGizmoInteracting)
  {
    // 结束变换预览
    m_transformGizmo->endTransformPreview();

    // 清除手柄高亮
    m_transformGizmo->clearHighlight();
  }

  m_isGizmoInteracting = false;
  m_gizmoHandleType = -1;
  m_gizmoAxisIndex = -1;
}

void RSInteractorV2::StartMeasure()
{
  double worldPos[3];
  if (PickWorldPosition(m_startMousePos[0], m_startMousePos[1], worldPos))
  {
    double* point = new double[3];
    point[0] = worldPos[0];
    point[1] = worldPos[1];
    point[2] = worldPos[2];
    m_measurePoints.append(point);
    m_measuring = true;
  }
}

void RSInteractorV2::UpdateMeasure()
{
  // 更新测量线
}

void RSInteractorV2::EndMeasure()
{
  m_measuring = false;
}

void RSInteractorV2::SwitchToPerspectiveView()
{
  if (m_camera)
  {
    m_camera->SetParallelProjection(false);
    m_viewType = ViewType::Perspective;
  }
}

void RSInteractorV2::SwitchToOrthographicView()
{
  if (m_camera)
  {
    m_camera->SetParallelProjection(true);
    m_viewType = ViewType::Orthographic;
  }
}

void RSInteractorV2::SwitchToTopView()
{
  SwitchToOrthographicView();
  m_viewType = ViewType::Top;
}

void RSInteractorV2::SwitchToBottomView()
{
  SwitchToOrthographicView();
  m_viewType = ViewType::Bottom;
}

void RSInteractorV2::SwitchToFrontView()
{
  SwitchToOrthographicView();
  m_viewType = ViewType::Front;
}

void RSInteractorV2::SwitchToBackView()
{
  SwitchToOrthographicView();
  m_viewType = ViewType::Back;
}

void RSInteractorV2::SwitchToLeftView()
{
  SwitchToOrthographicView();
  m_viewType = ViewType::Left;
}

void RSInteractorV2::SwitchToRightView()
{
  SwitchToOrthographicView();
  m_viewType = ViewType::Right;
}

void RSInteractorV2::UpdateCursor()
{
  // 根据交互模式更新鼠标光标
  switch (m_interactionMode)
  {
    case InteractionMode::Camera:
      // 默认光标
      break;

    case InteractionMode::Selection:
      // 选择光标
      break;

    case InteractionMode::Transform:
      // 移动光标
      break;

    case InteractionMode::Measure:
      // 测量光标
      break;
  }
}

void RSInteractorV2::UpdateStatusBar()
{
  // 更新状态栏文本
  switch (m_interactionMode)
  {
    case InteractionMode::Camera:
      m_statusText = "相机模式 - 左键拖动旋转，右键拖动平移，滚轮缩放";
      break;

    case InteractionMode::Selection:
      m_statusText = "选择模式 - 左键选择对象，Ctrl+点击多选，框选多个对象";
      break;

    case InteractionMode::Transform:
      m_statusText = "变换模式 - 拖动操纵器手柄进行变换";
      break;

    case InteractionMode::Measure:
      m_statusText = "测量模式 - 左键点击测量点，ESC结束测量";
      break;
  }
}

bool RSInteractorV2::IsModifierKeyPressed(int modifier)
{
  if (!this->Interactor)
    return false;

  return (this->Interactor->GetShiftKey() && modifier == ShiftKey) ||
    (this->Interactor->GetControlKey() && modifier == ControlKey) ||
    (this->Interactor->GetAltKey() && modifier == AltKey);
}

bool RSInteractorV2::IsKeyPressed(char key)
{
  // 检查特定键是否被按下
  return false;
}

void RSInteractorV2::CalculateRotation(
  double dx, double dy, double& angleX, double& angleY, double& angleZ)
{
  // 计算旋转角度
  angleX = dy * m_rotationSpeed;
  angleY = dx * m_rotationSpeed;
  angleZ = 0.0;
}

void RSInteractorV2::CalculateTranslation(double dx, double dy, double& tx, double& ty, double& tz)
{
  // 计算平移量
  tx = dx * m_panSpeed;
  ty = dy * m_panSpeed;
  tz = 0.0;
}

void RSInteractorV2::CalculateScale(double dx, double dy, double& sx, double& sy, double& sz)
{
  // 计算缩放量
  double scale = 1.0 + (dx + dy) * 0.01;
  sx = scale;
  sy = scale;
  sz = scale;
}