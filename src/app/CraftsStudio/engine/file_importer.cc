#include "file_importer.h"

#include <QApplication>
#include <QDateTime>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

#include <vtkSTLReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageThreshold.h>
#include <vtkMarchingCubes.h>
#include <vtkTransform.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkMath.h>
#include <vtkMassProperties.h>
#include <vtkCenterOfMass.h>
#include <vtkOutlineFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkWindowToImageFilter.h>

// 静态成员初始化
QMap<QString, FileType> FileImporter::s_fileTypeMap;

FileImporter::FileImporter(QObject* parent)
  : QObject(parent)
{
  // 初始化文件类型映射
  if (s_fileTypeMap.isEmpty())
  {
    s_fileTypeMap["stl"] = FileType::STL;
    s_fileTypeMap["obj"] = FileType::OBJ;
    s_fileTypeMap["ply"] = FileType::PLY;
    s_fileTypeMap["3mf"] = FileType::THREE;
    s_fileTypeMap["amf"] = FileType::AMF;
    s_fileTypeMap["gcode"] = FileType::GCODE;
    s_fileTypeMap["gc"] = FileType::GCODE;
    s_fileTypeMap["png"] = FileType::IMAGE;
    s_fileTypeMap["jpg"] = FileType::IMAGE;
    s_fileTypeMap["jpeg"] = FileType::IMAGE;
    s_fileTypeMap["bmp"] = FileType::IMAGE;
    s_fileTypeMap["tiff"] = FileType::IMAGE;
    s_fileTypeMap["tif"] = FileType::IMAGE;
  }
  
  m_totalImports = 0;
  m_successfulImports = 0;
  m_failedImports = 0;
}

FileImporter::~FileImporter()
{
}

QStringList FileImporter::getSupportedExtensions()
{
  QStringList extensions;
  
  for (auto it = s_fileTypeMap.begin(); it != s_fileTypeMap.end(); ++it)
  {
    extensions << it.key();
  }
  
  return extensions;
}

QString FileImporter::getFileFilter()
{
  return QString(
    "3D模型文件 (*.stl *.obj *.ply *.3mf *.amf);;"
    "STL文件 (*.stl);;"
    "OBJ文件 (*.obj);;"
    "PLY文件 (*.ply);;"
    "3MF文件 (*.3mf);;"
    "AMF文件 (*.amf);;"
    "图像文件 (*.png *.jpg *.jpeg *.bmp *.tiff *.tif);;"
    "所有文件 (*.*)"
  );
}

FileType FileImporter::getFileType(const QString& fileName)
{
  QFileInfo fileInfo(fileName);
  QString suffix = fileInfo.suffix().toLower();
  
  return s_fileTypeMap.value(suffix, FileType::Unknown);
}

bool FileImporter::isSupported(const QString& fileName)
{
  return getFileType(fileName) != FileType::Unknown;
}

ImportResult FileImporter::importFile(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  QTime startTime = QTime::currentTime();
  
  // 检查文件是否存在
  if (!QFileInfo::exists(fileName))
  {
    result.success = false;
    result.errorMessage = "文件不存在: " + fileName;
    return result;
  }
  
  // 获取文件信息
  QFileInfo fileInfo(fileName);
  result.fileName = fileName;
  result.fileSize = fileInfo.size();
  result.fileType = getFileType(fileName);
  
  emit importStarted(fileName);
  
  // 根据文件类型选择导入方法
  switch (result.fileType)
  {
    case FileType::STL:
      result = importSTL(fileName, options);
      break;
      
    case FileType::OBJ:
      result = importOBJ(fileName, options);
      break;
      
    case FileType::PLY:
      result = importPLY(fileName, options);
      break;
      
    case FileType::THREE:
      result = import3MF(fileName, options);
      break;
      
    case FileType::AMF:
      result = importAMF(fileName, options);
      break;
      
    case FileType::IMAGE:
      result = importImage(fileName, options);
      break;
      
    case FileType::GCODE:
    case FileType::Unknown:
    default:
      result.success = false;
      result.errorMessage = "不支持的文件类型";
      break;
  }
  
  // 计算处理时间
  result.processingTime = getProcessingTime(startTime);
  
  // 更新统计
  m_totalImports++;
  if (result.success)
  {
    m_successfulImports++;
  }
  else
  {
    m_failedImports++;
  }
  
  emit importFinished(fileName, result);
  
  return result;
}

QList<ImportResult> FileImporter::importFiles(const QStringList& fileNames, const ImportOptions& options)
{
  QList<ImportResult> results;
  
  // 创建进度对话框
  QProgressDialog progress("正在导入文件...", "取消", 0, fileNames.size(), nullptr);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(0);
  
  for (int i = 0; i < fileNames.size(); ++i)
  {
    // 检查是否取消
    if (progress.wasCanceled())
    {
      break;
    }
    
    progress.setValue(i);
    progress.setLabelText(QString("正在导入: %1").arg(QFileInfo(fileNames[i]).fileName()));
    
    // 导入文件
    ImportResult result = importFile(fileNames[i], options);
    results.append(result);
    
    // 显示进度
    emit importProgress(fileNames[i], i * 100 / fileNames.size());
  }
  
  progress.setValue(fileNames.size());
  
  return results;
}

vtkSmartPointer<vtkPolyData> FileImporter::repairMesh(vtkSmartPointer<vtkPolyData> polyData, const MeshRepairOptions& options)
{
  if (!polyData)
    return nullptr;
    
  vtkSmartPointer<vtkPolyData> result = polyData;
  
  // 移除重复顶点
  if (options.removeDuplicates)
  {
    vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
    cleaner->SetInputData(result);
    cleaner->SetAbsoluteTolerance(options.distanceThreshold);
    cleaner->Update();
    result = cleaner->GetOutput();
  }
  
  // 确保网格为三角形
  vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangleFilter->SetInputData(result);
  triangleFilter->Update();
  result = triangleFilter->GetOutput();
  
  // 填充孔洞
  if (options.fillHoles)
  {
    vtkSmartPointer<vtkFillHolesFilter> holeFiller = vtkSmartPointer<vtkFillHolesFilter>::New();
    holeFiller->SetInputData(result);
    holeFiller->SetHoleSize(1000.0);
    holeFiller->Update();
    result = holeFiller->GetOutput();
  }
  
  // 修复法向量
  if (options.fixNormals)
  {
    vtkSmartPointer<vtkPolyDataNormals> normalFixer = vtkSmartPointer<vtkPolyDataNormals>::New();
    normalFixer->SetInputData(result);
    normalFixer->AutoOrientNormalsOn();
    normalFixer->SplittingOff();
    normalFixer->Update();
    result = normalFixer->GetOutput();
  }
  
  // 移除退化面
  if (options.removeDegenerate)
  {
    vtkSmartPointer<vtkCleanPolyData> degenerateRemover = vtkSmartPointer<vtkCleanPolyData>::New();
    degenerateRemover->SetInputData(result);
    degenerateRemover->PointMergingOff();
    degenerateRemover->Update();
    result = degenerateRemover->GetOutput();
  }
  
  return result;
}

void FileImporter::calculateMeshInfo(vtkSmartPointer<vtkPolyData> polyData, QString& meshInfo, double& volume)
{
  if (!polyData)
    return;
    
  // 获取顶点和面数
  vtkIdType numPoints = polyData->GetNumberOfPoints();
  vtkIdType numPolys = polyData->GetNumberOfPolys();
  
  meshInfo = QString("顶点: %1, 面: %2").arg(numPoints).arg(numPolys);
  
  // 计算体积
  if (numPolys > 0)
  {
    vtkSmartPointer<vtkMassProperties> massProperties = vtkSmartPointer<vtkMassProperties>::New();
    massProperties->SetInputData(polyData);
    massProperties->Update();
    
    volume = massProperties->GetVolume();
    
    // 如果体积为负，取绝对值
    if (volume < 0)
      volume = -volume;
  }
  else
  {
    volume = 0.0;
  }
}

vtkSmartPointer<vtkPolyData> FileImporter::convertUnits(vtkSmartPointer<vtkPolyData> polyData, const QString& fromUnit, const QString& toUnit)
{
  if (!polyData || fromUnit == toUnit)
    return polyData;
    
  // 单位转换因子（相对于毫米）
  QMap<QString, double> unitFactors;
  unitFactors["mm"] = 1.0;
  unitFactors["cm"] = 10.0;
  unitFactors["m"] = 1000.0;
  unitFactors["in"] = 25.4;
  unitFactors["ft"] = 304.8;
  unitFactors["µm"] = 0.001;
  
  double fromFactor = unitFactors.value(fromUnit, 1.0);
  double toFactor = unitFactors.value(toUnit, 1.0);
  
  double scaleFactor = fromFactor / toFactor;
  
  // 创建变换
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Scale(scaleFactor, scaleFactor, scaleFactor);
  
  // 应用变换
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputData(polyData);
  transformFilter->SetTransform(transform);
  transformFilter->Update();
  
  return transformFilter->GetOutput();
}

ImportOptions FileImporter::showImportOptionsDialog(const QString& fileName, QWidget* parent)
{
  QDialog dialog(parent);
  dialog.setWindowTitle("导入选项 - " + QFileInfo(fileName).fileName());
  dialog.resize(400, 500);
  
  QVBoxLayout* layout = new QVBoxLayout(&dialog);
  
  // 基本选项组
  QGroupBox* basicGroup = new QGroupBox("基本选项");
  QFormLayout* basicLayout = new QFormLayout(basicGroup);
  
  QCheckBox* centerCheckBox = new QCheckBox();
  centerCheckBox->setChecked(true);
  basicLayout->addRow("居中模型:", centerCheckBox);
  
  QCheckBox* autoScaleCheckBox = new QCheckBox();
  autoScaleCheckBox->setChecked(false);
  basicLayout->addRow("自动缩放:", autoScaleCheckBox);
  
  QCheckBox* flipNormalsCheckBox = new QCheckBox();
  flipNormalsCheckBox->setChecked(false);
  basicLayout->addRow("翻转法向量:", flipNormalsCheckBox);
  
  QCheckBox* convertToMMCheckBox = new QCheckBox();
  convertToMMCheckBox->setChecked(true);
  basicLayout->addRow("转换为毫米:", convertToMMCheckBox);
  
  QDoubleSpinBox* scaleFactorSpinBox = new QDoubleSpinBox();
  scaleFactorSpinBox->setRange(0.001, 1000.0);
  scaleFactorSpinBox->setValue(1.0);
  scaleFactorSpinBox->setSingleStep(0.1);
  scaleFactorSpinBox->setSuffix("x");
  basicLayout->addRow("缩放因子:", scaleFactorSpinBox);
  
  // 旋转组
  QGroupBox* rotationGroup = new QGroupBox("旋转");
  QFormLayout* rotationLayout = new QFormLayout(rotationGroup);
  
  QDoubleSpinBox* rotXSpinBox = new QDoubleSpinBox();
  rotXSpinBox->setRange(-180.0, 180.0);
  rotXSpinBox->setValue(0.0);
  rotXSpinBox->setSuffix("°");
  rotationLayout->addRow("X轴旋转:", rotXSpinBox);
  
  QDoubleSpinBox* rotYSpinBox = new QDoubleSpinBox();
  rotYSpinBox->setRange(-180.0, 180.0);
  rotYSpinBox->setValue(0.0);
  rotYSpinBox->setSuffix("°");
  rotationLayout->addRow("Y轴旋转:", rotYSpinBox);
  
  QDoubleSpinBox* rotZSpinBox = new QDoubleSpinBox();
  rotZSpinBox->setRange(-180.0, 180.0);
  rotZSpinBox->setValue(0.0);
  rotZSpinBox->setSuffix("°");
  rotationLayout->addRow("Z轴旋转:", rotZSpinBox);
  
  // 修复选项组
  QGroupBox* repairGroup = new QGroupBox("网格修复");
  QFormLayout* repairLayout = new QFormLayout(repairGroup);
  
  QCheckBox* repairMeshCheckBox = new QCheckBox();
  repairMeshCheckBox->setChecked(true);
  repairLayout->addRow("修复网格:", repairMeshCheckBox);
  
  QCheckBox* mergeVerticesCheckBox = new QCheckBox();
  mergeVerticesCheckBox->setChecked(true);
  repairLayout->addRow("合并顶点:", mergeVerticesCheckBox);
  
  QCheckBox* smoothMeshCheckBox = new QCheckBox();
  smoothMeshCheckBox->setChecked(false);
  repairLayout->addRow("平滑网格:", smoothMeshCheckBox);
  
  QCheckBox* generateNormalsCheckBox = new QCheckBox();
  generateNormalsCheckBox->setChecked(false);
  repairLayout->addRow("生成法向量:", generateNormalsCheckBox);
  
  // 图像选项组（如果适用）
  QGroupBox* imageGroup = new QGroupBox("图像选项");
  QFormLayout* imageLayout = new QFormLayout(imageGroup);
  
  FileType fileType = getFileType(fileName);
  if (fileType == FileType::IMAGE)
  {
    imageGroup->setVisible(true);
    
    QDoubleSpinBox* imageDepthSpinBox = new QDoubleSpinBox();
    imageDepthSpinBox->setRange(0.1, 100.0);
    imageDepthSpinBox->setValue(10.0);
    imageDepthSpinBox->setSuffix(" mm");
    imageLayout->addRow("模型深度:", imageDepthSpinBox);
    
    QSpinBox* imageResolutionSpinBox = new QSpinBox();
    imageResolutionSpinBox->setRange(10, 1000);
    imageResolutionSpinBox->setValue(100);
    imageResolutionSpinBox->setSuffix("%");
    imageLayout->addRow("图像分辨率:", imageResolutionSpinBox);
    
    QDoubleSpinBox* thresholdSpinBox = new QDoubleSpinBox();
    thresholdSpinBox->setRange(0.0, 255.0);
    thresholdSpinBox->setValue(128.0);
    imageLayout->addRow("阈值:", thresholdSpinBox);
  }
  else
  {
    imageGroup->setVisible(false);
  }
  
  // 按钮组
  QHBoxLayout* buttonLayout = new QHBoxLayout();
  QPushButton* okButton = new QPushButton("确定");
  QPushButton* cancelButton = new QPushButton("取消");
  buttonLayout->addStretch();
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
  
  // 添加到布局
  layout->addWidget(basicGroup);
  layout->addWidget(rotationGroup);
  layout->addWidget(repairGroup);
  layout->addWidget(imageGroup);
  layout->addStretch();
  layout->addLayout(buttonLayout);
  
  // 连接信号
  connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
  connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
  
  // 显示对话框
  if (dialog.exec() == QDialog::Accepted)
  {
    ImportOptions options;
    options.centerModel = centerCheckBox->isChecked();
    options.autoScale = autoScaleCheckBox->isChecked();
    options.flipNormals = flipNormalsCheckBox->isChecked();
    options.convertToMM = convertToMMCheckBox->isChecked();
    options.scaleFactor = scaleFactorSpinBox->value();
    options.rotationX = rotXSpinBox->value();
    options.rotationY = rotYSpinBox->value();
    options.rotationZ = rotZSpinBox->value();
    options.repairMesh = repairMeshCheckBox->isChecked();
    options.mergeVertices = mergeVerticesCheckBox->isChecked();
    options.smoothMesh = smoothMeshCheckBox->isChecked();
    options.generateNormals = generateNormalsCheckBox->isChecked();
    
    if (fileType == FileType::IMAGE)
    {
      // 获取图像选项
      // ...
    }
    
    return options;
  }
  
  return ImportOptions(); // 返回默认选项
}

ImportResult FileImporter::importSTL(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  
  try
  {
    // 创建STL读取器
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();
    
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
    
    // 记录原始顶点和面数
    result.originalVertices = polyData->GetNumberOfPoints();
    result.originalTriangles = polyData->GetNumberOfPolys();
    
    // 预处理模型
    polyData = preprocessModel(polyData, options);
    
    // 应用变换
    polyData = applyTransform(polyData, options);
    
    // 后处理模型
    polyData = postprocessModel(polyData, options);
    
    // 记录最终顶点和面数
    result.finalVertices = polyData->GetNumberOfPoints();
    result.finalTriangles = polyData->GetNumberOfPolys();
    
    // 计算模型信息
    calculateMeshInfo(polyData, result.meshInfo, result.volume);
    
    // 计算重量（假设材料为PLA，密度约1.24 g/cm³）
    result.weight = result.volume * 1.24 * 0.000001; // mm³ 转换为 g
    
    // 创建Actor
    result.actor = createActor(polyData);
    result.polyData = polyData;
    result.success = true;
  }
  catch (const std::exception& e)
  {
    result.success = false;
    result.errorMessage = QString("STL导入错误: %1").arg(e.what());
  }
  
  return result;
}

ImportResult FileImporter::importOBJ(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  
  try
  {
    // 创建OBJ读取器
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();
    
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
    
    // 记录原始顶点和面数
    result.originalVertices = polyData->GetNumberOfPoints();
    result.originalTriangles = polyData->GetNumberOfPolys();
    
    // 预处理模型
    polyData = preprocessModel(polyData, options);
    
    // 应用变换
    polyData = applyTransform(polyData, options);
    
    // 后处理模型
    polyData = postprocessModel(polyData, options);
    
    // 记录最终顶点和面数
    result.finalVertices = polyData->GetNumberOfPoints();
    result.finalTriangles = polyData->GetNumberOfPolys();
    
    // 计算模型信息
    calculateMeshInfo(polyData, result.meshInfo, result.volume);
    
    // 计算重量（假设材料为PLA，密度约1.24 g/cm³）
    result.weight = result.volume * 1.24 * 0.000001; // mm³ 转换为 g
    
    // 创建Actor
    result.actor = createActor(polyData);
    result.polyData = polyData;
    result.success = true;
  }
  catch (const std::exception& e)
  {
    result.success = false;
    result.errorMessage = QString("OBJ导入错误: %1").arg(e.what());
  }
  
  return result;
}

ImportResult FileImporter::importPLY(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  
  try
  {
    // 创建PLY读取器
    vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();
    
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
    
    // 记录原始顶点和面数
    result.originalVertices = polyData->GetNumberOfPoints();
    result.originalTriangles = polyData->GetNumberOfPolys();
    
    // 预处理模型
    polyData = preprocessModel(polyData, options);
    
    // 应用变换
    polyData = applyTransform(polyData, options);
    
    // 后处理模型
    polyData = postprocessModel(polyData, options);
    
    // 记录最终顶点和面数
    result.finalVertices = polyData->GetNumberOfPoints();
    result.finalTriangles = polyData->GetNumberOfPolys();
    
    // 计算模型信息
    calculateMeshInfo(polyData, result.meshInfo, result.volume);
    
    // 计算重量（假设材料为PLA，密度约1.24 g/cm³）
    result.weight = result.volume * 1.24 * 0.000001; // mm³ 转换为 g
    
    // 创建Actor
    result.actor = createActor(polyData);
    result.polyData = polyData;
    result.success = true;
  }
  catch (const std::exception& e)
  {
    result.success = false;
    result.errorMessage = QString("PLY导入错误: %1").arg(e.what());
  }
  
  return result;
}

ImportResult FileImporter::import3MF(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  
  // TODO: 实现3MF导入
  result.success = false;
  result.errorMessage = "3MF导入暂未实现";
  
  return result;
}

ImportResult FileImporter::importAMF(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  
  // TODO: 实现AMF导入
  result.success = false;
  result.errorMessage = "AMF导入暂未实现";
  
  return result;
}

ImportResult FileImporter::importImage(const QString& fileName, const ImportOptions& options)
{
  ImportResult result;
  
  try
  {
    // 创建图像读取器工厂
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> reader;
    reader.TakeReference(readerFactory->CreateImageReader2(fileName.toStdString().c_str()));
    
    if (!reader)
    {
      result.success = false;
      result.errorMessage = "无法创建图像读取器";
      return result;
    }
    
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();
    
    vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();
    
    // 图像预处理
    if (options.smoothMesh)
    {
      vtkSmartPointer<vtkImageGaussianSmooth> smoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
      smoother->SetInputData(imageData);
      smoother->SetStandardDeviations(1.0, 1.0, 1.0);
      smoother->Update();
      imageData = smoother->GetOutput();
    }
    
    // 阈值处理
    vtkSmartPointer<vtkImageThreshold> threshold = vtkSmartPointer<vtkImageThreshold>::New();
    threshold->SetInputData(imageData);
    threshold->ThresholdByLower(options.threshold);
    threshold->ReplaceInOn();
    threshold->SetInValue(1);
    threshold->ReplaceOutOn();
    threshold->SetOutValue(0);
    threshold->Update();
    
    // 提取等值面
    vtkSmartPointer<vtkMarchingCubes> mcFilter = vtkSmartPointer<vtkMarchingCubes>::New();
    mcFilter->SetInputConnection(threshold->GetOutputPort());
    mcFilter->SetValue(0, 0.5);
    mcFilter->Update();
    
    vtkSmartPointer<vtkPolyData> polyData = mcFilter->GetOutput();
    
    // 记录原始顶点和面数
    result.originalVertices = polyData->GetNumberOfPoints();
    result.originalTriangles = polyData->GetNumberOfPolys();
    
    // 预处理模型
    polyData = preprocessModel(polyData, options);
    
    // 应用变换
    polyData = applyTransform(polyData, options);
    
    // 后处理模型
    polyData = postprocessModel(polyData, options);
    
    // 记录最终顶点和面数
    result.finalVertices = polyData->GetNumberOfPoints();
    result.finalTriangles = polyData->GetNumberOfPolys();
    
    // 计算模型信息
    calculateMeshInfo(polyData, result.meshInfo, result.volume);
    
    // 计算重量（假设材料为PLA，密度约1.24 g/cm³）
    result.weight = result.volume * 1.24 * 0.000001; // mm³ 转换为 g
    
    // 创建Actor
    result.actor = createActor(polyData);
    result.polyData = polyData;
    result.success = true;
  }
  catch (const std::exception& e)
  {
    result.success = false;
    result.errorMessage = QString("图像导入错误: %1").arg(e.what());
  }
  
  return result;
}

vtkSmartPointer<vtkPolyData> FileImporter::preprocessModel(vtkSmartPointer<vtkPolyData> polyData, const ImportOptions& options)
{
  if (!polyData)
    return nullptr;
    
  vtkSmartPointer<vtkPolyData> result = polyData;
  
  // 生成法向量
  if (options.generateNormals)
  {
    vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputData(result);
    normals->ComputePointNormalsOn();
    normals->ComputeCellNormalsOn();
    normals->Update();
    result = normals->GetOutput();
  }
  
  // 修复网格
  if (options.repairMesh)
  {
    MeshRepairOptions repairOptions;
    repairOptions.removeDuplicates = options.mergeVertices;
    repairOptions.fillHoles = true;
    repairOptions.fixNormals = options.flipNormals;
    repairOptions.removeDegenerate = true;
    
    result = repairMesh(result, repairOptions);
  }
  
  return result;
}

vtkSmartPointer<vtkPolyData> FileImporter::postprocessModel(vtkSmartPointer<vtkPolyData> polyData, const ImportOptions& options)
{
  if (!polyData)
    return nullptr;
    
  vtkSmartPointer<vtkPolyData> result = polyData;
  
  // 自动缩放
  if (options.autoScale)
  {
    result = autoScaleModel(result);
  }
  
  return result;
}

vtkSmartPointer<vtkActor> FileImporter::createActor(vtkSmartPointer<vtkPolyData> polyData)
{
  if (!polyData)
    return nullptr;
    
  // 创建Mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(polyData);
  mapper->ScalarVisibilityOff();
  
  // 创建Actor
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  
  // 设置属性
  vtkSmartPointer<vtkProperty> prop = actor->GetProperty();
  prop->SetColor(0.7, 0.7, 0.7); // 默认灰色
  prop->SetAmbient(0.1);
  prop->SetSpecular(0.5);
  prop->SetSpecularPower(20);
  
  return actor;
}

void FileImporter::getModelBounds(vtkSmartPointer<vtkPolyData> polyData, double bounds[6])
{
  if (!polyData)
  {
    for (int i = 0; i < 6; ++i)
    {
      bounds[i] = 0.0;
    }
    return;
  }
  
  polyData->GetBounds(bounds);
}

void FileImporter::getModelSize(vtkSmartPointer<vtkPolyData> polyData, double& sizeX, double& sizeY, double& sizeZ)
{
  double bounds[6];
  getModelBounds(polyData, bounds);
  
  sizeX = bounds[1] - bounds[0];
  sizeY = bounds[3] - bounds[2];
  sizeZ = bounds[5] - bounds[4];
}

void FileImporter::getModelCenter(vtkSmartPointer<vtkPolyData> polyData, double& centerX, double& centerY, double& centerZ)
{
  double bounds[6];
  getModelBounds(polyData, bounds);
  
  centerX = (bounds[0] + bounds[1]) / 2.0;
  centerY = (bounds[2] + bounds[3]) / 2.0;
  centerZ = (bounds[4] + bounds[5]) / 2.0;
}

vtkSmartPointer<vtkPolyData> FileImporter::applyTransform(vtkSmartPointer<vtkPolyData> polyData, const ImportOptions& options)
{
  if (!polyData)
    return nullptr;
    
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();
  
  // 应用缩放
  if (options.scaleFactor != 1.0)
  {
    transform->Scale(options.scaleFactor, options.scaleFactor, options.scaleFactor);
  }
  
  // 应用旋转
  if (options.rotationX != 0.0 || options.rotationY != 0.0 || options.rotationZ != 0.0)
  {
    transform->RotateX(options.rotationX);
    transform->RotateY(options.rotationY);
    transform->RotateZ(options.rotationZ);
  }
  
  // 居中模型
  if (options.centerModel)
  {
    double centerX, centerY, centerZ;
    getModelCenter(polyData, centerX, centerY, centerZ);
    transform->Translate(-centerX, -centerY, -centerZ);
  }
  
  // 应用变换
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputData(polyData);
  transformFilter->SetTransform(transform);
  transformFilter->Update();
  
  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> FileImporter::autoScaleModel(vtkSmartPointer<vtkPolyData> polyData, double targetSize)
{
  if (!polyData)
    return nullptr;
    
  // 获取模型大小
  double sizeX, sizeY, sizeZ;
  getModelSize(polyData, sizeX, sizeY, sizeZ);
  
  // 计算最大尺寸
  double maxSize = qMax(qMax(sizeX, sizeY), sizeZ);
  
  // 计算缩放因子
  double scaleFactor = targetSize / maxSize;
  
  // 应用缩放
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Scale(scaleFactor, scaleFactor, scaleFactor);
  
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputData(polyData);
  transformFilter->SetTransform(transform);
  transformFilter->Update();
  
  return transformFilter->GetOutput();
}

void FileImporter::showErrorMessage(const QString& title, const QString& message, QWidget* parent)
{
  QMessageBox::critical(parent, title, message);
}

double FileImporter::getProcessingTime(const QTime& startTime)
{
  return startTime.msecsTo(QTime::currentTime()) / 1000.0;
}