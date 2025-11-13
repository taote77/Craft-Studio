#ifndef RS_INTERACTOR_STYLE_H
#define RS_INTERACTOR_STYLE_H

#include <vtkBuffer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkProp3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTimeStamp.h>
#include <vtkTransform.h>

class vtkPropPicker;
class TransformGizmo;
class SceneManager;

class RSInteractorTrackCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static RSInteractorTrackCamera* New();

  vtkTypeMacro(RSInteractorTrackCamera, vtkInteractorStyleTrackballCamera);

  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);

  void SetvtkPropPicker(vtkSmartPointer<vtkPropPicker> picker);

  void SetSceneManager(SceneManager* manager);

  void SetTransformGizmo(TransformGizmo* gizmo);

  void SetMovePlane(vtkSmartPointer<vtkMatrix4x4> planeMatrix)
  {
    //
    m_planeMatrix = planeMatrix;
  }

  // 拾取操作
  vtkProp3D* PickActor(int x, int y);

  // 手柄拾取
  bool PickGizmoHandle(int x, int y, int& handleType, int& axisIndex);

protected:
  void OnLeftButtonDown() override;

  void OnMouseMove() override;

  void OnLeftButtonUp() override;

  void OnMouseWheelForward() override;

  void OnMouseWheelBackward() override;

  void OnKeyPress() override;

protected:
  RSInteractorTrackCamera();

  ~RSInteractorTrackCamera();

  // 平面平移
  void TranslatePlane(double dx, double dy);

  void RotatePlane(double dx, double dy);

  // 模型变换操作
  void StartModelTransform(int x, int y);
  void UpdateModelTransform(int x, int y);
  void EndModelTransform();

  // 手柄交互操作
  void StartGizmoInteraction(int x, int y);
  void UpdateGizmoInteraction(int x, int y);
  void EndGizmoInteraction();

  // 相机移动处理
  void HandleCameraMovement(int x, int y);

private:
  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkPropPicker> _picker;
  vtkSmartPointer<vtkProp3D> _picked_actor;
  int _startX, _startY;
  int _endX, _endY;

  //
  vtkSmartPointer<vtkMatrix4x4> m_planeMatrix;
  vtkSmartPointer<vtkMatrix4x4> m_initialMatrix;
  bool m_isDragging = false;
  int m_startPos[2];
  int m_endPos[2];

  // 新增：场景管理和变换操纵器
  SceneManager* m_sceneManager = nullptr;
  TransformGizmo* m_transformGizmo = nullptr;

  // 交互状态
  enum InteractionState
  {
    CAMERA_MODE,      // 相机控制模式
    OBJECT_SELECTION, // 对象选择模式
    GIZMO_INTERACTION // 操纵器交互模式
  };

  InteractionState m_interactionState = CAMERA_MODE;
  bool m_isGizmoInteracting = false;
  int m_gizmoHandleType = -1;  // 手柄类型：0-平移，1-旋转，2-缩放
  int m_gizmoAxisIndex = -1;   // 轴索引：0-X，1-Y，2-Z，3-均匀
  double m_startTransform[16]; // 初始变换矩阵
  int m_startMousePos[2];      // 鼠标起始位置
};

#endif // RS_INTERACTOR_STYLE_H
