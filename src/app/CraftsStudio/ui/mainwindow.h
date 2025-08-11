#ifndef MAINWINOW_H
#define MAINWINOW_H

#include "actionfactory.h"
#include "model_manager.h"
#include "project_tree.h"

#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPropPicker.h>
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
  void contextMenuEvent(QContextMenuEvent* event) override;

  void setupUI();

  void exit();

signals:

private:
  ActionFactory* _action_factory{ nullptr };

  ProjectTree* _project_tree;

  QWidget layoutContainer;

  QPointer<QVTKOpenGLNativeWidget> _vtkRenderWidget;

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };

  vtkSmartPointer<vtkRenderer> _vtkRenderer{ nullptr };

  std::mt19937 _rand_eng{ 0 };

  // 定义菜单
  QMenu* _right_button_menu{ nullptr }; // 右键菜单
};

#endif // MAINWINOW_H
