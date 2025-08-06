#ifndef MAINWINOW_H
#define MAINWINOW_H

#include "actionfactory.h"

#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
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

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };

  vtkSmartPointer<vtkRenderer> _vtkRenderer{ nullptr };

  vtkSmartPointer<vtkSphereSource> _sphere{ nullptr };

  vtkSmartPointer<vtkDataSetMapper> _mapper{ nullptr };

  std::mt19937 _rand_eng{ 0 };
};

#endif // MAINWINOW_H
