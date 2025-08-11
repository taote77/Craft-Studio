#ifndef RS_INTERACTOR_STYLE_H
#define RS_INTERACTOR_STYLE_H

#include <vtkBuffer.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkProp3D.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
class vtkPropPicker;

class RSInteractorStyle : public vtkInteractorStyleTrackballActor
{
public:
  static RSInteractorStyle* New();
  vtkTypeMacro(RSInteractorStyle, vtkInteractorStyleTrackballActor);

  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);

  void SetvtkPropPicker(vtkSmartPointer<vtkPropPicker> picker);

  void OnLeftButtonDown() override;

  void OnMouseMove() override;

  void OnLeftButtonUp() override;

  void OnMouseWheelForward() override;
  void OnMouseWheelBackward() override;

  void OnKeyPress() override;

protected:
  RSInteractorStyle();
  ~RSInteractorStyle();

private:
  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkPropPicker> _picker;
  vtkSmartPointer<vtkProp3D> _picked_actor;
  int _startX, _startY;
  int _endX, _endY;
};

#endif // RS_INTERACTOR_STYLE_H