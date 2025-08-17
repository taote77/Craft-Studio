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
#include <vtkTransform.h>

class vtkPropPicker;

class RSInteractorTrackCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static RSInteractorTrackCamera* New();

  vtkTypeMacro(RSInteractorTrackCamera, vtkInteractorStyleTrackballCamera);

  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);

  void SetvtkPropPicker(vtkSmartPointer<vtkPropPicker> picker);

  void SetMovePlane(vtkSmartPointer<vtkMatrix4x4> planeMatrix)
  {
    //
    m_planeMatrix = planeMatrix;
  }

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
};

#endif // RS_INTERACTOR_STYLE_H