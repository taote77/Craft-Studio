#include "rs_transform_gizmo.h"

TransformGizmo::TransformGizmo()
{
  // 创建轴线
  m_gizmoAxes = vtkSmartPointer<vtkAxesActor>::New();
  m_gizmoAxes->SetTotalLength(1.0, 1.0, 1.0);

  // 自定义轴属性
  vtkSmartPointer<vtkProperty> xAxisProp = vtkSmartPointer<vtkProperty>::New();
  xAxisProp->SetColor(1.0, 0.0, 0.0);
  xAxisProp->SetLineWidth(3.0);
  // m_gizmoAxes->GetXAxisActor()->SetProperty(xAxisProp);

  // 类似设置Y/Z轴属性...
}

vtkSmartPointer<vtkActor> TransformGizmo::CreateAxis(
  const char* color, const double* origin, const double* point)
{
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  // 构建轴线几何数据...

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(color[0], color[1], color[2]);
  return actor;
}

void TransformGizmo::OnLeftButtonDown(
  vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
  // 射线拾取选中轴
  // vtkSmartPointer<vtkProp3DPicker> picker = vtkSmartPointer<vtkProp3DPicker>::New();
  // picker->Pick(x, y, 0, renderer);

  // if (picker->GetProp3D() == m_gizmoAxes->GetXAxisActor())
  // {
  //   m_currentMode = MODE_TRANSLATE;
  // }
  // 其他轴处理...
}

void TransformGizmo::OnMouseMove(
  vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
  // if (!m_actor)
  //   return;

  // double dx = (pos[0] - m_lastMousePos[0]) * 0.01;
  // double dy = (pos[1] - m_lastMousePos[1]) * 0.01;

  // vtkSmartPointer<vtkMatrix4x4> transMat = vtkSmartPointer<vtkMatrix4x4>::New();
  // transMat->DeepCopy(m_baseMatrix);

  // switch (m_currentMode)
  // {
  //   case MODE_TRANSLATE:
  //     ApplyTranslation(transMat->GetElement(0, 3) + dx, transMat->GetElement(1, 3) + dy);
  //     break;
  //   case MODE_ROTATE:
  //     ApplyRotation(dx, dy);
  //     break;
  //     // 其他模式...
  // }

  // m_actor->SetUserMatrix(transMat);
  // renderer->Render();
}