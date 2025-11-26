#include "plater_widget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDoubleSpinBox>
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
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "engine/file_importer.h"

#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

PlaterWidget::PlaterWidget(QWidget* parent, Qt::WindowFlags flag)
  : QVTKOpenGLNativeWidget(parent, flag)
  , m_buildPlatform(nullptr)
  , m_sceneManager(nullptr)
{
  // 创建核心组件
  m_buildPlatform = new BuildPlatform();
  m_sceneManager = SceneManagerV2::getInstance();

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
  m_interactor_style->SetSceneManager(m_sceneManager);

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
  m_rightTabWidget->setVisible(false);
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
  m_progressBar->setVisible(false); // 默认隐藏
  m_bottomPanelLayout->addWidget(m_progressBar);

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

  // 场景管理器信号连接
  connect(m_sceneManager, &SceneManagerV2::multiSelectionChanged, this,
    &PlaterWidget::onSelectionChanged);
  connect(m_sceneManager, &SceneManagerV2::objectGeometryChanged, this,
    &PlaterWidget::onObjectGeometryChanged);
}

void PlaterWidget::updateUI()
{
  // 更新UI状态
  bool hasSelection = !m_sceneManager->selectedObjects().isEmpty();

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
  for (auto obj : m_sceneManager->rootObjects())
  {
    m_modelTable->insertRow(row);

    // 名称
    m_modelTable->setItem(row, 0, new QTableWidgetItem(obj->name()));

    // 尺寸 - 暂时使用固定值，后续根据SceneObject扩展
    m_modelTable->setItem(row, 1,
      new QTableWidgetItem(
        QString("%1x%2x%3").arg(10.0, 0, 'f', 1).arg(10.0, 0, 'f', 1).arg(10.0, 0, 'f', 1)));

    // 体积 - 暂时使用固定值，后续根据SceneObject扩展
    m_modelTable->setItem(row, 2, new QTableWidgetItem(QString("%1 cm³").arg(1.0, 0, 'f', 2)));

    // 重量 - 暂时使用固定值，后续根据SceneObject扩展
    m_modelTable->setItem(row, 3, new QTableWidgetItem(QString("%1 g").arg(1.0, 0, 'f', 2)));

    // 打印时间 - 暂时使用固定值，后续根据SceneObject扩展
    m_modelTable->setItem(row, 4, new QTableWidgetItem(QString("%1h %2m").arg(1).arg(0)));

    row++;
  }

  // 更新底部状态栏
  int modelCount = m_sceneManager->rootObjects().count();
  m_modelCountLabel->setText(QString("模型数量: %1").arg(modelCount));

  // 计算总重量和总打印时间 - 暂时使用固定值
  double totalWeight = 0.0;
  double totalTime = 0.0;

  for (auto obj : m_sceneManager->rootObjects())
  {
    totalWeight += 1.0; // 暂时使用固定值
    totalTime += 3600;  // 暂时使用固定值
  }

  m_materialUsageLabel->setText(QString("材料用量: %1g").arg(totalWeight, 0, 'f', 2));
  m_printTimeLabel->setText(QString("预估时间: %1h %2m")
                              .arg(static_cast<int>(totalTime) / 3600)
                              .arg((static_cast<int>(totalTime) % 3600) / 60));
}

// 实现公共槽函数
void PlaterWidget::addModel()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, "添加3D模型", "", FileImporter::getFileFilter());

  if (!fileName.isEmpty())
  {
    // 创建文件导入器
    FileImporter importer;

    // 显示导入选项对话框
    ImportOptions options = FileImporter::showImportOptionsDialog(fileName, this);

    // 导入文件
    ImportResult result = importer.importFile(fileName, options);

    if (result.success)
    {
      // 创建场景对象
      SceneObjectV2* obj = new SceneObjectV2(QFileInfo(fileName).baseName(), result.actor);

      obj->setPolyData(result.polyData);

      // 设置元数据
      ObjectMetadata metadata;
      metadata.meshInfo = result.meshInfo;
      metadata.volume = result.volume;
      metadata.weight = result.weight;
      metadata.fileSize = result.fileSize;
      metadata.importTime = QDateTime::currentDateTime();
      metadata.lastModified = QDateTime::currentDateTime();
      obj->setMetadata(metadata);

      // 添加到场景管理器
      m_sceneManager->addObject(obj);

      m_statusLabel->setText("成功加载模型: " + QFileInfo(fileName).fileName());
      emit modelAdded();
    }
    else
    {
      QMessageBox::critical(this, "导入错误", result.errorMessage);
      m_statusLabel->setText("导入失败: " + result.errorMessage);
    }
  }
}

void PlaterWidget::removeSelectedModels()
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  m_sceneManager->removeObjects(selectedObjects);
  emit modelRemoved();
}

void PlaterWidget::duplicateSelectedModels()
{
  auto selectedObjects = m_sceneManager->selectedObjects();

  for (auto obj : selectedObjects)
  {
    SceneObjectV2* newObj = obj->clone();

    // 稍微偏移复制后的模型位置
    QVector3D pos = obj->position();
    pos.setX(pos.x() + 10.0); // 向右偏移10mm
    newObj->setPosition(pos);

    m_sceneManager->addObject(newObj);
  }

  if (!selectedObjects.isEmpty())
  {
    m_statusLabel->setText(QString("复制了 %1 个模型").arg(selectedObjects.size()));
    emit modelAdded();
  }
}

void PlaterWidget::translateModel(double x, double y, double z)
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    m_sceneManager->setObjectsTranslation(selectedObjects, x, y, z);
    emit modelTransformed();
  }
}

void PlaterWidget::rotateModel(double x, double y, double z)
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    m_sceneManager->setObjectsRotation(selectedObjects, x, y, z);
    emit modelTransformed();
  }
}

void PlaterWidget::scaleModel(double x, double y, double z)
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    m_sceneManager->setObjectsScale(selectedObjects, x, y, z);
    emit modelTransformed();
  }
}

void PlaterWidget::autoLayout()
{
  m_sceneManager->autoLayoutObjects();
  emit modelTransformed();
}

void PlaterWidget::arrangeInGrid()
{
  m_sceneManager->arrangeInGrid(3);
  emit modelTransformed();
}

void PlaterWidget::centerModel()
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    for (auto obj : selectedObjects)
    {
      // 将模型中心移动到原点
      obj->setPosition(QVector3D(0, 0, 0));
    }
    m_statusLabel->setText("居中模型完成");
    emit modelTransformed();
  }
}

void PlaterWidget::layFlat()
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    // 简单实现：将模型Z坐标设置为0
    for (auto obj : selectedObjects)
    {
      QVector3D pos = obj->position();
      pos.setZ(0.0); // 平放在Z=0平面上
      obj->setPosition(pos);
    }
    m_statusLabel->setText("模型已平放");
    emit modelTransformed();
  }
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

  emit platformSettingsChanged();
}

void PlaterWidget::setShowGrid(bool show)
{
  m_buildPlatform->setGridVisible(show);
}

void PlaterWidget::setShowAxes(bool show)
{
  m_buildPlatform->setAxisVisible(show);
}

void PlaterWidget::setShowRuler(bool show)
{
  m_buildPlatform->setRulerVisible(show);
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

void PlaterWidget::generateSupport()
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    // 暂时注释，后续根据SceneObject扩展

    for (auto obj : selectedObjects)
    {
      obj->setHasSupport(true);
    }
    m_statusLabel->setText("生成支撑完成");
  }
}

void PlaterWidget::removeSupport()
{
  auto selectedObjects = m_sceneManager->selectedObjects();
  if (!selectedObjects.isEmpty())
  {
    // 暂时注释，后续根据SceneObject扩展
    // for (auto obj : selectedObjects)
    // {
    //   obj->setHasSupport(false);
    // }
    m_statusLabel->setText("移除支撑完成");
  }
}

void PlaterWidget::previewPrint()
{
  // TODO: 实现打印预览
  m_statusLabel->setText("生成打印预览...");
}

void PlaterWidget::generateGCode()
{
  // TODO: 实现G代码生成
  m_statusLabel->setText("生成G代码...");
}

void PlaterWidget::onSelectionChanged(const QList<SceneObjectV2*>& selectedObjects)
{
  updateUI();

  if (selectedObjects.isEmpty())
  {
    return;
  }

  // 更新变换面板 - 暂时注释，后续根据SceneObject扩展

  SceneObjectV2* firstObj = selectedObjects.first();

  m_updatingUI = true;

  // 更新位置
  QVector3D pos = firstObj->position();
  m_posXSpinBox->setValue(pos.x());
  m_posYSpinBox->setValue(pos.y());
  m_posZSpinBox->setValue(pos.z());

  // 更新旋转
  QVector3D rot = firstObj->rotation();
  m_rotXSpinBox->setValue(rot.x());
  m_rotYSpinBox->setValue(rot.y());
  m_rotZSpinBox->setValue(rot.z());

  // 更新缩放
  QVector3D scale = firstObj->scale();
  m_scaleXSpinBox->setValue(scale.x());
  m_scaleYSpinBox->setValue(scale.y());
  m_scaleZSpinBox->setValue(scale.z());

  m_updatingUI = false;
}

void PlaterWidget::onObjectGeometryChanged(SceneObjectV2* obj)
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
