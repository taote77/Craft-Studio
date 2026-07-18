#ifndef RS_INTERACTOR_V2_H
#define RS_INTERACTOR_V2_H

#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkProp3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTimeStamp.h>
#include <vtkTransform.h>

#include <QList>
#include <QRectF>

class vtkPropPicker;
class TransformGizmo;
class SceneManagerV2;

// 修饰键枚举
enum ModifierKey
{
  ShiftKey = 1,
  ControlKey = 2,
  AltKey = 4
};

// 相机视图类型
enum class ViewType
{
  Perspective,  // 透视图
  Orthographic, // 正交图
  Top,          // 顶视图
  Bottom,       // 底视图
  Front,        // 前视图
  Back,         // 后视图
  Left,         // 左视图
  Right         // 右视图
};

// 交互模式
enum class InteractionMode
{
  Camera,    // 相机控制模式
  Selection, // 对象选择模式
  Transform, // 变换操作模式
  Measure    // 测量模式
};

class RSInteractorV2 : public vtkInteractorStyleTrackballCamera
{
public:
  static RSInteractorV2* New();
  vtkTypeMacro(RSInteractorV2, vtkInteractorStyleTrackballCamera);

  // 设置渲染器
  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);

  // 设置场景管理器 (supports both legacy and new SceneDocument)
  void SetSceneManager(SceneManagerV2* manager);
  void SetSceneDocument(void* doc); // csengine::SceneDocument* (void* to avoid header dep)

  // 设置变换操纵器
  void SetTransformGizmo(TransformGizmo* gizmo);

  // 拾取操作
  vtkProp3D* PickActor(int x, int y);

  // 获取拾取的世界坐标
  bool PickWorldPosition(int x, int y, double worldPos[3]);

  // 视图控制
  void SetViewType(ViewType type);
  ViewType GetViewType() const { return m_viewType; }

  void ResetCamera();
  void FitAll();
  void FitSelected();
  void ZoomToExtents();

  // 交互模式控制
  void SetInteractionMode(InteractionMode mode);
  InteractionMode GetInteractionMode() const { return m_interactionMode; }

  // 选择控制
  void SetMultiSelectionEnabled(bool enabled) { m_multiSelectionEnabled = enabled; }
  bool IsMultiSelectionEnabled() const { return m_multiSelectionEnabled; }

  // 手柄拾取
  bool PickGizmoHandle(int x, int y, int& handleType, int& axisIndex);

  // 焦距控制
  void SetFocalPoint(double x, double y, double z);
  void SetFocalPoint(double focalPoint[3]);

  // 视角控制
  void SetCameraPosition(double x, double y, double z);
  void SetCameraPosition(double position[3]);

  // 平面投影
  void ProjectToPlane(
    double screenPos[2], double planeNormal[3], double planePoint[3], double worldPos[3]);

protected:
  // 鼠标事件
  void OnLeftButtonDown() override;
  void OnMouseMove() override;
  void OnLeftButtonUp() override;
  void OnRightButtonDown() override;
  void OnRightButtonUp() override;
  void OnMiddleButtonDown() override;
  void OnMiddleButtonUp() override;
  void OnMouseWheelForward() override;
  void OnMouseWheelBackward() override;

  // 键盘事件
  void OnKeyPress() override;
  void OnKeyRelease() override;

  // 触摸事件
  void OnPinch() override;
  void OnRotate() override;
  void OnPan() override;

protected:
  RSInteractorV2();
  ~RSInteractorV2();

  // 内部状态处理
  void StartCameraManipulation();
  void UpdateCameraManipulation();
  void EndCameraManipulation();

  void StartSelection();
  void UpdateSelection();
  void EndSelection();

  void StartTransform();
  void UpdateTransform();
  void EndTransform();

  void StartMeasure();
  void UpdateMeasure();
  void EndMeasure();

  // 视图切换
  void SwitchToPerspectiveView();
  void SwitchToOrthographicView();
  void SwitchToTopView();
  void SwitchToBottomView();
  void SwitchToFrontView();
  void SwitchToBackView();
  void SwitchToLeftView();
  void SwitchToRightView();

  // 辅助函数
  void UpdateCursor();
  void UpdateStatusBar();
  bool IsModifierKeyPressed(int modifier);
  bool IsKeyPressed(char key);

  // 计算函数
  void CalculateRotation(double dx, double dy, double& angleX, double& angleY, double& angleZ);
  void CalculateTranslation(double dx, double dy, double& tx, double& ty, double& tz);
  void CalculateScale(double dx, double dy, double& sx, double& sy, double& sz);

private:
  // VTK组件
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkPropPicker> m_picker;
  vtkSmartPointer<vtkCamera> m_camera;

  // 场景管理
  SceneManagerV2* m_sceneManager = nullptr;
  void* _sceneDocument = nullptr; // csengine::SceneDocument* (void* to avoid header dep)
  TransformGizmo* m_transformGizmo = nullptr;

  // 交互状态
  InteractionMode m_interactionMode = InteractionMode::Camera;
  ViewType m_viewType = ViewType::Perspective;
  bool m_isManipulating = false;
  bool m_multiSelectionEnabled = true;
  bool m_isDragging = false;

  // 鼠标状态
  int m_startMousePos[2] = { 0, 0 };
  int m_currentMousePos[2] = { 0, 0 };
  int m_lastMousePos[2] = { 0, 0 };

  // 相机状态
  double m_startCameraPosition[3];
  double m_startFocalPoint[3];
  double m_startViewUp[3];
  double m_startParallelScale;
  double m_startDistance;

  // 操纵器状态
  int m_gizmoHandleType = -1;
  int m_gizmoAxisIndex = -1;
  bool m_isGizmoInteracting = false;
  double m_startTransform[16];

  // 选择状态
  vtkSmartPointer<vtkProp3D> m_pickedActor;
  QList<vtkSmartPointer<vtkProp3D>> m_selectedActors;
  QRectF m_selectionRect;

  // 测量状态
  QList<double*> m_measurePoints;
  bool m_measuring = false;

  // 约束
  bool m_constraintToAxis = false;
  int m_constraintAxis = -1; // 0=X, 1=Y, 2=Z
  bool m_constraintToPlane = false;
  double m_constraintPlaneNormal[3] = { 0, 0, 1 };

  // 设置
  double m_rotationSpeed = 1.0;
  double m_panSpeed = 1.0;
  double m_zoomSpeed = 1.0;
  double m_dollySpeed = 1.0;
  double m_mouseSensitivity = 1.0;

  // 工具提示
  QString m_statusText;
  QString m_toolTipText;
};

#endif // RS_INTERACTOR_V2_H