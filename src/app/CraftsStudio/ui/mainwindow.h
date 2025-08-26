#ifndef MAINWINOW_H
#define MAINWINOW_H

#include "engine/action_factory.h"
#include "model_viewer.h"
#include "project_tree.h"

#include <QMainWindow>
#include <QPointer>

#include <vtkDataSetMapper.h>
#include <vtkSmartPointer.h>

class QDockWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);

protected:
  void setupUI();

  void exit();

signals:

private:
  ActionFactory* _action_factory{ nullptr };

  QDockWidget* _left_dock;

  ModelViewer* _model_viewer{ nullptr };
};

#endif // MAINWINOW_H
