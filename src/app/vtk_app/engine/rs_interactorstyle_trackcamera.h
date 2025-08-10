#ifndef RS_INTERACTORSTYLE_TRACKCAMERA_H
#define RS_INTERACTORSTYLE_TRACKCAMERA_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class RSInteractorStyleTrackCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static RSInteractorStyleTrackCamera* New();
  vtkTypeMacro(RSInteractorStyleTrackCamera, vtkInteractorStyleTrackballCamera);

  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);

private:
  vtkSmartPointer<vtkRenderer> _renderer;
  int _startX, _startY;
  int _endX, _endY;
};

#endif // RS_INTERACTORSTYLE_TRACKCAMERA_H