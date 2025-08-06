#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <QObject>
#include <qchar.h>
#include <vtkSmartPointer.h>

#include "action_interface.h"

namespace GUI
{

class ActionHandler : public QObject
{
  Q_OBJECT

public:
  explicit ActionHandler(QObject* parent);

  void setVtkRenderer(vtkSmartPointer<vtkRenderer> vtk_renderer,
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> render_window);

  virtual ~ActionHandler() = default;

public slots:

  // [model]
  void slotAddPlane();

  void slotAddCone();

  void slotAddCylinder();
  // [model]

  // [file]
  void slotFileNewTemplate();

  void slotFileOpen();

  void slotFileSaveAs();

  // [file]

  // [image process]
  void slotImageProcess(const QString& path);

  void slotSliceSTL(const QString& stl_file);
  // [image process]

private:
  BaseType::ActionType _action_type;

  vtkSmartPointer<vtkRenderer> _vtk_renderer;

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _window;
};

} // namespace GUI

#endif // ACTION_HANDLER_H