#ifndef MAINWINOW_H
#define MAINWINOW_H

#include "../action_factory.h"
#include "plater_widget.h"
#include "project_tree.h"

#include <QMainWindow>
#include <QPointer>

class QContextMenuEvent;
class QDockWidget;
class QLabel;
class QMenu;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class QVTKOpenGLNativeWidget;
class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class QVTKOpenGLNativeWidget;
class QVTKOpenGLNativeWidget;

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "menu_manager.h"

#include <random>

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;

  void setupUI();
  void setupMenuBar();
  void setupStatusBar();
  void setupTabWidget();

  void exit();

signals:

private slots:
  void onAddModel();
  void onRemoveModel();
  void onImportFile();
  void onExportFile();
  void onAbout();
  void onHelp();
  void onNewProject();
  void onSaveProject();
  void onSaveAsProject();

private:
  // 菜单和工具栏
  void createFileMenu();
  void createEditMenu();
  void createViewMenu();
  void createToolsMenu();
  void createHelpMenu();

  // UI组件
  ActionFactory* _action_factory{ nullptr };

  QDockWidget* _left_dock;
  QTabWidget* _centralTabWidget;
  PlaterWidget* _platerWidget;

  // 状态栏组件
  QLabel* _statusLabel;
  QLabel* _modelCountLabel;
  QLabel* _renderTimeLabel;
  QLabel* _memoryUsageLabel;

  QPointer<QVTKOpenGLNativeWidget> _vtkRenderWidget;

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };

  vtkSmartPointer<vtkRenderer> _vtkRenderer{ nullptr };

  std::mt19937 _rand_eng{ 0 };

  MenuManager* _menu_mgr{ nullptr };

  // 定义菜单
  QMenu* _right_button_menu{ nullptr }; // 右键菜单

  // 菜单项
  QAction* _newAction;
  QAction* _openAction;
  QAction* _saveAction;
  QAction* _saveAsAction;
  QAction* _exportAction;
  QAction* _exitAction;

  QAction* _undoAction;
  QAction* _redoAction;
  QAction* _addAction;
  QAction* _removeAction;
  QAction* _duplicateAction;
  QAction* _selectAllAction;

  // 视图控制动作
  QAction* _resetViewAction;
  QAction* _fitViewAction;
  QAction* _topViewAction;
  QAction* _bottomViewAction;
  QAction* _frontViewAction;
  QAction* _leftViewAction;
  QAction* _rightViewAction;
  QAction* _perspectiveViewAction;

  // 工具动作
  QAction* _generateSupportAction;
  QAction* _removeSupportAction;
  QAction* _autoLayoutAction;
  QAction* _sliceAction;
  QAction* _previewAction;
  QAction* _exportGCodeAction;

  QAction* _aboutAction;
  QAction* _helpAction;

  // 成员变量
  QString _currentProjectFile;
  SceneManagerV2* _sceneManager = nullptr;
};

#endif // MAINWINOW_H
