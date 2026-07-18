#ifndef MAINWINOW_H
#define MAINWINOW_H

#include <QMainWindow>
#include <QPointer>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <QVTKOpenGLNativeWidget.h>

#include <random>

class QDockWidget;
class QLabel;
class QMenu;
class QPlainTextEdit;
class QQuickWidget;
class QTabWidget;
class PlaterWidget;
class MenuController;
class SlicePreviewBridge;

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

public slots:
  void exit();
  void onAddModel();
  void onRemoveModel();
  void onSlicingCompleted(int layerCount, const QString& gcodeText);
  void onSliceDataReady();
  void onImportFile();
  void onExportFile();
  void onAbout();
  void onHelp();
  void onNewProject();
  void onSaveProject();
  void onSaveAsProject();

private:
  // UI components
  QDockWidget* _left_dock = nullptr;
  QTabWidget* _centralTabWidget = nullptr;
  PlaterWidget* _platerWidget = nullptr;
  MenuController* _menuController = nullptr;

  // Status bar
  QLabel* _statusLabel = nullptr;
  QLabel* _modelCountLabel = nullptr;
  QLabel* _renderTimeLabel = nullptr;
  QLabel* _memoryUsageLabel = nullptr;

  // G-code preview
  QPlainTextEdit* _gcodePreview = nullptr;

  // 3D Slice preview (QtQuick3D)
  QQuickWidget* _slicePreview3D = nullptr;
  SlicePreviewBridge* _previewBridge = nullptr;

  // VTK
  QPointer<QVTKOpenGLNativeWidget> _vtkRenderWidget;
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> _vtkRenderWindow{ nullptr };
  vtkSmartPointer<vtkRenderer> _vtkRenderer{ nullptr };

  // State
  std::mt19937 _rand_eng{ 0 };
  QString _currentProjectFile;

  // Right-click context menu
  QMenu* _right_button_menu = nullptr;
};

#endif // MAINWINOW_H
