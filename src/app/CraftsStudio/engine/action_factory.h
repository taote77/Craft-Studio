#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <QObject>

#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;

class ActionFactory : public QObject
{
  Q_OBJECT
public:
  explicit ActionFactory(QObject* parent = nullptr);

  void setVtkRenderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow> vtkRenderWindow);

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

  void onConstructionFile();

  void onContourConstruct();

  void clearScene();

  /// method

signals:
  void sigExit();

private:
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };
};

#endif // ACTIONFACTORY_H
