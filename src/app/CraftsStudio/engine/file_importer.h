#ifndef FILE_IMPORTER_H
#define FILE_IMPORTER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QFileInfo>
#include <QMessageBox>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

// 支持的文件类型
enum class FileType
{
  Unknown,
  STL,     // STL文件
  OBJ,     // OBJ文件
  PLY,     // PLY文件
  THREE,   // 3MF文件
  AMF,     // AMF文件
  GCODE,   // G代码文件（用于预览）
  IMAGE    // 图像文件（用于生成模型）
};

// 导入选项
struct ImportOptions
{
  bool centerModel = true;        // 将模型居中
  bool autoScale = false;          // 自动缩放到合适大小
  bool flipNormals = false;        // 翻转法向量
  bool repairMesh = true;          // 修复网格
  bool mergeVertices = true;       // 合并重复顶点
  bool smoothMesh = false;         // 平滑网格
  bool generateNormals = false;    // 生成法向量
  double scaleFactor = 1.0;         // 缩放因子
  double rotationX = 0.0;          // X轴旋转角度（度）
  double rotationY = 0.0;          // Y轴旋转角度（度）
  double rotationZ = 0.0;          // Z轴旋转角度（度）
  bool convertToMM = true;         // 转换为毫米单位
  
  // 图像导入选项
  double imageDepth = 10.0;        // 图像生成的模型深度（mm）
  int imageResolution = 100;       // 图像分辨率降低比例
  double threshold = 128.0;        // 图像阈值
};

// 导入结果
struct ImportResult
{
  bool success = false;
  QString errorMessage;
  vtkSmartPointer<vtkPolyData> polyData;
  vtkSmartPointer<vtkActor> actor;
  QString fileName;
  FileType fileType;
  long long fileSize = 0;
  QString meshInfo;
  double volume = 0.0;
  double weight = 0.0;
  
  // 处理统计
  int originalVertices = 0;
  int originalTriangles = 0;
  int finalVertices = 0;
  int finalTriangles = 0;
  double processingTime = 0.0;
};

// 网格修复选项
struct MeshRepairOptions
{
  bool removeDuplicates = true;     // 移除重复顶点
  bool fillHoles = true;            // 填充孔洞
  bool fixNormals = true;           // 修复法向量
  bool removeDegenerate = true;     // 移除退化面
  double angleThreshold = 30.0;     // 角度阈值（度）
  double distanceThreshold = 0.001; // 距离阈值
};

class FileImporter : public QObject
{
  Q_OBJECT

public:
  explicit FileImporter(QObject* parent = nullptr);
  ~FileImporter();

  // 获取支持的文件扩展名
  static QStringList getSupportedExtensions();
  static QString getFileFilter();
  static FileType getFileType(const QString& fileName);
  
  // 检查文件是否受支持
  static bool isSupported(const QString& fileName);
  
  // 导入文件
  ImportResult importFile(const QString& fileName, const ImportOptions& options = ImportOptions());
  
  // 批量导入
  QList<ImportResult> importFiles(const QStringList& fileNames, const ImportOptions& options = ImportOptions());
  
  // 修复网格
  vtkSmartPointer<vtkPolyData> repairMesh(vtkSmartPointer<vtkPolyData> polyData, const MeshRepairOptions& options);
  
  // 计算模型信息
  void calculateMeshInfo(vtkSmartPointer<vtkPolyData> polyData, QString& meshInfo, double& volume);
  
  // 单位转换
  vtkSmartPointer<vtkPolyData> convertUnits(vtkSmartPointer<vtkPolyData> polyData, const QString& fromUnit, const QString& toUnit);

public slots:
  // 显示导入选项对话框
  static ImportOptions showImportOptionsDialog(const QString& fileName, QWidget* parent = nullptr);

signals:
  // 导入进度信号
  void importStarted(const QString& fileName);
  void importProgress(const QString& fileName, int progress);
  void importFinished(const QString& fileName, const ImportResult& result);
  void importError(const QString& fileName, const QString& error);

private:
  // 内部导入函数
  ImportResult importSTL(const QString& fileName, const ImportOptions& options);
  ImportResult importOBJ(const QString& fileName, const ImportOptions& options);
  ImportResult importPLY(const QString& fileName, const ImportOptions& options);
  ImportResult import3MF(const QString& fileName, const ImportOptions& options);
  ImportResult importAMF(const QString& fileName, const ImportOptions& options);
  ImportResult importImage(const QString& fileName, const ImportOptions& options);
  
  // 预处理模型
  vtkSmartPointer<vtkPolyData> preprocessModel(vtkSmartPointer<vtkPolyData> polyData, const ImportOptions& options);
  
  // 后处理模型
  vtkSmartPointer<vtkPolyData> postprocessModel(vtkSmartPointer<vtkPolyData> polyData, const ImportOptions& options);
  
  // 创建Actor
  vtkSmartPointer<vtkActor> createActor(vtkSmartPointer<vtkPolyData> polyData);
  
  // 计算模型边界
  void getModelBounds(vtkSmartPointer<vtkPolyData> polyData, double bounds[6]);
  
  // 计算模型大小
  void getModelSize(vtkSmartPointer<vtkPolyData> polyData, double& sizeX, double& sizeY, double& sizeZ);
  
  // 计算模型中心
  void getModelCenter(vtkSmartPointer<vtkPolyData> polyData, double& centerX, double& centerY, double& centerZ);
  
  // 应用变换
  vtkSmartPointer<vtkPolyData> applyTransform(vtkSmartPointer<vtkPolyData> polyData, const ImportOptions& options);
  
  // 自动缩放
  vtkSmartPointer<vtkPolyData> autoScaleModel(vtkSmartPointer<vtkPolyData> polyData, double targetSize = 100.0);
  
  // 显示错误消息
  void showErrorMessage(const QString& title, const QString& message, QWidget* parent = nullptr);
  
  // 计算处理时间
  double getProcessingTime(const QTime& startTime);

private:
  // 文件类型映射
  static QMap<QString, FileType> s_fileTypeMap;
  
  // 导入统计
  int m_totalImports = 0;
  int m_successfulImports = 0;
  int m_failedImports = 0;
};

#endif // FILE_IMPORTER_H