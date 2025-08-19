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

private:
  // vtkSmartPointer<vtkPropPicker> _picker;

  bool m_isDragging = false;
  int m_startPos[2];
  int m_endPos[2];
};

#endif // RS_INTERACTOR_STYLE_H