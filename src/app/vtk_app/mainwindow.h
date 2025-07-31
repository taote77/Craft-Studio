#ifndef MAINWINOW_H
#define MAINWINOW_H

#include "actionfactory.h"

// #include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <QVTKOpenGLNativeWidget.h>
#include <vtk-9.5/vtkSmartPointer.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <cmath>
#include <cstdlib>
#include <random>

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);

protected:
  void setupUI();

  void exit();

  void Randomize(vtkSphereSource* sphere, vtkDataSetMapper* mapper,
    vtkGenericOpenGLRenderWindow* window, std::mt19937& randEng);

signals:

private:
  ActionFactory* _action_factory{ nullptr };

  QDockWidget controlDock;

  QWidget layoutContainer;

  QPushButton randomizeButton;

  QPointer<QVBoxLayout> dockLayout;

  QPointer<QVTKOpenGLNativeWidget> _vtkRenderWidget;

  // vtkGenericOpenGLRenderWindow* _vtkRenderWindow{ nullptr };

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };

  vtkSmartPointer<vtkSphereSource> sphere{ nullptr };

  vtkSmartPointer<vtkDataSetMapper> mapper{ nullptr };
};

#endif // MAINWINOW_H
