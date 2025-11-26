#ifndef PLATER_WIDGET_H
#define PLATER_WIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVTKOpenGLNativeWidget.h>

#include <vtkCamera.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "engine/build_platform.h"
#include "engine/rs_interactor_v2.h"
#include "engine/rs_scene_manager_v2.h"
#include "engine/rs_scene_object_v2.h"

// 使用 rs_interactor_v2.h 中定义的 ViewType 枚举

// Plater工作台主界面类，负责3D打印前处理的主要功能
class PlaterWidget : public QVTKOpenGLNativeWidget
{
  Q_OBJECT

public:
  explicit PlaterWidget(QWidget* parent = nullptr, Qt::WindowFlags flag = Qt::WindowFlags());
  ~PlaterWidget();

  // 获取VTK渲染窗口
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> getRenderWindow() const { return m_renderWindow; }

  // 获取渲染器
  vtkSmartPointer<vtkRenderer> getRenderer() const { return m_renderer; }

  // 获取交互器
  vtkSmartPointer<RSInteractorV2> getInteractor() const { return m_interactor_style; }

  // 获取场景管理器
  SceneManagerV2* getSceneManager() const { return m_sceneManager; }

  // 获取构建平台
  BuildPlatform* getBuildPlatform() const { return m_buildPlatform; }

  // 设置构建平台可见性
  void setPlatformVisible(bool visible);

public slots:
  // 模型操作
  void addModel();
  void removeSelectedModels();
  void duplicateSelectedModels();

  // 模型变换
  void translateModel(double x, double y, double z);
  void rotateModel(double x, double y, double z);
  void scaleModel(double x, double y, double z);

  // 视图控制
  void setViewType(int type);
  void resetCamera();
  void fitAll();
  void fitSelected();

  // 布局操作
  void autoLayout();
  void arrangeInGrid();
  void centerModel();
  void layFlat();

  // TODO: PlaterWidget 完全实现后启用
  /*
  // 视图控制
  void setViewType(int type);
  void resetCamera();
  void fitAll();
  void fitSelected();
  */

  // 构建平台设置
  void updatePlatformSettings();

  // 显示选项
  void setShowGrid(bool show);
  void setShowAxes(bool show);
  void setShowRuler(bool show);
  void setShowPlatform(bool show);

  // 支撑生成
  void generateSupport();
  void removeSupport();

  // 打印预览
  void previewPrint();
  void generateGCode();

signals:
  // 模型变化信号
  void modelAdded();
  void modelRemoved();
  void modelSelected(SceneObjectV2* obj);
  void modelTransformed();

  // TODO: PlaterWidget 完全实现后启用
  /*
  // 视图变化信号
  void viewTypeChanged(int type);
  void cameraReset();
  void cameraFitAll();
  void cameraFitSelected();
  */

  // 构建平台变化信号
  void platformSettingsChanged();

  // 视图控制信号
  void viewTypeChanged(int type);
  void cameraReset();
  void cameraFitAll();
  void cameraFitSelected();
  void showPlatformChanged(bool show);

private slots:
  // 内部槽函数
  void onSelectionChanged(const QList<SceneObjectV2*>& selectedObjects);
  void onObjectGeometryChanged(SceneObjectV2* obj);
  void onPlatformPropertyChanged();

protected:
  // 键盘事件处理
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

private:
  void setupUI();
  void setupToolbar();
  void setupViewport();
  void setupRightPanel();
  void setupBottomPanel();
  void setupModelListTab();
  void setupTransformTab();
  void setupPlatformTab();
  void setupSupportTab();
  void setupPrintSettingsTab();

  void connectSignals();
  void updateUI();
  void updateModelInfo();

  // 视图控制辅助函数
  void saveCurrentView();
  void restoreSavedView();
  void toggleGridVisibility();
  void toggleAxesVisibility();
  void cycleViewTypes();

private:
  // 主布局
  QVBoxLayout* m_mainLayout;
  QHBoxLayout* m_contentLayout;
  QSplitter* m_mainSplitter;
  QSplitter* m_rightSplitter;

  // 工具栏
  QWidget* m_toolbarWidget;
  QHBoxLayout* m_toolbarLayout;
  QToolButton* m_addModelButton;
  QToolButton* m_removeModelButton;
  QToolButton* m_duplicateModelButton;
  QToolButton* m_autoLayoutButton;
  QToolButton* m_centerModelButton;
  QToolButton* m_layFlatButton;
  QToolButton* m_generateSupportButton;
  QToolButton* m_removeSupportButton;
  QComboBox* m_viewTypeCombo;
  QToolButton* m_resetCameraButton;
  QToolButton* m_fitAllButton;
  QToolButton* m_fitSelectedButton;

  // 3D视口
  QWidget* m_viewportWidget;
  QWidget* m_vtkWidget; // 临时使用 QWidget，后续可以根据Qt版本和VTK配置修改
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<RSInteractorV2> m_interactor_style;

  vtkSmartPointer<vtkOrientationMarkerWidget> _axes_widget{ nullptr };

  vtkSmartPointer<vtkCameraOrientationWidget> _cam_widget{ nullptr };

  // 右侧面板
  QWidget* m_rightPanelWidget;
  QTabWidget* m_rightTabWidget;

  // 模型列表标签页
  QWidget* m_modelListTab;
  QTableWidget* m_modelTable;

  // 变换标签页
  QWidget* m_transformTab;
  QGroupBox* m_positionGroup;
  QDoubleSpinBox* m_posXSpinBox;
  QDoubleSpinBox* m_posYSpinBox;
  QDoubleSpinBox* m_posZSpinBox;
  QToolButton* m_resetPositionButton;

  QGroupBox* m_rotationGroup;
  QDoubleSpinBox* m_rotXSpinBox;
  QDoubleSpinBox* m_rotYSpinBox;
  QDoubleSpinBox* m_rotZSpinBox;
  QToolButton* m_resetRotationButton;

  QGroupBox* m_scaleGroup;
  QDoubleSpinBox* m_scaleXSpinBox;
  QDoubleSpinBox* m_scaleYSpinBox;
  QDoubleSpinBox* m_scaleZSpinBox;
  QCheckBox* m_uniformScaleCheckBox;
  QToolButton* m_resetScaleButton;

  // 构建平台标签页
  QWidget* m_platformTab;
  QGroupBox* m_platformSizeGroup;
  QDoubleSpinBox* m_platformWidthSpinBox;
  QDoubleSpinBox* m_platformHeightSpinBox;
  QDoubleSpinBox* m_platformDepthSpinBox;

  QGroupBox* m_platformDisplayGroup;
  QCheckBox* m_showGridCheckBox;
  QCheckBox* m_showAxesCheckBox;
  QCheckBox* m_showRulerCheckBox;
  QCheckBox* m_showPlatformCheckBox;
  QDoubleSpinBox* m_gridSpacingSpinBox;

  // 支撑标签页
  QWidget* m_supportTab;
  QGroupBox* m_supportSettingsGroup;
  QCheckBox* m_autoSupportCheckBox;
  QDoubleSpinBox* m_supportDensitySpinBox;
  QDoubleSpinBox* m_supportZOffsetSpinBox;
  QCheckBox* m_supportPillarCheckBox;
  QDoubleSpinBox* m_pillarSizeSpinBox;
  QCheckBox* m_supportBaseCheckBox;
  QDoubleSpinBox* m_baseSizeSpinBox;
  QDoubleSpinBox* m_baseHeightSpinBox;

  // 打印设置标签页
  QWidget* m_printSettingsTab;
  QGroupBox* m_qualityGroup;
  QDoubleSpinBox* m_layerHeightSpinBox;
  QComboBox* m_qualityPresetCombo;

  QGroupBox* m_infillGroup;
  QDoubleSpinBox* m_infillDensitySpinBox;
  QComboBox* m_infillPatternCombo;

  QGroupBox* m_materialGroup;
  QComboBox* m_materialTypeCombo;
  QDoubleSpinBox* m_printTempSpinBox;
  QDoubleSpinBox* m_bedTempSpinBox;

  // 底部面板
  QWidget* m_bottomPanelWidget;
  QHBoxLayout* m_bottomPanelLayout;
  QLabel* m_statusLabel;
  QLabel* m_modelCountLabel;
  QLabel* m_printTimeLabel;
  QLabel* m_materialUsageLabel;
  QProgressBar* m_progressBar;

  // 核心组件
  BuildPlatform* m_buildPlatform;
  SceneManagerV2* m_sceneManager;

  // 视图控制状态
  struct SavedView
  {
    double cameraPosition[3];
    double focalPoint[3];
    double viewUp[3];
    double viewAngle;
    double parallelScale;
    bool parallelProjection;
    bool isSaved = false;
  } m_savedView;

  // 视图状态
  int m_currentViewType = 0; // 当前视图类型索引

  // 状态变量
  bool m_updatingUI = false;
  int m_selectedModelIndex = -1;
};

#endif // PLATER_WIDGET_H