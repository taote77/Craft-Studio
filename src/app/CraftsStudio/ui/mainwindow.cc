#include "mainwindow.h"

#include "../action_factory.h"
#include "engine/file_importer.h"
#include "engine/rs_interactor_v2.h"
#include "engine/rs_scene_manager_v2.h"
#include "engine/rs_scene_object_v2.h"

#include "menu_manager.h"
#include "project_tree.h"

#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>

#include <vtkDebugLeaks.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupUI();
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
  return QMainWindow::contextMenuEvent(event);
  // _right_button_menu->exec(QCursor::pos());
}

void MainWindow::setupUI()
{
  this->resize(1200, 900);

  // 创建动作工厂
  _action_factory = new ActionFactory(this);
  connect(_action_factory, &ActionFactory::sigExit, this, &MainWindow::exit);

  _menu_mgr = new MenuManager(this);
  _menu_mgr->init(*dynamic_cast<MainWindow*>(this), *_action_factory);

  // 创建菜单栏
  setupMenuBar();

  // 创建状态栏
  setupStatusBar();

  // 创建中央标签页部件
  setupTabWidget();

  // 创建左侧停靠窗口
  _left_dock = new QDockWidget("项目树", this);
  _left_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _left_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  _left_dock->setFloating(false);
  _left_dock->setMaximumWidth(400);
  addDockWidget(Qt::LeftDockWidgetArea, _left_dock);

  auto treeView = new ProjectTree(this);
  _left_dock->setWidget(treeView);

  // 右键菜单
  _right_button_menu = new QMenu(this);

  auto style_menu = _right_button_menu->addMenu("3D Interactor Style");

  auto action = new QAction("Interact Style");
  connect(action, &QAction::triggered, this,
    [this]
    {
      auto style = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
      auto interactor = _platerWidget->getRenderWindow()->GetInteractor();
      interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);

  action = style_menu->addAction("Camera Track");
  connect(action, &QAction::triggered, this,
    [this]
    {
      auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
      auto interactor = _platerWidget->getRenderWindow()->GetInteractor();
      interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);

  action = style_menu->addAction("Rubber Band Zoom");
  connect(action, &QAction::triggered, this,
    [this]
    {
      auto style = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
      auto interactor = _platerWidget->getRenderWindow()->GetInteractor();
      interactor->SetInteractorStyle(style);
    });
  style_menu->addAction(action);
}

void MainWindow::setupMenuBar()
{
  createFileMenu();
  createEditMenu();
  createViewMenu();
  createToolsMenu();
  createHelpMenu();
}

void MainWindow::setupStatusBar()
{
  QStatusBar* statusBar = this->statusBar();

  // 创建状态栏标签
  _statusLabel = new QLabel("就绪");
  _modelCountLabel = new QLabel("模型: 0");
  _renderTimeLabel = new QLabel("渲染时间: 0ms");
  _memoryUsageLabel = new QLabel("内存: 0MB");

  // 添加到状态栏
  statusBar->addWidget(_statusLabel, 1); // 伸展因子为1
  statusBar->addPermanentWidget(_modelCountLabel);
  statusBar->addPermanentWidget(_renderTimeLabel);
  statusBar->addPermanentWidget(_memoryUsageLabel);
}

void MainWindow::setupTabWidget()
{
  _centralTabWidget = new QTabWidget(this);
  this->setCentralWidget(_centralTabWidget);

  // 创建Plater工作台
  _platerWidget = new PlaterWidget(this);
  _centralTabWidget->addTab(_platerWidget, QString("构建平台"));

  _action_factory->setVtkRenderWindow(_platerWidget->getRenderWindow());
  // 连接Plater信号
  connect(_platerWidget, &PlaterWidget::modelAdded, this, &MainWindow::onAddModel);
  connect(_platerWidget, &PlaterWidget::modelRemoved, this, &MainWindow::onRemoveModel);
}

void MainWindow::createFileMenu()
{
  QMenu* fileMenu = menuBar()->addMenu("文件(&F)");

  _newAction = new QAction("新建(&N)", this);
  _newAction->setShortcut(QKeySequence::New);
  _newAction->setStatusTip("创建新项目");
  connect(_newAction, &QAction::triggered, this, &MainWindow::onNewProject);
  fileMenu->addAction(_newAction);

  _openAction = new QAction("打开(&O)", this);
  _openAction->setShortcut(QKeySequence::Open);
  _openAction->setStatusTip("打开项目文件");
  connect(_openAction, &QAction::triggered, this, &MainWindow::onImportFile);
  fileMenu->addAction(_openAction);

  fileMenu->addSeparator();

  _saveAction = new QAction("保存(&S)", this);
  _saveAction->setShortcut(QKeySequence::Save);
  _saveAction->setStatusTip("保存当前项目");
  connect(_saveAction, &QAction::triggered, this, &MainWindow::onSaveProject);
  fileMenu->addAction(_saveAction);

  _saveAsAction = new QAction("另存为(&A)", this);
  _saveAsAction->setShortcut(QKeySequence::SaveAs);
  _saveAsAction->setStatusTip("将项目另存为");
  connect(_saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsProject);
  fileMenu->addAction(_saveAsAction);

  fileMenu->addSeparator();

  _exportAction = new QAction("导出(&E)", this);
  _exportAction->setShortcut(QKeySequence("Ctrl+E"));
  _exportAction->setStatusTip("导出模型或G代码");
  connect(_exportAction, &QAction::triggered, this, &MainWindow::onExportFile);
  fileMenu->addAction(_exportAction);

  fileMenu->addSeparator();

  _exitAction = new QAction("退出(&X)", this);
  _exitAction->setShortcut(QKeySequence::Quit);
  _exitAction->setStatusTip("退出应用程序");
  connect(_exitAction, &QAction::triggered, this, &MainWindow::exit);
  fileMenu->addAction(_exitAction);
}

void MainWindow::createEditMenu()
{
  QMenu* editMenu = menuBar()->addMenu("编辑(&E)");

  _undoAction = new QAction("撤销(&U)", this);
  _undoAction->setShortcut(QKeySequence::Undo);
  _undoAction->setStatusTip("撤销上一步操作");
  connect(_undoAction, &QAction::triggered, this,
    []()
    {
      // TODO: 实现撤销
    });
  editMenu->addAction(_undoAction);

  _redoAction = new QAction("重做(&R)", this);
  _redoAction->setShortcut(QKeySequence::Redo);
  _redoAction->setStatusTip("重做上一步操作");
  connect(_redoAction, &QAction::triggered, this,
    []()
    {
      // TODO: 实现重做
    });
  editMenu->addAction(_redoAction);

  editMenu->addSeparator();

  _addAction = new QAction("添加模型(&A)", this);
  _addAction->setShortcut(QKeySequence("Ctrl+A"));
  _addAction->setStatusTip("添加新模型");
  connect(_addAction, &QAction::triggered, this, &MainWindow::onAddModel);
  editMenu->addAction(_addAction);

  _removeAction = new QAction("删除模型(&D)", this);
  _removeAction->setShortcut(QKeySequence::Delete);
  _removeAction->setStatusTip("删除选中的模型");
  connect(_removeAction, &QAction::triggered, this, &MainWindow::onRemoveModel);
  editMenu->addAction(_removeAction);

  _duplicateAction = new QAction("复制模型(&C)", this);
  _duplicateAction->setShortcut(QKeySequence("Ctrl+D"));
  _duplicateAction->setStatusTip("复制选中的模型");
  connect(_duplicateAction, &QAction::triggered, this,
    []()
    {
      // TODO: 实现复制模型
    });
  editMenu->addAction(_duplicateAction);

  editMenu->addSeparator();

  _selectAllAction = new QAction("全选(&L)", this);
  _selectAllAction->setShortcut(QKeySequence::SelectAll);
  _selectAllAction->setStatusTip("选择所有模型");
  connect(_selectAllAction, &QAction::triggered, this,
    []()
    {
      // TODO: 实现全选
    });
  editMenu->addAction(_selectAllAction);
}

void MainWindow::createViewMenu()
{
  QMenu* viewMenu = menuBar()->addMenu("视图(&V)");

  // 启用视图控制功能
  _resetViewAction = new QAction("重置视图(&R)", this);
  _resetViewAction->setShortcut(QKeySequence("Ctrl+R"));
  _resetViewAction->setStatusTip("重置相机视图");
  connect(_resetViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->resetCamera();
    });
  viewMenu->addAction(_resetViewAction);

  _fitViewAction = new QAction("适配所有(&F)", this);
  _fitViewAction->setShortcut(QKeySequence("Ctrl+F"));
  _fitViewAction->setStatusTip("适配所有模型到视图");
  connect(_fitViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->fitAll();
    });
  viewMenu->addAction(_fitViewAction);

  viewMenu->addSeparator();

  // 启用视图切换动作
  _topViewAction = new QAction("顶视图(&T)", this);
  _topViewAction->setShortcut(QKeySequence("1"));
  _topViewAction->setStatusTip("切换到顶视图");
  connect(_topViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->setViewType(static_cast<int>(ViewType::Top));
    });
  viewMenu->addAction(_topViewAction);

  _bottomViewAction = new QAction("底视图(&B)", this);
  _bottomViewAction->setShortcut(QKeySequence("2"));
  _bottomViewAction->setStatusTip("切换到底视图");
  connect(_bottomViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->setViewType(static_cast<int>(ViewType::Bottom));
    });
  viewMenu->addAction(_bottomViewAction);

  _frontViewAction = new QAction("前视图(&F)", this);
  _frontViewAction->setShortcut(QKeySequence("3"));
  _frontViewAction->setStatusTip("切换到前视图");
  connect(_frontViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->setViewType(static_cast<int>(ViewType::Front));
    });
  viewMenu->addAction(_frontViewAction);

  _leftViewAction = new QAction("左视图(&L)", this);
  _leftViewAction->setShortcut(QKeySequence("4"));
  _leftViewAction->setStatusTip("切换到左视图");
  connect(_leftViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->setViewType(static_cast<int>(ViewType::Left));
    });
  viewMenu->addAction(_leftViewAction);

  _rightViewAction = new QAction("右视图(&R)", this);
  _rightViewAction->setShortcut(QKeySequence("5"));
  _rightViewAction->setStatusTip("切换到右视图");
  connect(_rightViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->setViewType(static_cast<int>(ViewType::Right));
    });
  viewMenu->addAction(_rightViewAction);

  _perspectiveViewAction = new QAction("透视视图(&P)", this);
  _perspectiveViewAction->setShortcut(QKeySequence("6"));
  _perspectiveViewAction->setStatusTip("切换到透视视图");
  connect(_perspectiveViewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->setViewType(static_cast<int>(ViewType::Perspective));
    });
  viewMenu->addAction(_perspectiveViewAction);
}

void MainWindow::createToolsMenu()
{
  QMenu* toolsMenu = menuBar()->addMenu("工具(&T)");

  // 启用工具功能
  _generateSupportAction = new QAction("生成支撑(&G)", this);
  _generateSupportAction->setShortcut(QKeySequence("Ctrl+G"));
  _generateSupportAction->setStatusTip("为模型生成支撑结构");
  connect(_generateSupportAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->generateSupport();
    });
  toolsMenu->addAction(_generateSupportAction);

  _removeSupportAction = new QAction("移除支撑(&R)", this);
  _removeSupportAction->setShortcut(QKeySequence("Ctrl+Shift+G"));
  _removeSupportAction->setStatusTip("移除模型的所有支撑");
  connect(_removeSupportAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->removeSupport();
    });
  toolsMenu->addAction(_removeSupportAction);

  toolsMenu->addSeparator();

  _autoLayoutAction = new QAction("自动布局(&L)", this);
  _autoLayoutAction->setShortcut(QKeySequence("Ctrl+L"));
  _autoLayoutAction->setStatusTip("自动排列模型");
  connect(_autoLayoutAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->autoLayout();
    });
  toolsMenu->addAction(_autoLayoutAction);

  toolsMenu->addSeparator();

  // TODO: 切片功能实现完成后启用
  /*
  _sliceAction = new QAction("切片(&S)", this);
  _sliceAction->setShortcut(QKeySequence("Ctrl+S"));
  _sliceAction->setStatusTip("切片模型生成G代码");
  connect(_sliceAction, &QAction::triggered, this, []() {
    // TODO: 实现切片功能
  });
  toolsMenu->addAction(_sliceAction);
  */

  // 启用预览和导出功能
  _previewAction = new QAction("预览(&P)", this);
  _previewAction->setShortcut(QKeySequence("Ctrl+P"));
  _previewAction->setStatusTip("预览切片结果");
  connect(_previewAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->previewPrint();
    });
  toolsMenu->addAction(_previewAction);

  _exportGCodeAction = new QAction("导出G代码(&E)", this);
  _exportGCodeAction->setShortcut(QKeySequence("Ctrl+E"));
  _exportGCodeAction->setStatusTip("导出G代码文件");
  connect(_exportGCodeAction, &QAction::triggered, this,
    [this]()
    {
      if (_platerWidget)
        _platerWidget->generateGCode();
    });
  toolsMenu->addAction(_exportGCodeAction);
}

void MainWindow::createHelpMenu()
{
  QMenu* helpMenu = menuBar()->addMenu("帮助(&H)");

  _helpAction = new QAction("帮助(&H)", this);
  _helpAction->setShortcut(QKeySequence::HelpContents);
  _helpAction->setStatusTip("显示帮助");
  connect(_helpAction, &QAction::triggered, this, &MainWindow::onHelp);
  helpMenu->addAction(_helpAction);

  helpMenu->addSeparator();

  _aboutAction = new QAction("关于(&A)", this);
  _aboutAction->setStatusTip("关于此应用程序");
  connect(_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
  helpMenu->addAction(_aboutAction);
}

void MainWindow::onAddModel()
{
  if (_platerWidget)
  {
    _platerWidget->addModel();
  }
  _statusLabel->setText("添加模型");
}

void MainWindow::onRemoveModel()
{
  if (_platerWidget)
  {
    _platerWidget->removeSelectedModels();
  }
  _statusLabel->setText("删除模型");
}

void MainWindow::onImportFile()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, "导入文件", "", FileImporter::getFileFilter());

  if (!fileName.isEmpty())
  {
    // 使用文件导入器导入文件
    FileImporter importer;
    ImportOptions options;
    ImportResult result = importer.importFile(fileName, options);

    if (result.success)
    {
      // 将模型添加到场景
      SceneManagerV2* sceneManager = SceneManagerV2::getInstance();
      SceneObjectV2* sceneObj = new SceneObjectV2(QFileInfo(fileName).fileName(), result.actor);

      // 设置元数据
      ObjectMetadata metadata;
      metadata.filename = QFileInfo(fileName).fileName();
      metadata.filePath = fileName;
      metadata.fileSize = QFileInfo(fileName).size();
      metadata.meshInfo = result.meshInfo;
      metadata.volume = result.volume;
      metadata.weight = result.weight;

      FileType fileType = FileImporter::getFileType(fileName);
      switch (fileType)
      {
        case FileType::STL:
          metadata.fileType = "STL";
          break;
        case FileType::OBJ:
          metadata.fileType = "OBJ";
          break;
        case FileType::PLY:
          metadata.fileType = "PLY";
          break;
        case FileType::THREE:
          metadata.fileType = "3MF";
          break;
        case FileType::AMF:
          metadata.fileType = "AMF";
          break;
        case FileType::IMAGE:
          metadata.fileType = "Image";
          break;
        default:
          metadata.fileType = "Unknown";
          break;
      }

      sceneManager->addObject(sceneObj);

      _statusLabel->setText("成功导入: " + fileName);
    }
    else
    {
      QMessageBox::critical(this, "导入错误", result.errorMessage);
      _statusLabel->setText("导入失败: " + result.errorMessage);
    }
  }
}

void MainWindow::onExportFile()
{
  // TODO: 实现导出功能
  _statusLabel->setText("导出功能尚未实现");
}

void MainWindow::onNewProject()
{
  // 确认是否保存当前项目
  if (_sceneManager && !_sceneManager->rootObjects().isEmpty())
  {
    QMessageBox::StandardButton reply = QMessageBox::question(
      this, "新建项目", "当前项目尚未保存，是否继续？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
      return;
  }

  // 清空场景 - 删除所有根对象
  if (_sceneManager)
  {
    auto rootObjects = _sceneManager->rootObjects();
    _sceneManager->removeObjects(rootObjects);
  }

  _currentProjectFile.clear();
  _statusLabel->setText("新建项目");
  setWindowTitle("MasterWay - 新项目");
}

void MainWindow::onSaveProject()
{
  if (_currentProjectFile.isEmpty())
  {
    onSaveAsProject();
  }
  else
  {
    // TODO: 实现项目保存逻辑
    _statusLabel->setText("项目已保存: " + _currentProjectFile);
    setWindowTitle("MasterWay - " + QFileInfo(_currentProjectFile).fileName());
  }
}

void MainWindow::onSaveAsProject()
{
  QString fileName =
    QFileDialog::getSaveFileName(this, "保存项目", QDir::homePath(), "MasterWay 项目文件 (*.mwp)");

  if (!fileName.isEmpty())
  {
    _currentProjectFile = fileName;
    // TODO: 实现项目保存逻辑
    _statusLabel->setText("项目已保存: " + fileName);
    setWindowTitle("MasterWay - " + QFileInfo(fileName).fileName());
  }
}

void MainWindow::onAbout()
{
  QMessageBox::about(this, "关于 MasterWay",
    "MasterWay 是一款基于VTK开发的光固化3D打印前处理软件。\n\n"
    "功能特点：\n"
    "- 支持多种3D模型格式导入\n"
    "- 专业的3D视图控制\n"
    "- 模型变换操作\n"
    "- 构建平台管理\n"
    "- 支撑生成\n"
    "- 自动布局\n"
    "- 切片预览\n\n"
    "版本: 1.0.0\n"
    "构建时间: " __DATE__ " " __TIME__);
}

void MainWindow::onHelp()
{
  QMessageBox::information(this, "帮助",
    "MasterWay 使用说明\n\n"
    "基本操作：\n"
    "1. 点击'添加模型'按钮导入3D模型\n"
    "2. 使用鼠标左键旋转视图\n"
    "3. 使用鼠标右键平移视图\n"
    "4. 使用鼠标滚轮缩放视图\n"
    "5. 选择模型后可以进行平移、旋转和缩放操作\n\n"
    "快捷键：\n"
    "Ctrl+O: 打开文件\n"
    "Ctrl+S: 保存\n"
    "Delete: 删除选中的模型\n"
    "Ctrl+Z: 撤销\n"
    "Ctrl+Y: 重做\n"
    "1-6: 切换视图\n\n"
    "更多帮助请参考用户手册。");
}

void MainWindow::exit()
{
  qDebug() << "exiting...";

  QApplication::exit();
}
