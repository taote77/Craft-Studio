#ifndef RS_INTERACTOR_SWITCH_H
#define RS_INTERACTOR_SWITCH_H

#include <vtkInteractorStyleSwitch.h>
#include <vtkNew.h>
#include <vtkObject.h>       // 基础类
#include <vtkSmartPointer.h> // 智能指针支持

class RSInteractorSwitch : public vtkInteractorStyleSwitch
{
public:
  // static RSInteractorSwitch* New();

  vtkTypeMacro(RSInteractorSwitch, vtkInteractorStyleSwitch);

protected:
  RSInteractorSwitch();
  ~RSInteractorSwitch() override;

private:
};

#endif // RS_INTERACTOR_SWITCH_H