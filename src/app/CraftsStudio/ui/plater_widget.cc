#include "plater_widget.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QSlider>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "engine/file_importer.h"

#include <csengine/scene/scene_document.hpp>
#include <csengine/scene/vtk_converter.hpp>
#include <slicingcore/mesh/model_object.hpp>

#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkCellArray.h>
#include <vtkCylinderSource.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

PlaterWidget::PlaterWidget(QWidget* parent, Qt::WindowFlags flag)
  : QVTKOpenGLNativeWidget(parent, flag)
  , m_buildPlatform(nullptr)
  , m_sceneDocument(nullptr)
  , m_slicingController(nullptr)
  , m_layerSlider(nullptr)
  , m_layerLabel(nullptr)
{
  // 创建核心组件
  m_buildPlatform = new BuildPlatform();
  m_sceneDocument = new csengine::SceneDocument(this);
  m_slicingController = new csbridge::SlicingController(this);

  // 设置UI
  setupUI();

  // 连接信号槽
  connectSignals();

  // 更新UI
  updateUI();
}

PlaterWidget::~PlaterWidget() = default;

void PlaterWidget::setupUI()
{
  // 设置主布局
  m_mainLayout = new QVBoxLayout(this);
  m_mainLayout->setContentsMargins(5, 5, 5, 5);
  m_mainLayout->setSpacing(5);

  // 创建工具栏
  setupToolbar();
  m_mainLayout->addWidget(m_toolbarWidget);

  // 创建主内容区域
  m_contentLayout = new QHBoxLayout();
  m_contentLayout->setContentsMargins(0, 0, 0, 0);
  m_contentLayout->setSpacing(5);

  // 创建主分割器
  m_mainSplitter = new QSplitter(Qt::Horizontal);
  m_contentLayout->addWidget(m_mainSplitter);

  // 创建3D视口
  setupViewport();
  m_mainSplitter->addWidget(m_viewportWidget);

  // 创建右侧面板
  setupRightPanel();
  m_mainSplitter->addWidget(m_rightPanelWidget);

  // 设置分割器比例
  m_mainSplitter->setStretchFactor(0, 3);
  m_mainSplitter->setStretchFactor(1, 1);

  m_mainLayout->addLayout(m_contentLayout);

  // 创建底部面板
  setupBottomPanel();
  m_mainLayout->addWidget(m_bottomPanelWidget);
}

void PlaterWidget::setupToolbar()
{
  // 创建工具栏部件
  m_toolbarWidget = new QWidget();
  m_toolbarWidget->setMaximumHeight(50);
  m_toolbarLayout = new QHBoxLayout(m_toolbarWidget);
  m_toolbarLayout->setContentsMargins(5, 5, 5, 5);
  m_toolbarLayout->setSpacing(5);

  // 模型操作按钮
  m_addModelButton = new QToolButton();
  m_addModelButton->setText("添加模型");
  m_addModelButton->setToolTip("添加3D模型到构建平台");
  m_addModelButton->setIcon(QIcon(":/icons/add_model.png"));
  m_addModelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_addModelButton);

  m_removeModelButton = new QToolButton();
  m_removeModelButton->setText("删除模型");
  m_removeModelButton->setToolTip("删除选中的模型");
  m_removeModelButton->setIcon(QIcon(":/icons/remove_model.png"));
  m_removeModelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_removeModelButton);

  m_duplicateModelButton = new QToolButton();
  m_duplicateModelButton->setText("复制模型");
  m_duplicateModelButton->setToolTip("复制选中的模型");
  m_duplicateModelButton->setIcon(QIcon(":/icons/duplicate_model.png"));
  m_duplicateModelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_duplicateModelButton);

  // 添加分隔符
  QFrame* line1 = new QFrame();
  line1->setFrameShape(QFrame::VLine);
  line1->setFrameShadow(QFrame::Sunken);
  m_toolbarLayout->addWidget(line1);

  // 布局操作按钮
  m_autoLayoutButton = new QToolButton();
  m_autoLayoutButton->setText("自动布局");
  m_autoLayoutButton->setToolTip("自动排列模型");
  m_autoLayoutButton->setIcon(QIcon(":/icons/auto_layout.png"));
  m_autoLayoutButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_autoLayoutButton);

  m_centerModelButton = new QToolButton();
  m_centerModelButton->setText("居中模型");
  m_centerModelButton->setToolTip("将模型居中放置");
  m_centerModelButton->setIcon(QIcon(":/icons/center_model.png"));
  m_centerModelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_centerModelButton);

  m_layFlatButton = new QToolButton();
  m_layFlatButton->setText("平放模型");
  m_layFlatButton->setToolTip("将模型平放在构建平台上");
  m_layFlatButton->setIcon(QIcon(":/icons/lay_flat.png"));
  m_layFlatButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_layFlatButton);

  // 添加分隔符
  QFrame* line2 = new QFrame();
  line2->setFrameShape(QFrame::VLine);
  line2->setFrameShadow(QFrame::Sunken);
  m_toolbarLayout->addWidget(line2);

  // 支撑操作按钮
  m_generateSupportButton = new QToolButton();
  m_generateSupportButton->setText("生成支撑");
  m_generateSupportButton->setToolTip("为模型生成支撑结构");
  m_generateSupportButton->setIcon(QIcon(":/icons/generate_support.png"));
  m_generateSupportButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_generateSupportButton);

  m_removeSupportButton = new QToolButton();
  m_removeSupportButton->setText("移除支撑");
  m_removeSupportButton->setToolTip("移除模型的所有支撑");
  m_removeSupportButton->setIcon(QIcon(":/icons/remove_support.png"));
  m_removeSupportButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  m_toolbarLayout->addWidget(m_removeSupportButton);

  // 添加弹性空间
  m_toolbarLayout->addStretch();

  // 视图控制
  m_viewTypeCombo = new QComboBox();
  m_viewTypeCombo->addItem("透视视图", static_cast<int>(ViewType::Perspective));
  m_viewTypeCombo->addItem("顶视图", static_cast<int>(ViewType::Top));
  m_viewTypeCombo->addItem("底视图", static_cast<int>(ViewType::Bottom));
  m_viewTypeCombo->addItem("前视图", static_cast<int>(ViewType::Front));
  m_viewTypeCombo->addItem("后视图", static_cast<int>(ViewType::Back));
  m_viewTypeCombo->addItem("左视图", static_cast<int>(ViewType::Left));
  m_viewTypeCombo->addItem("右视图", static_cast<int>(ViewType::Right));
  m_viewTypeCombo->addItem("正交视图", static_cast<int>(ViewType::Orthographic));
  m_viewTypeCombo->setToolTip("切换视图类型");
  m_toolbarLayout->addWidget(m_viewTypeCombo);

  m_resetCameraButton = new QToolButton();
  m_resetCameraButton->setText("重置视图");
  m_resetCameraButton->setToolTip("重置相机视图");
  m_resetCameraButton->setIcon(QIcon(":/icons/reset_camera.png"));
  m_toolbarLayout->addWidget(m_resetCameraButton);

  m_fitAllButton = new QToolButton();
  m_fitAllButton->setText("适配所有");
  m_fitAllButton->setToolTip("适配所有模型到视口");
  m_fitAllButton->setIcon(QIcon(":/icons/fit_all.png"));
  m_toolbarLayout->addWidget(m_fitAllButton);

  m_fitSelectedButton = new QToolButton();
  m_fitSelectedButton->setText("适配选中");
  m_fitSelectedButton->setToolTip("适配选中的模型到视口");
  m_fitSelectedButton->setIcon(QIcon(":/icons/fit_selected.png"));
  m_toolbarLayout->addWidget(m_fitSelectedButton);
}

void PlaterWidget::setupViewport()
{
  // 创建视口部件
  m_viewportWidget = new QWidget();
  QVBoxLayout* viewportLayout = new QVBoxLayout(m_viewportWidget);
  viewportLayout->setContentsMargins(0, 0, 0, 0);

  // 创建VTK窗口 (暂时使用QWidget替代，后续根据Qt版本调整)
  m_vtkWidget = new QWidget();
  viewportLayout->addWidget(m_vtkWidget);

  // 创建VTK渲染窗口
  m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  m_renderWindow->SetMultiSamples(8);
  m_renderWindow->SetAlphaBitPlanes(1);

  this->setRenderWindow(m_renderWindow);

  // 如果支持QVTKOpenGLNativeWidget，则使用真正的VTK窗口
  // 当前使用QWidget作为占位符，后续可以根据Qt版本调整

  // 创建渲染器
  m_renderer = vtkSmartPointer<vtkRenderer>::New();
  m_renderer->SetBackground(0.2, 0.2, 0.2);     // 深灰色背景
  m_renderer->SetBackground2(0.05, 0.05, 0.05); // 更深的背景色用于渐变
  m_renderer->SetGradientBackground(true);      // 启用渐变背景
  m_renderer->SetTwoSidedLighting(false);       // 关闭双面光照
  m_renderer->SetAutomaticLightCreation(true);  // 自动创建光源
  m_renderer->SetUseDepthPeeling(true);         // 启用深度剥离
  m_renderer->SetMaximumNumberOfPeels(10);      // 最大剥离层数
  m_renderer->SetOcclusionRatio(0.1);           // 遮挡比率

  m_renderWindow->AddRenderer(m_renderer);

  {
    auto axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetTotalLength(1, 1, 1); // 创建坐标轴标记窗口部件
    _axes_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    _axes_widget->SetOrientationMarker(axes);

    _cam_widget = vtkSmartPointer<vtkCameraOrientationWidget>::New();
  }

  // 创建交互器

  auto iterator = m_renderWindow->GetInteractor();
  iterator->RemoveAllObservers();

  m_interactor_style = vtkSmartPointer<RSInteractorV2>::New();
  m_interactor_style->SetRenderer(m_renderer);
  m_interactor_style->SetSceneDocument(m_sceneDocument);

  _axes_widget->SetInteractor(iterator);
  _axes_widget->SetEnabled(1);
  _axes_widget->InteractiveOff();

  if (!m_renderWindow->GetInteractor())
  {
    qDebug() << "Interactor is null";
  }

  iterator->SetInteractorStyle(m_interactor_style);

  _cam_widget->SetParentRenderer(m_renderer);
  _cam_widget->On();

  // 设置渲染窗口属性
  m_renderWindow->SetAlphaBitPlanes(1);
  m_renderWindow->SetMultiSamples(8);        // 开启多重采样抗锯齿
  m_renderWindow->SetLineSmoothing(true);    // 线条平滑
  m_renderWindow->SetPolygonSmoothing(true); // 多边形平滑

  // 初始化相机视图
  if (m_renderer->IsActiveCameraCreated())
  {
    vtkCamera* camera = m_renderer->GetActiveCamera();
    camera->SetViewAngle(30);             // 设置视角
    camera->SetPosition(0, 0, 400);       // 设置相机位置
    camera->SetFocalPoint(0, 0, 0);       // 设置焦点
    camera->SetViewUp(0, 1, 0);           // 设置相机上方向
    camera->SetParallelProjection(false); // 设置为透视投影
    camera->SetParallelScale(1.0);        // 设置平行投影比例
  }

  // 添加构建平台到渲染器
  if (m_buildPlatform)
  {
    m_buildPlatform->addToRenderer(m_renderer);
  }

  // 连接构建平台信号
  if (m_buildPlatform)
  {
    connect(m_buildPlatform, &BuildPlatform::platformUpdated, this,
      &PlaterWidget::onPlatformPropertyChanged);
  }

  // 将渲染器传递给场景管理器
  if (m_sceneDocument)
  {
    m_sceneDocument->setRenderer(m_renderer);
  }
}

void PlaterWidget::setupRightPanel()
{
  // 创建右侧面板部件
  m_rightPanelWidget = new QWidget();
  m_rightPanelWidget->setMaximumWidth(350);
  m_rightPanelWidget->setMinimumWidth(300);

  QVBoxLayout* rightPanelLayout = new QVBoxLayout(m_rightPanelWidget);
  rightPanelLayout->setContentsMargins(5, 5, 5, 5);
  rightPanelLayout->setSpacing(5);

  // 创建标签页
  m_rightTabWidget = new QTabWidget();
  m_rightTabWidget->setVisible(true); // 默认显示设置面板
  rightPanelLayout->addWidget(m_rightTabWidget);

  // 创建模型列表标签页
  setupModelListTab();

  // 创建变换标签页
  setupTransformTab();

  // 创建构建平台标签页
  setupPlatformTab();

  // 创建支撑标签页
  setupSupportTab();

  // 创建打印设置标签页
  setupPrintSettingsTab();
}

void PlaterWidget::setupModelListTab()
{
  // 创建模型列表标签页
  m_modelListTab = new QWidget();
  m_rightTabWidget->addTab(m_modelListTab, "模型");

  QVBoxLayout* modelListLayout = new QVBoxLayout(m_modelListTab);
  modelListLayout->setContentsMargins(5, 5, 5, 5);

  // 创建模型表格
  m_modelTable = new QTableWidget(0, 5);
  m_modelTable->setHorizontalHeaderLabels({ "名称", "大小", "体积", "重量", "打印时间" });
  m_modelTable->horizontalHeader()->setStretchLastSection(true);
  m_modelTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_modelTable->setSelectionMode(QAbstractItemView::SingleSelection);
  modelListLayout->addWidget(m_modelTable);
}

void PlaterWidget::setupTransformTab()
{
  // 创建变换标签页
  m_transformTab = new QWidget();
  m_rightTabWidget->addTab(m_transformTab, "变换");

  QVBoxLayout* transformLayout = new QVBoxLayout(m_transformTab);
  transformLayout->setContentsMargins(5, 5, 5, 5);
  transformLayout->setSpacing(10);

  // 位置组
  m_positionGroup = new QGroupBox("位置");
  transformLayout->addWidget(m_positionGroup);

  QGridLayout* positionLayout = new QGridLayout(m_positionGroup);

  positionLayout->addWidget(new QLabel("X:"), 0, 0);
  m_posXSpinBox = new QDoubleSpinBox();
  m_posXSpinBox->setRange(-1000, 1000);
  m_posXSpinBox->setSingleStep(1.0);
  m_posXSpinBox->setSuffix(" mm");
  positionLayout->addWidget(m_posXSpinBox, 0, 1);

  positionLayout->addWidget(new QLabel("Y:"), 1, 0);
  m_posYSpinBox = new QDoubleSpinBox();
  m_posYSpinBox->setRange(-1000, 1000);
  m_posYSpinBox->setSingleStep(1.0);
  m_posYSpinBox->setSuffix(" mm");
  positionLayout->addWidget(m_posYSpinBox, 1, 1);

  positionLayout->addWidget(new QLabel("Z:"), 2, 0);
  m_posZSpinBox = new QDoubleSpinBox();
  m_posZSpinBox->setRange(-1000, 1000);
  m_posZSpinBox->setSingleStep(1.0);
  m_posZSpinBox->setSuffix(" mm");
  positionLayout->addWidget(m_posZSpinBox, 2, 1);

  m_resetPositionButton = new QToolButton();
  m_resetPositionButton->setText("重置");
  positionLayout->addWidget(m_resetPositionButton, 3, 0, 1, 2);

  // 旋转组
  m_rotationGroup = new QGroupBox("旋转");
  transformLayout->addWidget(m_rotationGroup);

  QGridLayout* rotationLayout = new QGridLayout(m_rotationGroup);

  rotationLayout->addWidget(new QLabel("X:"), 0, 0);
  m_rotXSpinBox = new QDoubleSpinBox();
  m_rotXSpinBox->setRange(-360, 360);
  m_rotXSpinBox->setSingleStep(1.0);
  m_rotXSpinBox->setSuffix("°");
  rotationLayout->addWidget(m_rotXSpinBox, 0, 1);

  rotationLayout->addWidget(new QLabel("Y:"), 1, 0);
  m_rotYSpinBox = new QDoubleSpinBox();
  m_rotYSpinBox->setRange(-360, 360);
  m_rotYSpinBox->setSingleStep(1.0);
  m_rotYSpinBox->setSuffix("°");
  rotationLayout->addWidget(m_rotYSpinBox, 1, 1);

  rotationLayout->addWidget(new QLabel("Z:"), 2, 0);
  m_rotZSpinBox = new QDoubleSpinBox();
  m_rotZSpinBox->setRange(-360, 360);
  m_rotZSpinBox->setSingleStep(1.0);
  m_rotZSpinBox->setSuffix("°");
  rotationLayout->addWidget(m_rotZSpinBox, 2, 1);

  m_resetRotationButton = new QToolButton();
  m_resetRotationButton->setText("重置");
  rotationLayout->addWidget(m_resetRotationButton, 3, 0, 1, 2);

  // 缩放组
  m_scaleGroup = new QGroupBox("缩放");
  transformLayout->addWidget(m_scaleGroup);

  QGridLayout* scaleLayout = new QGridLayout(m_scaleGroup);

  m_uniformScaleCheckBox = new QCheckBox("均匀缩放");
  m_uniformScaleCheckBox->setChecked(true);
  scaleLayout->addWidget(m_uniformScaleCheckBox, 0, 0, 1, 2);

  scaleLayout->addWidget(new QLabel("X:"), 1, 0);
  m_scaleXSpinBox = new QDoubleSpinBox();
  m_scaleXSpinBox->setRange(0.01, 1000);
  m_scaleXSpinBox->setSingleStep(0.1);
  m_scaleXSpinBox->setValue(1.0);
  scaleLayout->addWidget(m_scaleXSpinBox, 1, 1);

  scaleLayout->addWidget(new QLabel("Y:"), 2, 0);
  m_scaleYSpinBox = new QDoubleSpinBox();
  m_scaleYSpinBox->setRange(0.01, 1000);
  m_scaleYSpinBox->setSingleStep(0.1);
  m_scaleYSpinBox->setValue(1.0);
  scaleLayout->addWidget(m_scaleYSpinBox, 2, 1);

  scaleLayout->addWidget(new QLabel("Z:"), 3, 0);
  m_scaleZSpinBox = new QDoubleSpinBox();
  m_scaleZSpinBox->setRange(0.01, 1000);
  m_scaleZSpinBox->setSingleStep(0.1);
  m_scaleZSpinBox->setValue(1.0);
  scaleLayout->addWidget(m_scaleZSpinBox, 3, 1);

  m_resetScaleButton = new QToolButton();
  m_resetScaleButton->setText("重置");
  scaleLayout->addWidget(m_resetScaleButton, 4, 0, 1, 2);

  transformLayout->addStretch();
}

void PlaterWidget::setupPlatformTab()
{
  // 创建构建平台标签页
  m_platformTab = new QWidget();
  m_rightTabWidget->addTab(m_platformTab, "构建平台");

  QVBoxLayout* platformLayout = new QVBoxLayout(m_platformTab);
  platformLayout->setContentsMargins(5, 5, 5, 5);
  platformLayout->setSpacing(10);

  // 平台尺寸组
  m_platformSizeGroup = new QGroupBox("平台尺寸");
  platformLayout->addWidget(m_platformSizeGroup);

  QGridLayout* sizeLayout = new QGridLayout(m_platformSizeGroup);

  sizeLayout->addWidget(new QLabel("宽度:"), 0, 0);
  m_platformWidthSpinBox = new QDoubleSpinBox();
  m_platformWidthSpinBox->setRange(50, 500);
  m_platformWidthSpinBox->setSingleStep(1.0);
  m_platformWidthSpinBox->setSuffix(" mm");
  m_platformWidthSpinBox->setValue(m_buildPlatform->width());
  sizeLayout->addWidget(m_platformWidthSpinBox, 0, 1);

  sizeLayout->addWidget(new QLabel("高度:"), 1, 0);
  m_platformHeightSpinBox = new QDoubleSpinBox();
  m_platformHeightSpinBox->setRange(50, 500);
  m_platformHeightSpinBox->setSingleStep(1.0);
  m_platformHeightSpinBox->setSuffix(" mm");
  m_platformHeightSpinBox->setValue(m_buildPlatform->height());
  sizeLayout->addWidget(m_platformHeightSpinBox, 1, 1);

  sizeLayout->addWidget(new QLabel("深度:"), 2, 0);
  m_platformDepthSpinBox = new QDoubleSpinBox();
  m_platformDepthSpinBox->setRange(0, 50);
  m_platformDepthSpinBox->setSingleStep(1.0);
  m_platformDepthSpinBox->setSuffix(" mm");
  m_platformDepthSpinBox->setValue(m_buildPlatform->depth());
  sizeLayout->addWidget(m_platformDepthSpinBox, 2, 1);

  // 显示选项组
  m_platformDisplayGroup = new QGroupBox("显示选项");
  platformLayout->addWidget(m_platformDisplayGroup);

  QGridLayout* displayLayout = new QGridLayout(m_platformDisplayGroup);

  m_showPlatformCheckBox = new QCheckBox("显示平台");
  m_showPlatformCheckBox->setChecked(true);
  displayLayout->addWidget(m_showPlatformCheckBox, 0, 0, 1, 2);

  m_showGridCheckBox = new QCheckBox("显示网格");
  m_showGridCheckBox->setChecked(true);
  displayLayout->addWidget(m_showGridCheckBox, 1, 0, 1, 2);

  displayLayout->addWidget(new QLabel("网格间距:"), 2, 0);
  m_gridSpacingSpinBox = new QDoubleSpinBox();
  m_gridSpacingSpinBox->setRange(1, 50);
  m_gridSpacingSpinBox->setSingleStep(1.0);
  m_gridSpacingSpinBox->setSuffix(" mm");
  m_gridSpacingSpinBox->setValue(m_buildPlatform->gridSpacing());
  displayLayout->addWidget(m_gridSpacingSpinBox, 2, 1);

  m_showAxesCheckBox = new QCheckBox("显示坐标轴");
  m_showAxesCheckBox->setChecked(true);
  displayLayout->addWidget(m_showAxesCheckBox, 3, 0, 1, 2);

  m_showRulerCheckBox = new QCheckBox("显示标尺");
  m_showRulerCheckBox->setChecked(true);
  displayLayout->addWidget(m_showRulerCheckBox, 4, 0, 1, 2);

  platformLayout->addStretch();
}

void PlaterWidget::setupSupportTab()
{
  // 创建支撑标签页
  m_supportTab = new QWidget();
  m_rightTabWidget->addTab(m_supportTab, "支撑");

  QVBoxLayout* supportLayout = new QVBoxLayout(m_supportTab);
  supportLayout->setContentsMargins(5, 5, 5, 5);
  supportLayout->setSpacing(10);

  // 支撑设置组
  m_supportSettingsGroup = new QGroupBox("支撑设置");
  supportLayout->addWidget(m_supportSettingsGroup);

  QGridLayout* supportSettingsLayout = new QGridLayout(m_supportSettingsGroup);

  m_autoSupportCheckBox = new QCheckBox("自动生成支撑");
  supportSettingsLayout->addWidget(m_autoSupportCheckBox, 0, 0, 1, 2);

  supportSettingsLayout->addWidget(new QLabel("支撑密度:"), 1, 0);
  m_supportDensitySpinBox = new QDoubleSpinBox();
  m_supportDensitySpinBox->setRange(0.05, 0.5);
  m_supportDensitySpinBox->setSingleStep(0.05);
  m_supportDensitySpinBox->setValue(0.15);
  supportSettingsLayout->addWidget(m_supportDensitySpinBox, 1, 1);

  supportSettingsLayout->addWidget(new QLabel("Z轴偏移:"), 2, 0);
  m_supportZOffsetSpinBox = new QDoubleSpinBox();
  m_supportZOffsetSpinBox->setRange(0.0, 1.0);
  m_supportZOffsetSpinBox->setSingleStep(0.05);
  m_supportZOffsetSpinBox->setValue(0.2);
  m_supportZOffsetSpinBox->setSuffix(" mm");
  supportSettingsLayout->addWidget(m_supportZOffsetSpinBox, 2, 1);

  m_supportPillarCheckBox = new QCheckBox("启用支柱");
  m_supportPillarCheckBox->setChecked(true);
  supportSettingsLayout->addWidget(m_supportPillarCheckBox, 3, 0, 1, 2);

  supportSettingsLayout->addWidget(new QLabel("支柱尺寸:"), 4, 0);
  m_pillarSizeSpinBox = new QDoubleSpinBox();
  m_pillarSizeSpinBox->setRange(0.2, 2.0);
  m_pillarSizeSpinBox->setSingleStep(0.1);
  m_pillarSizeSpinBox->setValue(0.8);
  m_pillarSizeSpinBox->setSuffix(" mm");
  supportSettingsLayout->addWidget(m_pillarSizeSpinBox, 4, 1);

  m_supportBaseCheckBox = new QCheckBox("启用底座");
  m_supportBaseCheckBox->setChecked(true);
  supportSettingsLayout->addWidget(m_supportBaseCheckBox, 5, 0, 1, 2);

  supportSettingsLayout->addWidget(new QLabel("底座尺寸:"), 6, 0);
  m_baseSizeSpinBox = new QDoubleSpinBox();
  m_baseSizeSpinBox->setRange(2.0, 10.0);
  m_baseSizeSpinBox->setSingleStep(0.5);
  m_baseSizeSpinBox->setValue(5.0);
  m_baseSizeSpinBox->setSuffix(" mm");
  supportSettingsLayout->addWidget(m_baseSizeSpinBox, 6, 1);

  supportSettingsLayout->addWidget(new QLabel("底座高度:"), 7, 0);
  m_baseHeightSpinBox = new QDoubleSpinBox();
  m_baseHeightSpinBox->setRange(0.5, 2.0);
  m_baseHeightSpinBox->setSingleStep(0.1);
  m_baseHeightSpinBox->setValue(1.0);
  m_baseHeightSpinBox->setSuffix(" mm");
  supportSettingsLayout->addWidget(m_baseHeightSpinBox, 7, 1);

  supportLayout->addStretch();
}

void PlaterWidget::setupPrintSettingsTab()
{
  // 创建打印设置标签页
  m_printSettingsTab = new QWidget();
  m_rightTabWidget->addTab(m_printSettingsTab, "打印设置");

  QVBoxLayout* printSettingsLayout = new QVBoxLayout(m_printSettingsTab);
  printSettingsLayout->setContentsMargins(5, 5, 5, 5);
  printSettingsLayout->setSpacing(10);

  // 质量组
  m_qualityGroup = new QGroupBox("质量");
  printSettingsLayout->addWidget(m_qualityGroup);

  QGridLayout* qualityLayout = new QGridLayout(m_qualityGroup);

  qualityLayout->addWidget(new QLabel("层高:"), 0, 0);
  m_layerHeightSpinBox = new QDoubleSpinBox();
  m_layerHeightSpinBox->setRange(0.05, 0.5);
  m_layerHeightSpinBox->setSingleStep(0.05);
  m_layerHeightSpinBox->setValue(0.1);
  m_layerHeightSpinBox->setSuffix(" mm");
  qualityLayout->addWidget(m_layerHeightSpinBox, 0, 1);

  qualityLayout->addWidget(new QLabel("质量预设:"), 1, 0);
  m_qualityPresetCombo = new QComboBox();
  m_qualityPresetCombo->addItems({ "超高质量", "高质量", "标准质量", "高速度", "超高速" });
  m_qualityPresetCombo->setCurrentIndex(2); // 标准质量
  qualityLayout->addWidget(m_qualityPresetCombo, 1, 1);

  // 填充组
  m_infillGroup = new QGroupBox("填充");
  printSettingsLayout->addWidget(m_infillGroup);

  QGridLayout* infillLayout = new QGridLayout(m_infillGroup);

  infillLayout->addWidget(new QLabel("填充密度:"), 0, 0);
  m_infillDensitySpinBox = new QDoubleSpinBox();
  m_infillDensitySpinBox->setRange(0.0, 1.0);
  m_infillDensitySpinBox->setSingleStep(0.1);
  m_infillDensitySpinBox->setValue(0.2);
  infillLayout->addWidget(m_infillDensitySpinBox, 0, 1);

  infillLayout->addWidget(new QLabel("填充图案:"), 1, 0);
  m_infillPatternCombo = new QComboBox();
  m_infillPatternCombo->addItems({ "网格", "蜂窝", "回旋", "立方体", "线性", "同心" });
  m_infillPatternCombo->setCurrentIndex(0); // 网格
  infillLayout->addWidget(m_infillPatternCombo, 1, 1);

  // 材料组
  m_materialGroup = new QGroupBox("材料");
  printSettingsLayout->addWidget(m_materialGroup);

  QGridLayout* materialLayout = new QGridLayout(m_materialGroup);

  materialLayout->addWidget(new QLabel("材料类型:"), 0, 0);
  m_materialTypeCombo = new QComboBox();
  m_materialTypeCombo->addItems({ "PLA", "ABS", "PETG", "TPU", "树脂" });
  m_materialTypeCombo->setCurrentIndex(0); // PLA
  materialLayout->addWidget(m_materialTypeCombo, 0, 1);

  materialLayout->addWidget(new QLabel("打印温度:"), 1, 0);
  m_printTempSpinBox = new QDoubleSpinBox();
  m_printTempSpinBox->setRange(150, 300);
  m_printTempSpinBox->setSingleStep(5);
  m_printTempSpinBox->setValue(210);
  m_printTempSpinBox->setSuffix(" °C");
  materialLayout->addWidget(m_printTempSpinBox, 1, 1);

  materialLayout->addWidget(new QLabel("热床温度:"), 2, 0);
  m_bedTempSpinBox = new QDoubleSpinBox();
  m_bedTempSpinBox->setRange(0, 120);
  m_bedTempSpinBox->setSingleStep(5);
  m_bedTempSpinBox->setValue(60);
  m_bedTempSpinBox->setSuffix(" °C");
  materialLayout->addWidget(m_bedTempSpinBox, 2, 1);

  printSettingsLayout->addStretch();
}

void PlaterWidget::setupBottomPanel()
{
  // 创建底部面板部件
  m_bottomPanelWidget = new QWidget();
  m_bottomPanelWidget->setMaximumHeight(80);
  m_bottomPanelWidget->setMinimumHeight(60);

  m_bottomPanelLayout = new QHBoxLayout(m_bottomPanelWidget);
  m_bottomPanelLayout->setContentsMargins(5, 5, 5, 5);

  // 状态标签
  m_statusLabel = new QLabel("就绪");
  m_statusLabel->setMinimumWidth(150);
  m_bottomPanelLayout->addWidget(m_statusLabel);

  // 模型数量标签
  m_modelCountLabel = new QLabel("模型数量: 0");
  m_modelCountLabel->setMinimumWidth(100);
  m_bottomPanelLayout->addWidget(m_modelCountLabel);

  // 打印时间标签
  m_printTimeLabel = new QLabel("预估时间: 0h 0m");
  m_printTimeLabel->setMinimumWidth(120);
  m_bottomPanelLayout->addWidget(m_printTimeLabel);

  // 材料使用量标签
  m_materialUsageLabel = new QLabel("材料用量: 0g");
  m_materialUsageLabel->setMinimumWidth(100);
  m_bottomPanelLayout->addWidget(m_materialUsageLabel);

  // 进度条
  m_progressBar = new QProgressBar();
  m_progressBar->setVisible(false);
  m_bottomPanelLayout->addWidget(m_progressBar);

  // 层级预览滑块
  m_layerLabel = new QLabel("层: -/-");
  m_layerLabel->setMinimumWidth(80);
  m_bottomPanelLayout->addWidget(m_layerLabel);

  m_layerSlider = new QSlider(Qt::Horizontal);
  m_layerSlider->setMinimum(0);
  m_layerSlider->setMaximum(0);
  m_layerSlider->setVisible(false);
  m_layerSlider->setMinimumWidth(150);
  connect(m_layerSlider, &QSlider::valueChanged, this, &PlaterWidget::updateLayerPreview);
  m_bottomPanelLayout->addWidget(m_layerSlider);

  // 添加弹性空间
  m_bottomPanelLayout->addStretch();
}

void PlaterWidget::connectSignals()
{
  // 工具栏按钮连接
  if (m_addModelButton)
    connect(m_addModelButton, &QToolButton::clicked, this, &PlaterWidget::addModel);
  if (m_removeModelButton)
    connect(m_removeModelButton, &QToolButton::clicked, this, &PlaterWidget::removeSelectedModels);
  if (m_duplicateModelButton)
    connect(
      m_duplicateModelButton, &QToolButton::clicked, this, &PlaterWidget::duplicateSelectedModels);
  if (m_autoLayoutButton)
    connect(m_autoLayoutButton, &QToolButton::clicked, this, &PlaterWidget::autoLayout);
  if (m_centerModelButton)
    connect(m_centerModelButton, &QToolButton::clicked, this, &PlaterWidget::centerModel);
  if (m_layFlatButton)
    connect(m_layFlatButton, &QToolButton::clicked, this, &PlaterWidget::layFlat);
  if (m_generateSupportButton)
    connect(m_generateSupportButton, &QToolButton::clicked, this, &PlaterWidget::generateSupport);
  if (m_removeSupportButton)
    connect(m_removeSupportButton, &QToolButton::clicked, this, &PlaterWidget::removeSupport);

  // 视图控制连接
  if (m_viewTypeCombo)
    connect(m_viewTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
      &PlaterWidget::setViewType);
  if (m_resetCameraButton)
    connect(m_resetCameraButton, &QToolButton::clicked, this, &PlaterWidget::resetCamera);
  if (m_fitAllButton)
    connect(m_fitAllButton, &QToolButton::clicked, this, &PlaterWidget::fitAll);
  if (m_fitSelectedButton)
    connect(m_fitSelectedButton, &QToolButton::clicked, this, &PlaterWidget::fitSelected);

  // 变换控制连接
  if (m_posXSpinBox)
    connect(m_posXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
      [this](double value)
      { translateModel(value, m_posYSpinBox->value(), m_posZSpinBox->value()); });
  if (m_posYSpinBox)
    connect(m_posYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
      [this](double value)
      { translateModel(m_posXSpinBox->value(), value, m_posZSpinBox->value()); });
  if (m_posZSpinBox)
    connect(m_posZSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
      [this](double value)
      { translateModel(m_posXSpinBox->value(), m_posYSpinBox->value(), value); });

  connect(m_rotXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    [this](double value) { rotateModel(value, m_rotYSpinBox->value(), m_rotZSpinBox->value()); });
  connect(m_rotYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    [this](double value) { rotateModel(m_rotXSpinBox->value(), value, m_rotZSpinBox->value()); });
  connect(m_rotZSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    [this](double value) { rotateModel(m_rotXSpinBox->value(), m_rotYSpinBox->value(), value); });

  connect(m_scaleXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    [this](double value)
    {
      if (m_uniformScaleCheckBox->isChecked())
      {
        m_scaleYSpinBox->blockSignals(true);
        m_scaleZSpinBox->blockSignals(true);
        m_scaleYSpinBox->setValue(value);
        m_scaleZSpinBox->setValue(value);
        m_scaleYSpinBox->blockSignals(false);
        m_scaleZSpinBox->blockSignals(false);
      }
      scaleModel(value, m_scaleYSpinBox->value(), m_scaleZSpinBox->value());
    });
  connect(m_scaleYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    [this](double value)
    {
      if (m_uniformScaleCheckBox->isChecked())
      {
        m_scaleXSpinBox->blockSignals(true);
        m_scaleZSpinBox->blockSignals(true);
        m_scaleXSpinBox->setValue(value);
        m_scaleZSpinBox->setValue(value);
        m_scaleXSpinBox->blockSignals(false);
        m_scaleZSpinBox->blockSignals(false);
      }
      scaleModel(m_scaleXSpinBox->value(), value, m_scaleZSpinBox->value());
    });
  connect(m_scaleZSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    [this](double value)
    {
      if (m_uniformScaleCheckBox->isChecked())
      {
        m_scaleXSpinBox->blockSignals(true);
        m_scaleYSpinBox->blockSignals(true);
        m_scaleXSpinBox->setValue(value);
        m_scaleYSpinBox->setValue(value);
        m_scaleXSpinBox->blockSignals(false);
        m_scaleYSpinBox->blockSignals(false);
      }
      scaleModel(m_scaleXSpinBox->value(), m_scaleYSpinBox->value(), value);
    });

  // 重置按钮连接
  connect(m_resetPositionButton, &QToolButton::clicked,
    [this]()
    {
      m_posXSpinBox->setValue(0.0);
      m_posYSpinBox->setValue(0.0);
      m_posZSpinBox->setValue(0.0);
    });

  connect(m_resetRotationButton, &QToolButton::clicked,
    [this]()
    {
      m_rotXSpinBox->setValue(0.0);
      m_rotYSpinBox->setValue(0.0);
      m_rotZSpinBox->setValue(0.0);
    });

  connect(m_resetScaleButton, &QToolButton::clicked,
    [this]()
    {
      m_scaleXSpinBox->setValue(1.0);
      m_scaleYSpinBox->setValue(1.0);
      m_scaleZSpinBox->setValue(1.0);
    });

  // 构建平台设置连接
  if (m_platformWidthSpinBox)
    connect(m_platformWidthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
      &PlaterWidget::updatePlatformSettings);
  if (m_platformHeightSpinBox)
    connect(m_platformHeightSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
      &PlaterWidget::updatePlatformSettings);
  if (m_platformDepthSpinBox)
    connect(m_platformDepthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
      &PlaterWidget::updatePlatformSettings);

  if (m_showPlatformCheckBox)
    connect(m_showPlatformCheckBox, &QCheckBox::toggled, this, &PlaterWidget::setShowPlatform);
  if (m_showGridCheckBox)
    connect(m_showGridCheckBox, &QCheckBox::toggled, this, &PlaterWidget::setShowGrid);
  if (m_showAxesCheckBox)
    connect(m_showAxesCheckBox, &QCheckBox::toggled, this, &PlaterWidget::setShowAxes);
  if (m_showRulerCheckBox)
    connect(m_showRulerCheckBox, &QCheckBox::toggled, this, &PlaterWidget::setShowRuler);
  if (m_gridSpacingSpinBox)
    connect(m_gridSpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
      &PlaterWidget::updatePlatformSettings);

  // Support tab — enable/disable child controls based on checkboxes
  connect(m_supportPillarCheckBox, &QCheckBox::toggled,
          m_pillarSizeSpinBox, &QDoubleSpinBox::setEnabled);
  connect(m_supportBaseCheckBox, &QCheckBox::toggled,
          m_baseSizeSpinBox, &QDoubleSpinBox::setEnabled);
  connect(m_supportBaseCheckBox, &QCheckBox::toggled,
          m_baseHeightSpinBox, &QDoubleSpinBox::setEnabled);
  // Initialize enabled state
  m_pillarSizeSpinBox->setEnabled(m_supportPillarCheckBox->isChecked());
  m_baseSizeSpinBox->setEnabled(m_supportBaseCheckBox->isChecked());
  m_baseHeightSpinBox->setEnabled(m_supportBaseCheckBox->isChecked());

  // Print settings — quality preset updates layer height
  connect(m_qualityPresetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) {
    // Preset values (mm): Ultra High=0.05, High=0.1, Standard=0.2, Fast=0.3, Ultra Fast=0.4
    const double presets[] = { 0.05, 0.1, 0.2, 0.3, 0.4 };
    if (index >= 0 && index < static_cast<int>(sizeof(presets) / sizeof(presets[0])))
      m_layerHeightSpinBox->setValue(presets[index]);
  });

  // Print settings — material preset updates temperatures
  connect(m_materialTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) {
    // PLA=0, ABS=1, PETG=2, TPU=3, Resin=4
    struct MaterialPreset { double nozzle; double bed; };
    const MaterialPreset presets[] = {
      { 210, 60 },  // PLA
      { 245, 90 },  // ABS
      { 235, 75 },  // PETG
      { 220, 50 },  // TPU
      { 25,  0  },  // Resin (not applicable)
    };
    if (index >= 0 && index < static_cast<int>(sizeof(presets) / sizeof(presets[0])))
    {
      m_printTempSpinBox->setValue(presets[index].nozzle);
      m_bedTempSpinBox->setValue(presets[index].bed);
    }
  });

  // 场景管理器信号连接
  connect(m_sceneDocument, &csengine::SceneDocument::selectionChanged, this,
    &PlaterWidget::onSelectionChanged);
  connect(m_sceneDocument, &csengine::SceneDocument::objectModified, this,
    &PlaterWidget::onObjectModified);

  // Model table selection → 3D viewport selection
  connect(m_modelTable, &QTableWidget::cellClicked,
          this, [this](int row, int /*col*/) {
    auto objects = m_sceneDocument->objects();
    if (row >= 0 && row < objects.size())
    {
      m_sceneDocument->setSelected(objects[row]);
      if (m_renderWindow)
        m_renderWindow->Render();
    }
  });

  // 切片控制器信号连接
  connect(m_slicingController, &csbridge::SlicingController::progressChanged,
          this, [this](int percent) {
    if (m_progressBar)
    {
      m_progressBar->setVisible(percent >= 0 && percent < 100);
      m_progressBar->setValue(percent);
    }
  });
  connect(m_slicingController, &csbridge::SlicingController::logMessage,
          this, [this](const QString& msg) {
    if (m_statusLabel) m_statusLabel->setText(msg);
  });
  connect(m_slicingController, &csbridge::SlicingController::sliceFinished,
          this, [this](bool success, const QString& error) {
    if (m_progressBar) m_progressBar->setVisible(false);
    if (success)
    {
      auto* pipeline = m_slicingController->lastPipeline();
      int layers = pipeline ? pipeline->totalLayerCount() : 0;
      QString gcode = pipeline ? pipeline->gcodeText() : QString();
      m_statusLabel->setText(QString("切片完成: %1 层").arg(layers));

      // Estimate print time from path length
      if (pipeline && layers > 0)
      {
        double totalPathMm = 0;
        for (const auto& layer : pipeline->layers())
          for (const auto& expoly : layer.slices())
            for (size_t i = 0; i < expoly.contour.size(); ++i)
            {
              const auto& a = expoly.contour[i];
              const auto& b = expoly.contour[(i+1) % expoly.contour.size()];
              totalPathMm += a.dist(b) / 1000.0; // microns → mm
            }
        double speed = 60.0; // mm/s default
        double printTimeMin = totalPathMm / speed / 60.0;
        int hours = static_cast<int>(printTimeMin / 60);
        int mins = static_cast<int>(printTimeMin) % 60;
        m_printTimeLabel->setText(
          QString("预估: %1h %2m").arg(hours).arg(mins, 2, 10, QChar('0')));
      }

      // Configure layer preview slider
      if (m_layerSlider && layers > 0)
      {
        m_layerSlider->setMaximum(layers - 1);
        m_layerSlider->setValue(0);
        m_layerSlider->setVisible(true);
        if (m_layerLabel)
          m_layerLabel->setText(QString("层: 0/%1").arg(layers));
      }

      // Emit signal for UI updates (G-code preview)
      emit slicingCompleted(layers, gcode);

      // Save G-code to file
      if (!gcode.isEmpty())
      {
        QString path = QFileDialog::getSaveFileName(
          this, "保存G-code", "output.gcode", "G-code Files (*.gcode *.gco)");
        if (!path.isEmpty())
        {
          QFile file(path);
          if (file.open(QIODevice::WriteOnly | QIODevice::Text))
          {
            file.write(pipeline->gcodeText().toUtf8());
            file.close();
            m_statusLabel->setText(QString("G-code 已保存: %1").arg(path));
          }
        }
      }
    }
    else
    {
      m_statusLabel->setText("切片失败: " + error);
    }
  });
}

void PlaterWidget::updateUI()
{
  // 更新UI状态
  bool hasSelection = !m_sceneDocument->selection().isEmpty();

  // 更新工具栏按钮状态
  m_removeModelButton->setEnabled(hasSelection);
  m_duplicateModelButton->setEnabled(hasSelection);
  m_centerModelButton->setEnabled(hasSelection);
  m_layFlatButton->setEnabled(hasSelection);
  m_generateSupportButton->setEnabled(hasSelection);
  m_removeSupportButton->setEnabled(hasSelection);
  m_fitSelectedButton->setEnabled(hasSelection);

  // 更新变换面板
  m_positionGroup->setEnabled(hasSelection);
  m_rotationGroup->setEnabled(hasSelection);
  m_scaleGroup->setEnabled(hasSelection);

  // 更新模型信息
  updateModelInfo();
}

void PlaterWidget::updateModelInfo()
{
  // 更新模型表格
  m_modelTable->setRowCount(0);

  int row = 0;
  for (auto obj : m_sceneDocument->objects())
  {
    m_modelTable->insertRow(row);

    // 名称
    m_modelTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(obj->name())));

    // 尺寸 (from bounding box)
    double sx, sy, sz, dummy;
    obj->worldBounds(dummy, dummy, dummy, sx, sy, sz);
    QVector3D size(sx, sy, sz);
    m_modelTable->setItem(row, 1,
      new QTableWidgetItem(
        QString("%1x%2x%3").arg(size.x(), 0, 'f', 1).arg(size.y(), 0, 'f', 1).arg(size.z(), 0, 'f', 1)));

    // 体积 (from ImportResult, fallback to tetrahedral)
    double volume = obj->importVolumeMm3() > 0 ? obj->importVolumeMm3() : obj->totalVolumeMm3();
    m_modelTable->setItem(row, 2, new QTableWidgetItem(QString("%1 cm³").arg(volume / 1000.0, 0, 'f', 2)));

    // 重量 (from ImportResult, fallback to computed)
    double weight = obj->importWeightG() > 0 ? obj->importWeightG() : volume / 1000.0 * 1.24;
    m_modelTable->setItem(row, 3, new QTableWidgetItem(QString("%1 g").arg(weight, 0, 'f', 2)));

    // 打印时间 - 简化计算（基于体积和层高）
    double layerHeight = m_layerHeightSpinBox ? m_layerHeightSpinBox->value() : 0.1;
    double estimatedTime = 0.0;
    if (volume > 0.0 && layerHeight > 0.0)
    {
      estimatedTime = (volume / 1000.0) / (layerHeight * 100.0) * 60.0; // 简化估算
    }
    int hours = static_cast<int>(estimatedTime) / 3600;
    int minutes = (static_cast<int>(estimatedTime) % 3600) / 60;
    m_modelTable->setItem(row, 4, new QTableWidgetItem(QString("%1h %2m").arg(hours).arg(minutes)));

    row++;
  }

  // 更新底部状态栏
  int modelCount = m_sceneDocument->objects().count();
  m_modelCountLabel->setText(QString("模型数量: %1").arg(modelCount));

  // 计算总重量和总打印时间
  double totalWeight = 0.0;
  double totalTime = 0.0;

  for (auto obj : m_sceneDocument->objects())
  {
    double volume = obj->importVolumeMm3() > 0 ? obj->importVolumeMm3() : obj->totalVolumeMm3();
    double weight = obj->importWeightG() > 0 ? obj->importWeightG() : volume / 1000.0 * 1.24;
    totalWeight += weight;
    double layerHeight = m_layerHeightSpinBox ? m_layerHeightSpinBox->value() : 0.1;
    if (volume > 0.0 && layerHeight > 0.0)
    {
      totalTime += (volume / 1000.0) / (layerHeight * 100.0) * 60.0;
    }
  }

  m_materialUsageLabel->setText(QString("材料用量: %1g").arg(totalWeight, 0, 'f', 2));
  
  int totalHours = static_cast<int>(totalTime) / 3600;
  int totalMinutes = (static_cast<int>(totalTime) % 3600) / 60;
  m_printTimeLabel->setText(QString("预估时间: %1h %2m")
                              .arg(totalHours)
                              .arg(totalMinutes));
}

// 实现公共槽函数
void PlaterWidget::addModel()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, "添加3D模型", "", FileImporter::getFileFilter());

  if (fileName.isEmpty())
  {
    return;
  }

  // 显示进度提示
  m_statusLabel->setText("正在导入模型...");
  QApplication::processEvents();

  // 创建文件导入器
  FileImporter importer;

  // 显示导入选项对话框
  ImportOptions options = FileImporter::showImportOptionsDialog(fileName, this);

  // 导入文件
  ImportResult result = importer.importFile(fileName, options);

  if (result.success)
  {
    // Convert VTK data to engine mesh (for slicing)
    slicing::TriangleMesh mesh = csengine::triangleMeshFromVTK(result.polyData);
    // Auto-repair mesh
    mesh = csengine::repairMesh(mesh);
    QString name = QFileInfo(fileName).baseName();

    // Add model: use VTK polydata directly for rendering (avoids re-conversion)
    slicing::ModelObject* obj = m_sceneDocument->addModelWithVTK(
        std::move(mesh), result.polyData, name);

    // Preserve import metadata
    obj->setMeshInfo(result.meshInfo.toStdString());
    obj->setImportVolume(result.volume);
    obj->setImportWeight(result.weight);

    // Set renderer on document if not already set
    if (m_renderer)
    {
      m_sceneDocument->setRenderer(m_renderer);
      m_sceneDocument->syncToRenderer();
    }

    // Auto-center model on build plate
    if (m_buildPlatform && obj)
    {
      double bw = m_buildPlatform->width();
      double bh = m_buildPlatform->height();
      double cx = bw / 2.0, cy = bh / 2.0;
      obj->setPosition(cx, cy, 0);
      m_sceneDocument->syncToRenderer();
    }

    // Trigger render
    if (m_renderWindow)
    {
      m_renderWindow->Render();
    }

    // Update UI
    updateUI();
    updateModelInfo();

    m_statusLabel->setText("成功加载模型: " + QFileInfo(fileName).fileName());
    emit modelAdded();
  }
  else
  {
    QMessageBox::critical(this, "导入错误", result.errorMessage);
    m_statusLabel->setText("导入失败: " + result.errorMessage);
  }
}

void PlaterWidget::removeSelectedModels()
{
  auto selectedObjects = m_sceneDocument->selection();
  m_sceneDocument->removeModels(selectedObjects);
  
  // 触发渲染窗口更新
  if (m_renderWindow)
  {
    m_renderWindow->Render();
  }
  
  // 更新UI
  updateUI();
  updateModelInfo();
  
  emit modelRemoved();
}

void PlaterWidget::duplicateSelectedModels()
{
  auto selectedObjects = m_sceneDocument->selection();

  for (auto obj : selectedObjects)
  {
    // Duplicate by re-adding the mesh data
    if (obj->volumeCount() > 0)
    {
      slicing::TriangleMesh mesh = obj->volumes()[0].mesh(); // copy mesh
      QString name = QString::fromStdString(obj->name()) + "_copy";
      slicing::ModelObject* newObj = m_sceneDocument->addModel(std::move(mesh), name);

      // Offset the copy
      newObj->setPosition(
        obj->placement().posX + 10.0,
        obj->placement().posY,
        obj->placement().posZ);
    }
  }

  if (!selectedObjects.isEmpty())
  {
    // 触发渲染窗口更新
    if (m_renderWindow)
    {
      m_renderWindow->Render();
    }
    
    // 更新UI
    updateUI();
    updateModelInfo();
    
    m_statusLabel->setText(QString("复制了 %1 个模型").arg(selectedObjects.size()));

    // open dialog after 
    // emit modelAdded();
  }
}

void PlaterWidget::translateModel(double x, double y, double z)
{
  auto selectedObjects = m_sceneDocument->selection();
  if (!selectedObjects.isEmpty())
  {
    m_sceneDocument->translate({ selectedObjects }, x, y, z);
    emit modelTransformed();
  }
}

void PlaterWidget::rotateModel(double x, double y, double z)
{
  auto selectedObjects = m_sceneDocument->selection();
  if (!selectedObjects.isEmpty())
  {
    m_sceneDocument->rotate({ selectedObjects }, x, y, z);
    emit modelTransformed();
  }
}

void PlaterWidget::scaleModel(double x, double y, double z)
{
  auto selectedObjects = m_sceneDocument->selection();
  if (!selectedObjects.isEmpty())
  {
    m_sceneDocument->scale({ selectedObjects }, x, y, z);
    emit modelTransformed();
  }
}

void PlaterWidget::autoLayout()
{
  auto objects = m_sceneDocument->objects();
  if (objects.isEmpty())
    return;

  double bw = m_buildPlatform ? m_buildPlatform->width() : 200.0;
  double bh = m_buildPlatform ? m_buildPlatform->height() : 200.0;
  double margin = 5.0;

  // Sort objects by size (largest first) for better packing
  QList<slicing::ModelObject*> sorted = objects;
  std::sort(sorted.begin(), sorted.end(),
            [](slicing::ModelObject* a, slicing::ModelObject* b) {
    double minAX, minAY, minAZ, maxAX, maxAY, maxAZ;
    double minBX, minBY, minBZ, maxBX, maxBY, maxBZ;
    a->worldBounds(minAX, minAY, minAZ, maxAX, maxAY, maxAZ);
    b->worldBounds(minBX, minBY, minBZ, maxBX, maxBY, maxBZ);
    double areaA = (maxAX - minAX) * (maxAY - minAY);
    double areaB = (maxBX - minBX) * (maxBY - minBY);
    return areaA > areaB; // largest first
  });

  double curX = margin, curY = margin;
  double rowMaxH = 0.0;

  for (auto* obj : sorted)
  {
    double minX, minY, minZ, maxX, maxY, maxZ;
    obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);
    double w = maxX - minX;
    double h = maxY - minY;

    // Wrap to next row if object doesn't fit in current row
    if (curX + w > bw - margin && curX > margin)
    {
      curX = margin;
      curY += rowMaxH + margin;
      rowMaxH = 0.0;
    }

    // Skip if object is too large for a new row
    if (curY + h > bh - margin)
    {
      qWarning() << "Build plate full, skipping:" << obj->name().c_str();
      continue;
    }

    // Move object: align bottom-left to (curX, curY), bottom to Z=0
    obj->setPosition(curX - minX, curY - minY, -minZ);
    curX += w + margin;
    rowMaxH = qMax(rowMaxH, h);
  }

  m_sceneDocument->syncToRenderer();
  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText(QString("自动布局完成: %1 个模型").arg(sorted.size()));
  emit modelTransformed();
}

void PlaterWidget::arrangeInGrid()
{
  auto objects = m_sceneDocument->objects();
  if (objects.isEmpty())
    return;

  double bw = m_buildPlatform ? m_buildPlatform->width() : 200.0;
  double bh = m_buildPlatform ? m_buildPlatform->height() : 200.0;
  double margin = 5.0;

  int n = objects.size();
  int cols = qMax(1, static_cast<int>(ceil(sqrt(static_cast<double>(n)))));
  double cellW = (bw - margin * (cols + 1)) / cols;

  for (int i = 0; i < n; ++i)
  {
    auto* obj = objects[i];
    double minX, minY, minZ, maxX, maxY, maxZ;
    obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);

    int row = i / cols;
    int col = i % cols;
    double targetX = margin + col * (cellW + margin) + cellW / 2.0;
    double targetY = margin + row * (cellW + margin) + cellW / 2.0;

    double cx = (minX + maxX) / 2.0;
    double cy = (minY + maxY) / 2.0;
    obj->setPosition(targetX - cx + obj->placement().posX,
                     targetY - cy + obj->placement().posY,
                     -minZ);
  }

  m_sceneDocument->syncToRenderer();
  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText(QString("网格排列完成: %1×%2").arg(cols).arg((n + cols - 1) / cols));
  emit modelTransformed();
}

void PlaterWidget::centerModel()
{
  auto selectedObjects = m_sceneDocument->selection();
  if (selectedObjects.isEmpty())
  {
    m_statusLabel->setText("没有选中的模型");
    return;
  }

  double bw = m_buildPlatform ? m_buildPlatform->width() : 200.0;
  double bh = m_buildPlatform ? m_buildPlatform->height() : 200.0;
  double cx = bw / 2.0, cy = bh / 2.0;

  for (auto* obj : selectedObjects)
  {
    double minX, minY, minZ, maxX, maxY, maxZ;
    obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);
    double objCx = (minX + maxX) / 2.0;
    double objCy = (minY + maxY) / 2.0;
    obj->setPosition(cx - objCx + obj->placement().posX,
                     cy - objCy + obj->placement().posY,
                     -minZ);
  }

  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText("居中模型完成");
  emit modelTransformed();
}

void PlaterWidget::layFlat()
{
  auto selectedObjects = m_sceneDocument->selection();
  if (selectedObjects.isEmpty())
  {
    m_statusLabel->setText("没有选中的模型");
    return;
  }

  // For each selected model, try several orientations and pick the one
  // that minimizes Z-extent (makes the model as flat as possible).
  const int numSteps = 6; // every 60 degrees around X and Y
  for (auto* obj : selectedObjects)
  {
    // Store original placement
    double origPX = obj->placement().posX;
    double origPY = obj->placement().posY;

    double bestZExtent = std::numeric_limits<double>::max();
    double bestRX = 0.0, bestRY = 0.0;

    // Try rotations around X and Y axes
    for (int xi = 0; xi < numSteps; ++xi)
    {
      double rx = xi * 360.0 / numSteps;
      for (int yi = 0; yi < numSteps; ++yi)
      {
        double ry = yi * 360.0 / numSteps;
        // Apply candidate rotation (keep position unchanged)
        obj->setRotation(rx, ry, 0);

        // Measure Z extent
        double minX, minY, minZ, maxX, maxY, maxZ;
        obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);
        double zExtent = maxZ - minZ;

        if (zExtent < bestZExtent)
        {
          bestZExtent = zExtent;
          bestRX = rx;
          bestRY = ry;
        }
      }
    }

    // Apply best rotation and drop to Z=0
    obj->setRotation(bestRX, bestRY, 0);

    // Re-read bounds after rotation to set Z correctly
    double minX, minY, minZ, maxX, maxY, maxZ;
    obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);
    obj->setPosition(origPX, origPY, -minZ);
  }

  m_sceneDocument->syncToRenderer();
  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText(QString("平放完成: %1 个模型").arg(selectedObjects.size()));
  emit modelTransformed();
}

void PlaterWidget::setViewType(int type)
{
  ViewType viewType = static_cast<ViewType>(type);

  if (m_interactor_style)
  {
    m_interactor_style->SetViewType(viewType);
  }

  // 更新渲染
  if (m_renderWindow)
  {
    m_renderWindow->Render();
  }

  emit viewTypeChanged(type);
}

void PlaterWidget::resetCamera()
{
  if (m_interactor_style)
  {
    m_interactor_style->ResetCamera();
  }
  emit cameraReset();
}

void PlaterWidget::fitAll()
{
  if (m_interactor_style)
  {
    m_interactor_style->FitAll();
  }
  emit cameraFitAll();
}

void PlaterWidget::fitSelected()
{
  if (m_interactor_style)
  {
    m_interactor_style->FitSelected();
  }
  emit cameraFitSelected();
}

void PlaterWidget::updatePlatformSettings()
{
  m_buildPlatform->setWidth(m_platformWidthSpinBox->value());
  m_buildPlatform->setHeight(m_platformHeightSpinBox->value());
  m_buildPlatform->setDepth(m_platformDepthSpinBox->value());
  m_buildPlatform->setGridSpacing(m_gridSpacingSpinBox->value());

  // Trigger 3D viewport refresh
  if (m_renderWindow)
    m_renderWindow->Render();

  emit platformSettingsChanged();
}

void PlaterWidget::setShowGrid(bool show)
{
  m_buildPlatform->setGridVisible(show);
  if (m_renderWindow)
    m_renderWindow->Render();
}

void PlaterWidget::setShowAxes(bool show)
{
  m_buildPlatform->setAxisVisible(show);
  if (m_renderWindow)
    m_renderWindow->Render();
}

void PlaterWidget::setShowRuler(bool show)
{
  m_buildPlatform->setRulerVisible(show);
  if (m_renderWindow)
    m_renderWindow->Render();
}

void PlaterWidget::setShowPlatform(bool show)
{
  if (m_buildPlatform)
  {
    m_buildPlatform->setPlatformVisible(show);
    if (m_renderWindow)
    {
      m_renderWindow->Render();
    }
  }
}

void PlaterWidget::setPlatformVisible(bool visible)
{
  setShowPlatform(visible);
}

void PlaterWidget::clearSupportActors()
{
  for (auto& actor : _supportActors)
  {
    if (actor && m_renderer)
      m_renderer->RemoveActor(actor);
  }
  _supportActors.clear();
}

void PlaterWidget::generateSupport()
{
  auto selectedObjects = m_sceneDocument->selection();
  if (selectedObjects.isEmpty())
  {
    m_statusLabel->setText("没有选中的模型");
    return;
  }

  // Remove old support geometry
  clearSupportActors();
  for (auto* obj : selectedObjects)
    obj->setHasSupport(false);

  // Read parameters from UI
  double density = m_supportDensitySpinBox ? m_supportDensitySpinBox->value() : 0.15;
  double pillarSize = m_pillarSizeSpinBox ? m_pillarSizeSpinBox->value() : 0.8;
  double zOffset = m_supportZOffsetSpinBox ? m_supportZOffsetSpinBox->value() : 0.2;
  bool usePillars = m_supportPillarCheckBox ? m_supportPillarCheckBox->isChecked() : true;
  bool useBase = m_supportBaseCheckBox ? m_supportBaseCheckBox->isChecked() : true;
  double baseSize = m_baseSizeSpinBox ? m_baseSizeSpinBox->value() : 5.0;
  double baseHeight = m_baseHeightSpinBox ? m_baseHeightSpinBox->value() : 1.0;

  if (!usePillars)
  {
    m_statusLabel->setText("支撑支柱已禁用");
    return;
  }

  int totalPillars = 0;

  for (auto* obj : selectedObjects)
  {
    if (obj->volumeCount() == 0)
      continue;

    // Get the mesh from the first volume
    const auto& mesh = obj->volumes()[0].mesh();
    const auto& triangles = mesh.triangles();
    const auto& verts = mesh.vertices();

    if (triangles.empty() || verts.empty())
      continue;

    // Overhang threshold: faces with normal Z < -0.3 (~72° overhang angle)
    const double overhangThreshold = -0.3;

    // Find the Z-extent of the model for density calculation
    double minX, minY, minZ, maxX, maxY, maxZ;
    obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);

    // Collect overhang support points
    struct SupportPoint { double x, y, z; };
    std::vector<SupportPoint> candidates;

    for (const auto& tri : triangles)
    {
      // Compute face normal using cross product
      const auto& v0 = verts[tri.v0];
      const auto& v1 = verts[tri.v1];
      const auto& v2 = verts[tri.v2];

      double ax = slicing::unscale(v1.x - v0.x), ay = slicing::unscale(v1.y - v0.y), az = slicing::unscale(v1.z - v0.z);
      double bx = slicing::unscale(v2.x - v0.x), by = slicing::unscale(v2.y - v0.y), bz = slicing::unscale(v2.z - v0.z);

      double nx = ay * bz - az * by;
      double ny = az * bx - ax * bz;
      double nz = ax * by - ay * bx;

      double len = sqrt(nx * nx + ny * ny + nz * nz);
      if (len < 1e-9)
        continue;
      nx /= len;
      ny /= len;
      nz /= len;

      // Check for overhang (downward-facing face)
      if (nz < overhangThreshold)
      {
        // Centroid of the triangle (convert from scaled microns to mm)
        double cx = slicing::unscale((v0.x + v1.x + v2.x) / 3);
        double cy = slicing::unscale((v0.y + v1.y + v2.y) / 3);
        double cz = slicing::unscale((v0.z + v1.z + v2.z) / 3);

        // Project support point to Z=0 + zOffset (build plate)
        candidates.push_back({ cx, cy, cz + zOffset });
      }
    }

    if (candidates.empty())
      continue;

    // Sample support points based on density
    // Density: higher value = more supports. Sample grid spacing inversely proportional to density.
    double gridSpacing = 10.0 / density; // density 0.15 → ~66mm, 0.5 → 20mm
    gridSpacing = qMax(2.0, qMin(gridSpacing, 50.0));

    std::vector<SupportPoint> sampled;
    for (const auto& pt : candidates)
    {
      // Check if this point is far enough from already-sampled points
      bool tooClose = false;
      for (const auto& sp : sampled)
      {
        double dx = pt.x - sp.x, dy = pt.y - sp.y;
        if (dx * dx + dy * dy < gridSpacing * gridSpacing)
        {
          tooClose = true;
          break;
        }
      }
      if (!tooClose && pt.z > minZ + 0.5) // must be above the bottom of the model
        sampled.push_back(pt);
    }

    // Generate pillar geometry for each support point
    for (const auto& pt : sampled)
    {
      if (pt.z <= 0.1)
        continue;

      double pillarHeight = pt.z;
      double pillarRadius = pillarSize;

      // Create cylinder for pillar
      auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
      cylinder->SetRadius(pillarRadius);
      cylinder->SetHeight(pillarHeight);
      cylinder->SetResolution(8);
      cylinder->SetCenter(0, pillarHeight / 2.0, 0);

      // Position the pillar
      auto transform = vtkSmartPointer<vtkTransform>::New();
      transform->Translate(pt.x, pt.y, 0);
      transform->RotateX(90); // cylinder along Y → Z axis

      auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transformFilter->SetInputConnection(cylinder->GetOutputPort());
      transformFilter->SetTransform(transform);
      transformFilter->Update();

      auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetInputConnection(transformFilter->GetOutputPort());

      auto actor = vtkSmartPointer<vtkActor>::New();
      actor->SetMapper(mapper);
      actor->GetProperty()->SetColor(0.3, 0.8, 0.3); // green supports
      actor->GetProperty()->SetOpacity(0.6);
      actor->SetPickable(false);

      if (m_renderer)
        m_renderer->AddActor(actor);
      _supportActors.push_back(actor);
      totalPillars++;

      // Add base pad if enabled
      if (useBase)
      {
        auto base = vtkSmartPointer<vtkCylinderSource>::New();
        base->SetRadius(baseSize);
        base->SetHeight(baseHeight);
        base->SetResolution(12);
        base->SetCenter(0, baseHeight / 2.0, 0);

        auto baseTransform = vtkSmartPointer<vtkTransform>::New();
        baseTransform->Translate(pt.x, pt.y, 0);
        baseTransform->RotateX(90);

        auto baseFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        baseFilter->SetInputConnection(base->GetOutputPort());
        baseFilter->SetTransform(baseTransform);
        baseFilter->Update();

        auto baseMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        baseMapper->SetInputConnection(baseFilter->GetOutputPort());

        auto baseActor = vtkSmartPointer<vtkActor>::New();
        baseActor->SetMapper(baseMapper);
        baseActor->GetProperty()->SetColor(0.3, 0.8, 0.3);
        baseActor->GetProperty()->SetOpacity(0.6);
        baseActor->SetPickable(false);

        if (m_renderer)
          m_renderer->AddActor(baseActor);
        _supportActors.push_back(baseActor);
      }
    }

    obj->setHasSupport(!sampled.empty());
  }

  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText(QString("生成支撑完成: %1 个支柱").arg(totalPillars));
}

void PlaterWidget::removeSupport()
{
  clearSupportActors();

  for (auto* obj : m_sceneDocument->objects())
    obj->setHasSupport(false);

  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText("移除支撑完成");
}

void PlaterWidget::previewPrint()
{
  generateGCode(); // Preview = slice + show layers
}

void PlaterWidget::generateGCode()
{
  if (m_slicingController->isSlicing())
  {
    m_slicingController->cancelSlice();
    m_statusLabel->setText("取消切片...");
    return;
  }

  auto objects = m_sceneDocument->objects();
  if (objects.isEmpty())
  {
    m_statusLabel->setText("没有模型可以切片");
    return;
  }

  // Check for model collisions
  auto collisions = m_sceneDocument->checkCollisions();
  if (!collisions.isEmpty())
  {
    QStringList names;
    for (const auto& c : collisions)
      names << QString("%1 ↔ %2")
        .arg(QString::fromStdString(c.a->name()))
        .arg(QString::fromStdString(c.b->name()));
    auto reply = QMessageBox::warning(this, "模型碰撞",
      QString("检测到模型重叠:\n%1\n\n继续切片可能导致打印失败。是否继续?")
        .arg(names.join("\n")),
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::No) return;
  }

  // Check if models fit on the build plate
  if (m_buildPlatform)
  {
    double bw = m_buildPlatform->width(), bh = m_buildPlatform->height();
    QStringList outOfBounds;
    for (auto* obj : objects)
    {
      double minX, minY, minZ, maxX, maxY, maxZ;
      obj->worldBounds(minX, minY, minZ, maxX, maxY, maxZ);
      if (minX < 0 || minY < 0 || maxX > bw || maxY > bh)
        outOfBounds << QString::fromStdString(obj->name());
    }
    if (!outOfBounds.isEmpty())
    {
      auto reply = QMessageBox::warning(this, "超出平台",
        QString("以下模型超出打印平台范围:\n%1\n\n继续切片可能导致打印失败。是否继续?")
          .arg(outOfBounds.join("\n")),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (reply == QMessageBox::No) return;
    }
  }

  // Convert QList → std::vector for SlicingController
  std::vector<slicing::ModelObject*> objVec(objects.begin(), objects.end());

  double layerHeight = m_layerHeightSpinBox ? m_layerHeightSpinBox->value() : 0.2;

  m_statusLabel->setText("开始切片...");
  m_progressBar->setVisible(true);
  m_progressBar->setValue(0);

  m_slicingController->startSlice(objVec, slicing::scale(layerHeight));
}

void PlaterWidget::onSelectionChanged(const QList<slicing::ModelObject*>& selectedObjects)
{
  updateUI();

  if (selectedObjects.isEmpty())
  {
    return;
  }

  // 更新变换面板 - 暂时注释，后续根据SceneObject扩展

  slicing::ModelObject* firstObj = selectedObjects.first();

  m_updatingUI = true;

  // 更新位置
  QVector3D pos = QVector3D(firstObj->placement().posX, firstObj->placement().posY, firstObj->placement().posZ);
  m_posXSpinBox->setValue(pos.x());
  m_posYSpinBox->setValue(pos.y());
  m_posZSpinBox->setValue(pos.z());

  // 更新旋转
  QVector3D rot(firstObj->placement().rotX, firstObj->placement().rotY, firstObj->placement().rotZ);
  m_rotXSpinBox->setValue(rot.x());
  m_rotYSpinBox->setValue(rot.y());
  m_rotZSpinBox->setValue(rot.z());

  // 更新缩放
  QVector3D scale(firstObj->placement().scaleX, firstObj->placement().scaleY, firstObj->placement().scaleZ);
  m_scaleXSpinBox->setValue(scale.x());
  m_scaleYSpinBox->setValue(scale.y());
  m_scaleZSpinBox->setValue(scale.z());

  m_updatingUI = false;
}

void PlaterWidget::onObjectModified(slicing::ModelObject* obj)
{
  updateModelInfo();
  emit modelTransformed();
}

void PlaterWidget::onPlatformPropertyChanged()
{
  updateUI();
  emit platformSettingsChanged();
}

// 键盘事件处理
void PlaterWidget::keyPressEvent(QKeyEvent* event)
{
  if (!event)
    return;

  int key = event->key();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  // 处理快捷键
  switch (key)
  {
    case Qt::Key_F:
      if (modifiers & Qt::ControlModifier)
      {
        if (modifiers & Qt::ShiftModifier)
          fitSelected();
        else
          fitAll();
      }
      break;

    case Qt::Key_R:
      if (modifiers & Qt::ControlModifier)
        resetCamera();
      break;

    case Qt::Key_Space:
      cycleViewTypes();
      break;

    case Qt::Key_G:
      if (modifiers & Qt::ControlModifier)
        toggleGridVisibility();
      break;

    case Qt::Key_A:
      if (modifiers & Qt::ControlModifier)
        toggleAxesVisibility();
      break;

    case Qt::Key_V:
      if (modifiers & Qt::ControlModifier)
        saveCurrentView();
      else if (modifiers & Qt::AltModifier)
        restoreSavedView();
      break;

    case Qt::Key_1:
      if (modifiers & Qt::ControlModifier)
        setViewType(static_cast<int>(ViewType::Perspective));
      break;

    case Qt::Key_2:
      if (modifiers & Qt::ControlModifier)
        setViewType(static_cast<int>(ViewType::Top));
      break;

    case Qt::Key_3:
      if (modifiers & Qt::ControlModifier)
        setViewType(static_cast<int>(ViewType::Front));
      break;

    case Qt::Key_4:
      if (modifiers & Qt::ControlModifier)
        setViewType(static_cast<int>(ViewType::Right));
      break;

    default:
      QWidget::keyPressEvent(event);
      return;
  }

  event->accept();
}

void PlaterWidget::keyReleaseEvent(QKeyEvent* event)
{
  QWidget::keyReleaseEvent(event);
}

// 保存当前视图
void PlaterWidget::saveCurrentView()
{
  if (!m_renderer || !m_renderer->IsActiveCameraCreated())
    return;

  vtkCamera* camera = m_renderer->GetActiveCamera();

  camera->GetPosition(m_savedView.cameraPosition);
  camera->GetFocalPoint(m_savedView.focalPoint);
  camera->GetViewUp(m_savedView.viewUp);
  m_savedView.viewAngle = camera->GetViewAngle();
  m_savedView.parallelScale = camera->GetParallelScale();
  m_savedView.parallelProjection = camera->GetParallelProjection() != 0;
  m_savedView.isSaved = true;

  m_statusLabel->setText("视图已保存");
}

// 恢复保存的视图
void PlaterWidget::restoreSavedView()
{
  if (!m_savedView.isSaved || !m_renderer || !m_renderer->IsActiveCameraCreated())
    return;

  vtkCamera* camera = m_renderer->GetActiveCamera();

  camera->SetPosition(m_savedView.cameraPosition);
  camera->SetFocalPoint(m_savedView.focalPoint);
  camera->SetViewUp(m_savedView.viewUp);
  camera->SetViewAngle(m_savedView.viewAngle);
  camera->SetParallelScale(m_savedView.parallelScale);
  camera->SetParallelProjection(m_savedView.parallelProjection);

  if (m_renderWindow)
    m_renderWindow->Render();

  m_statusLabel->setText("视图已恢复");
}

// 切换网格可见性
void PlaterWidget::toggleGridVisibility()
{
  bool currentState = m_showGridCheckBox->isChecked();
  m_showGridCheckBox->setChecked(!currentState);
  setShowGrid(!currentState);
}

// 切换坐标轴可见性
void PlaterWidget::toggleAxesVisibility()
{
  bool currentState = m_showAxesCheckBox->isChecked();
  m_showAxesCheckBox->setChecked(!currentState);
  setShowAxes(!currentState);
}

// 循环切换视图类型
void PlaterWidget::cycleViewTypes()
{
  m_currentViewType = (m_currentViewType + 1) % 8; // 总共有8种视图类型
  setViewType(m_currentViewType);

  // 更新状态栏显示当前视图类型
  QString viewTypeNames[] = { "透视图", "正交图", "顶视图", "底视图", "前视图", "后视图", "左视图",
    "右视图" };
  m_statusLabel->setText(QString("视图: %1").arg(viewTypeNames[m_currentViewType]));
}

void PlaterWidget::updateLayerPreview()
{
    if (!m_layerSlider || !m_renderer || !m_slicingController) return;

    auto* pipeline = m_slicingController->lastPipeline();
    if (!pipeline) return;

    int layerIdx = m_layerSlider->value();
    const auto& layers = pipeline->layers();
    if (layerIdx < 0 || static_cast<size_t>(layerIdx) >= layers.size()) return;

    // Remove old preview actor
    if (m_previewActor)
    {
        m_renderer->RemoveActor(m_previewActor);
        m_previewActor = nullptr;
    }

    const auto& layer = layers[layerIdx];
    if (layer.empty()) return;

    // Build VTK polydata for this layer's slices
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> lines;
    vtkIdType ptIdx = 0;

    for (const auto& expoly : layer.slices())
    {
        const auto& contour = expoly.contour;
        if (contour.size() < 2) continue;

        vtkIdType startIdx = ptIdx;
        for (const auto& pt : contour)
        {
            points->InsertNextPoint(slicing::unscale(pt.x), slicing::unscale(pt.y), layer.zMm());
            ptIdx++;
        }
        // Connect points as line loop
        for (vtkIdType i = 0; i < static_cast<vtkIdType>(contour.size()); ++i)
        {
            vtkIdType p1 = startIdx + i;
            vtkIdType p2 = startIdx + (i + 1) % contour.size();
            lines->InsertNextCell(2);
            lines->InsertCellPoint(p1);
            lines->InsertCellPoint(p2);
        }
    }

    if (points->GetNumberOfPoints() == 0) return;

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(lines);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    m_previewActor = vtkSmartPointer<vtkActor>::New();
    m_previewActor->SetMapper(mapper);
    m_previewActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green lines
    m_previewActor->GetProperty()->SetLineWidth(2.0);
    m_renderer->AddActor(m_previewActor);

    if (m_layerLabel)
        m_layerLabel->setText(QString("层: %1/%2").arg(layerIdx).arg(layers.size()));

    m_renderWindow->Render();
}
