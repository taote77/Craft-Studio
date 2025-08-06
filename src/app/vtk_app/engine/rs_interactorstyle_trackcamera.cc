#include "rs_interactorstyle_trackcamera.h"

#include <vtkBuffer.h>

vtkStandardNewMacro(RSInteractorStyleTrackCamera);

void RSInteractorStyleTrackCamera::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  this->_renderer = renderer;
}
