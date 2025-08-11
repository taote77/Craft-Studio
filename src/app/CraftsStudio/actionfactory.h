#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <QObject>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class ActionFactory : public QObject
{
  Q_OBJECT
public:
  explicit ActionFactory(QObject* parent = nullptr);

  void setVtkRenderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow> vtkRenderWindow)
  {
    _vtkRenderWindow = vtkRenderWindow;
  }

  void setVtkRenderer(vtkSmartPointer<vtkRenderer> vtkRenderer) { _vtkRenderer = vtkRenderer; }

public slots:

  void exitApp();

  void addSphere();

  void addCylinder();

  void addCube();

  void addPlane();

  void addCone();

  ///

  void openSTLFile();

  void openOBJFile();

  void openGeneralModelFile();

  void clearScene();

signals:
  void sigExit();

private:
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };

  vtkSmartPointer<vtkRenderer> _vtkRenderer{ nullptr };
};

#endif // ACTIONFACTORY_H
