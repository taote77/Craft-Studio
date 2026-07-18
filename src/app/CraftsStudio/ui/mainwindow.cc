#include "mainwindow.h"
#include "plater_widget.h"
#include "project_tree.h"
#include "menu_controller.h"
#include "slice_preview_bridge.h"

#include "engine/file_importer.h"
#include <csengine/scene/scene_document.hpp>
#include <csengine/scene/vtk_converter.hpp>
#include <slicingcore/geometry/coord.hpp>

#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QQmlContext>
#include <QQuickWidget>
#include <QStatusBar>
#include <QTabWidget>

#include <vtkAppendPolyData.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOBJWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkSTLWriter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setupUI();
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
  return QMainWindow::contextMenuEvent(event);
}

void MainWindow::setupUI()
{
  this->resize(1200, 900);
  setupTabWidget();
  setupStatusBar();

  // Left dock: ProjectTree
  _left_dock = new QDockWidget("项目树", this);
  _left_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _left_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  _left_dock->setFloating(false);
  _left_dock->setMaximumWidth(400);
  addDockWidget(Qt::LeftDockWidgetArea, _left_dock);

  auto treeView = new ProjectTree(this);
  treeView->setSceneDocument(_platerWidget->getSceneDocument());
  _left_dock->setWidget(treeView);

  // Create menus (after platerWidget is ready)
  setupMenuBar();

  // Right-click context menu
  _right_button_menu = new QMenu(this);
  auto style_menu = _right_button_menu->addMenu("3D Interactor Style");

  auto action = new QAction("Interact Style");
  connect(action, &QAction::triggered, this, [this] {
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
    _platerWidget->getRenderWindow()->GetInteractor()->SetInteractorStyle(style);
  });
  style_menu->addAction(action);

  action = style_menu->addAction("Camera Track");
  connect(action, &QAction::triggered, this, [this] {
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    _platerWidget->getRenderWindow()->GetInteractor()->SetInteractorStyle(style);
  });
  style_menu->addAction(action);

  action = style_menu->addAction("Rubber Band Zoom");
  connect(action, &QAction::triggered, this, [this] {
    auto style = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
    _platerWidget->getRenderWindow()->GetInteractor()->SetInteractorStyle(style);
  });
  style_menu->addAction(action);
}

void MainWindow::setupMenuBar()
{
  _menuController = new MenuController(this);
  _menuController->setup(menuBar(), this, _platerWidget,
                         _platerWidget->getSceneDocument());
}

void MainWindow::setupStatusBar()
{
  QStatusBar* sb = statusBar();
  _statusLabel = new QLabel("就绪");
  _modelCountLabel = new QLabel("模型: 0");
  _renderTimeLabel = new QLabel("渲染时间: 0ms");
  _memoryUsageLabel = new QLabel("内存: 0MB");
  sb->addWidget(_statusLabel, 1);
  sb->addPermanentWidget(_modelCountLabel);
  sb->addPermanentWidget(_renderTimeLabel);
  sb->addPermanentWidget(_memoryUsageLabel);
}

void MainWindow::setupTabWidget()
{
  _centralTabWidget = new QTabWidget(this);
  setCentralWidget(_centralTabWidget);

  _platerWidget = new PlaterWidget(this);
  _centralTabWidget->addTab(_platerWidget, QString("构建平台"));

  connect(_platerWidget, &PlaterWidget::modelAdded, this, &MainWindow::onAddModel);
  connect(_platerWidget, &PlaterWidget::modelRemoved, this, &MainWindow::onRemoveModel);
  connect(_platerWidget, &PlaterWidget::slicingCompleted,
          this, &MainWindow::onSlicingCompleted);

  // Add G-code preview tab
  _gcodePreview = new QPlainTextEdit();
  _gcodePreview->setReadOnly(true);
  _gcodePreview->setFont(QFont("monospace", 10));
  _gcodePreview->setLineWrapMode(QPlainTextEdit::NoWrap);
  _centralTabWidget->addTab(_gcodePreview, QString("G-code 预览"));

  // Add 3D slice preview tab (QtQuick3D)
  _slicePreview3D = new QQuickWidget();
  _slicePreview3D->setResizeMode(QQuickWidget::SizeRootObjectToView);
  _previewBridge = new SlicePreviewBridge();
  _slicePreview3D->rootContext()->setContextProperty("bridge", _previewBridge);
  // QML path: try source tree first, fall back to build output
  QString qmlPath = QString("%1/../../src/app/CraftsStudio/ui/SlicePreview.qml")
                      .arg(QCoreApplication::applicationDirPath());
  if (!QFile::exists(qmlPath))
  {
    // Fallback: relative to working directory (project root)
    qmlPath = "src/app/CraftsStudio/ui/SlicePreview.qml";
  }
  _slicePreview3D->setSource(QUrl::fromLocalFile(qmlPath));
  _centralTabWidget->addTab(_slicePreview3D, QString("3D 预览"));
}

// ============================================================================
// Slots
// ============================================================================

void MainWindow::exit()
{
  QApplication::quit();
}

void MainWindow::onAddModel()
{
  // Update UI only — the actual addModel() was already triggered
  // by toolbar button or menu action. This slot handles the
  // modelAdded signal for status updates.
  int count = _platerWidget->getSceneDocument()->objects().size();
  _modelCountLabel->setText(QString("模型: %1").arg(count));
}

void MainWindow::onRemoveModel()
{
  _platerWidget->removeSelectedModels();
}

void MainWindow::onImportFile()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, "导入模型", QString(),
    "3D Models (*.stl *.obj *.ply *.3mf *.amf);;All Files (*)");

  if (fileName.isEmpty()) return;

  _statusLabel->setText("正在导入模型...");
  QApplication::processEvents();

  FileImporter importer;
  ImportOptions options = FileImporter::showImportOptionsDialog(fileName, this);
  ImportResult result = importer.importFile(fileName, options);

  if (result.success)
  {
    auto* doc = _platerWidget->getSceneDocument();
    slicing::TriangleMesh mesh = csengine::triangleMeshFromVTK(result.polyData);
    mesh = csengine::repairMesh(mesh);
    doc->addModelWithVTK(std::move(mesh), result.polyData, QFileInfo(fileName).fileName());

    if (_platerWidget->getRenderWindow())
      _platerWidget->getRenderWindow()->Render();

    _statusLabel->setText("成功导入: " + fileName);
    onAddModel();
  }
  else
  {
    QMessageBox::critical(this, "导入错误", result.errorMessage);
    _statusLabel->setText("导入失败: " + result.errorMessage);
  }
}

void MainWindow::onNewProject()
{
  auto* doc = _platerWidget->getSceneDocument();
  if (doc && !doc->objects().isEmpty())
  {
    auto reply = QMessageBox::question(
      this, "新建项目", "当前项目尚未保存，是否继续？",
      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;
  }

  if (doc)
  {
    auto objects = doc->objects();
    doc->removeModels(objects);
  }

  _currentProjectFile.clear();
  _statusLabel->setText("新建项目");
}

void MainWindow::onSaveProject()
{
  if (_currentProjectFile.isEmpty())
  {
    onSaveAsProject();
    return;
  }

  auto* doc = _platerWidget->getSceneDocument();
  auto* platform = _platerWidget->getBuildPlatform();
  if (!doc)
    return;

  QJsonObject root;

  // Build platform settings
  QJsonObject platformObj;
  if (platform)
  {
    platformObj["width"] = platform->width();
    platformObj["height"] = platform->height();
    platformObj["depth"] = platform->depth();
    platformObj["gridSpacing"] = platform->gridSpacing();
    root["platform"] = platformObj;
  }

  // Models
  QJsonArray modelsArr;
  for (auto* obj : doc->objects())
  {
    QJsonObject modelObj;
    modelObj["name"] = QString::fromStdString(obj->name());
    modelObj["id"] = static_cast<qint64>(obj->id());
    modelObj["posX"] = obj->placement().posX;
    modelObj["posY"] = obj->placement().posY;
    modelObj["posZ"] = obj->placement().posZ;
    modelObj["rotX"] = obj->placement().rotX;
    modelObj["rotY"] = obj->placement().rotY;
    modelObj["rotZ"] = obj->placement().rotZ;
    modelObj["scaleX"] = obj->placement().scaleX;
    modelObj["scaleY"] = obj->placement().scaleY;
    modelObj["scaleZ"] = obj->placement().scaleZ;
    if (obj->hasSupport())
      modelObj["hasSupport"] = true;
    modelsArr.append(modelObj);
  }
  root["models"] = modelsArr;

  // Write to file
  QFile file(_currentProjectFile);
  if (!file.open(QIODevice::WriteOnly))
  {
    QMessageBox::critical(this, "保存失败",
      QString("无法写入文件: %1").arg(file.errorString()));
    _statusLabel->setText("保存失败");
    return;
  }

  QJsonDocument docJson(root);
  file.write(docJson.toJson(QJsonDocument::Indented));
  file.close();

  _statusLabel->setText("项目已保存: " + QFileInfo(_currentProjectFile).fileName());
}

void MainWindow::onSaveAsProject()
{
  QString fileName = QFileDialog::getSaveFileName(
    this, "保存项目", QString(), "CraftsStudio Project (*.csproj);;All Files (*)");

  if (fileName.isEmpty())
    return;

  _currentProjectFile = fileName;
  onSaveProject();
}

void MainWindow::onExportFile()
{
  auto* doc = _platerWidget->getSceneDocument();
  if (!doc || doc->objects().isEmpty())
  {
    _statusLabel->setText("没有模型可以导出");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
    this, "导出模型", QString(),
    "STL Files (*.stl);;OBJ Files (*.obj);;All Files (*)");

  if (fileName.isEmpty())
    return;

  QString ext = QFileInfo(fileName).suffix().toLower();

  if (ext == "stl")
  {
    // Export all visible model actors to a merged STL
    // Use VTK's STL writer on the combined polydata
    vtkSmartPointer<vtkAppendPolyData> appendFilter =
      vtkSmartPointer<vtkAppendPolyData>::New();

    for (auto* obj : doc->objects())
    {
      auto* actor = doc->actorForObject(obj);
      if (actor)
      {
        auto* mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
        if (mapper && mapper->GetInput())
        {
          // Transform the polydata by the actor's transform
          vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
          auto* userTransform = actor->GetUserTransform();
          if (userTransform)
          {
            transformFilter->SetTransform(userTransform);
            transformFilter->SetInputData(mapper->GetInput());
          }
          else
          {
            transformFilter->SetInputData(mapper->GetInput());
          }
          appendFilter->AddInputConnection(transformFilter->GetOutputPort());
        }
      }
    }

    appendFilter->Update();

    vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
    writer->SetFileName(fileName.toStdString().c_str());
    writer->SetInputConnection(appendFilter->GetOutputPort());
    writer->Write();

    _statusLabel->setText("STL 导出成功: " + QFileInfo(fileName).fileName());
  }
  else if (ext == "obj")
  {
    // Export merged OBJ
    vtkSmartPointer<vtkAppendPolyData> appendFilter =
      vtkSmartPointer<vtkAppendPolyData>::New();

    for (auto* obj : doc->objects())
    {
      auto* actor = doc->actorForObject(obj);
      if (actor)
      {
        auto* mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
        if (mapper && mapper->GetInput())
        {
          appendFilter->AddInputData(mapper->GetInput());
        }
      }
    }

    appendFilter->Update();

    vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
    writer->SetFileName(fileName.toStdString().c_str());
    writer->SetInputConnection(appendFilter->GetOutputPort());
    writer->Write();

    _statusLabel->setText("OBJ 导出成功: " + QFileInfo(fileName).fileName());
  }
  else
  {
    _statusLabel->setText("不支持的导出格式: " + ext);
    return;
  }
}

void MainWindow::onAbout()
{
  QMessageBox::about(this, "关于 CraftsStudio",
    "CraftsStudio - 3D打印预处理切片软件\n\n"
    "基于 C++20/Qt6/VTK 构建");
}

void MainWindow::onHelp()
{
  QMessageBox::information(this, "帮助",
    "快捷键:\n"
    "  Ctrl+O  导入模型\n"
    "  Ctrl+Z  撤销\n"
    "  Ctrl+Y  重做\n"
    "  Delete  删除选中模型\n"
    "  1-6     切换视图\n"
    "  Ctrl+G  生成支撑");
}

void MainWindow::onSlicingCompleted(int layerCount, const QString& gcodeText)
{
  _statusLabel->setText(QString("切片完成: %1 层").arg(layerCount));
  if (_gcodePreview && !gcodeText.isEmpty())
  {
    _gcodePreview->setPlainText(gcodeText);
    _centralTabWidget->setCurrentWidget(_gcodePreview);
  }
  onSliceDataReady();
}

void MainWindow::onSliceDataReady()
{
  if (!_previewBridge || !_platerWidget) return;

  auto* controller = _platerWidget->getSlicingController();
  auto* pipeline = controller ? controller->lastPipeline() : nullptr;
  if (!pipeline || pipeline->totalLayerCount() == 0) return;

  QVector<QVector<QVector3D>> allPoints;
  QVector<float> allZs;

  for (const auto& layer : pipeline->layers())
  {
    allZs.append(static_cast<float>(layer.zMm()));
    QVector<QVector3D> pts;
    for (const auto& expoly : layer.slices())
    {
      for (size_t i = 0; i < expoly.contour.size(); ++i)
      {
        const auto& p = expoly.contour[i];
        pts.append(QVector3D(
          static_cast<float>(slicing::unscale(p.x)),
          static_cast<float>(slicing::unscale(p.y)),
          static_cast<float>(layer.zMm())));
      }
    }
    allPoints.append(pts);
  }

  _previewBridge->loadSliceData(allPoints, allZs);
}
