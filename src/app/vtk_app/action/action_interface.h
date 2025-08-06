#ifndef ACTION_INTERFACE_H
#define ACTION_INTERFACE_H

#include "base_type.h"

#include <QObject>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>

class ActionInterface : public QObject
{
    Q_OBJECT
public:
    ActionInterface(const char *name, vtkSmartPointer<vtkRenderer> vtk_render);

    virtual ~ActionInterface();

    void setName(const char *name);
    QString getName();

protected:
    QString _name;
    vtkSmartPointer<vtkRenderer> _vtk_render;
};

#endif // ACTION_INTERFACE_H