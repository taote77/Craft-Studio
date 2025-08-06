#include "action_interface.h"

ActionInterface::ActionInterface(const char *name, vtkSmartPointer<vtkRenderer> vtk_render) :
    _name(name),
    _vtk_render(std::move(vtk_render))
{
}

ActionInterface::~ActionInterface()
{
}

void ActionInterface::setName(const char *name)
{
    this->_name = name;
}

QString ActionInterface::getName()
{
    return _name;
}