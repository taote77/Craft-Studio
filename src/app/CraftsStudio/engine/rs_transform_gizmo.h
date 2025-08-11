#ifndef RS_TRANSFORM_GIZMO_H
#define RS_TRANSFORM_GIZMO_H

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

class TransformGizmo
{
public:
  enum GizmoMode
  {
    MODE_TRANSLATE,
    MODE_ROTATE,
    MODE_SCALE
  };

  TransformGizmo();
  void SetActor(vtkSmartPointer<vtkActor> actor);
  void UpdateTransform();

  // 事件处理
  static void OnLeftButtonDown(
    vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
  static void OnMouseMove(
    vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

private:
  // 轴线创建
  vtkSmartPointer<vtkActor> CreateAxis(
    const char* color, const double* origin, const double* point);

  // 变换计算
  void ApplyTranslation(double* displacement);
  void ApplyRotation(double* angleAxis);
  void ApplyScaling(double* scaleFactor);

  vtkSmartPointer<vtkActor> m_actor;
  vtkSmartPointer<vtkAxesActor> m_gizmoAxes;

  // 变换状态
  vtkSmartPointer<vtkMatrix4x4> m_baseMatrix;
  GizmoMode m_currentMode = MODE_TRANSLATE;
  double m_lastMousePos[2];
};

#endif // RS_TRANSFORM_GIZMO_H