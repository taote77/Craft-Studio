#ifndef RS_SCENE_OBJECT_V2_H
#define RS_SCENE_OBJECT_V2_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QVector3D>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkTransform.h>

// 打印设置
struct PrintSettings
{
  double layerHeight = 0.1;        // 层高(mm)
  double infillDensity = 0.2;      // 填充密度(0-1)
  int supportEnabled = true;        // 是否启用支撑
  double supportDensity = 0.15;     // 支撑密度(0-1)
  double supportZOffset = 0.2;     // 支撑Z轴偏移(mm)
  double raftEnabled = false;      // 是否启用筏板
  double brimWidth = 0.0;          // 边缘宽度(mm)
  double skirtCount = 1;           // 裙边圈数
  double skirtDistance = 6.0;      // 裙边距离(mm)
};

// 对象元数据
struct ObjectMetadata
{
  QString filename;               // 原始文件名
  QString filePath;               // 原始文件路径
  QDateTime importTime;          // 导入时间
  QDateTime lastModified;        // 最后修改时间
  long long fileSize = 0;        // 文件大小(字节)
  QString fileType;              // 文件类型(STL/OBJ等)
  QString meshInfo;              // 网格信息(顶点数、面数等)
  QString materialType;          // 材料类型
  double volume = 0.0;           // 体积(mm³)
  double weight = 0.0;           // 重量(g)
  double cost = 0.0;             // 成本
  double estimatedPrintTime = 0.0; // 预估打印时间(秒)
};

class SceneObjectV2 : public QObject
{
  Q_OBJECT

public:
  // 构造函数
  explicit SceneObjectV2(QString name, vtkSmartPointer<vtkActor> actor, QObject* parent = nullptr);
  
  // 析构函数
  ~SceneObjectV2();

  // 唯一标识符
  QString id() const { return _id; }
  void setId(const QString& id);

  // 显示名称
  QString name() const { return _name; }
  void setName(const QString& name);

  // 父对象
  SceneObjectV2* parent() const { return _parent; }
  void setParent(SceneObjectV2* parent);

  // 子对象列表
  QList<SceneObjectV2*> children() const { return _children; }

  // 层级操作：添加/移除子对象
  void addChild(SceneObjectV2* child);
  void removeChild(SceneObjectV2* child);

  // 选中状态
  bool isSelected() const { return _selected; }
  void setSelected(bool selected);

  // 可见性
  bool isVisible() const { return _visible; }
  void setVisible(bool visible);

  // 锁定状态
  bool isLocked() const { return _locked; }
  void setLocked(bool locked);

  // VTK渲染组件
  vtkSmartPointer<vtkActor> actor() const { return _actor; }
  void setActor(vtkSmartPointer<vtkActor> actor);
  
  vtkSmartPointer<vtkPolyData> polyData() const { return _polyData; }
  void setPolyData(vtkSmartPointer<vtkPolyData> polyData);
  
  vtkSmartPointer<vtkProperty> property() const { return _property; }
  void setProperty(vtkSmartPointer<vtkProperty> property);
  
  vtkSmartPointer<vtkTransform> transform() const { return _transform; }
  void setTransform(vtkSmartPointer<vtkTransform> transform);

  // 变换操作
  QVector3D position() const;
  void setPosition(const QVector3D& position);
  
  QVector3D rotation() const;
  void setRotation(const QVector3D& rotation);
  
  QVector3D scale() const;
  void setScale(const QVector3D& scale);
  
  void applyTransform();

  // 边界框
  void getBounds(double bounds[6]) const;
  void getLocalBounds(double bounds[6]) const;
  QVector3D getSize() const;
  QVector3D getCenter() const;
  
  // 元数据
  ObjectMetadata metadata() const { return _metadata; }
  void setMetadata(const ObjectMetadata& metadata);
  void updateMeshInfo();
  
  // 打印设置
  PrintSettings printSettings() const { return _printSettings; }
  void setPrintSettings(const PrintSettings& settings);
  
  // 支撑生成
  bool hasSupport() const { return _hasSupport; }
  void setHasSupport(bool hasSupport);
  
  // 复制对象
  SceneObjectV2* clone() const;

public slots:
  void onTransformChanged();

signals:
  // 属性变化信号
  void idChanged(const QString& newId);
  void nameChanged(const QString& newName);
  void visibilityChanged(bool visible);
  void selectedChanged(bool selected);
  void lockedChanged(bool locked);
  void geometryChanged();
  
  // 层级变化信号
  void childAdded(SceneObjectV2* child);
  void childRemoved(SceneObjectV2* child);
  void parentChanged(SceneObjectV2* newParent);
  
  // 变换信号
  void positionChanged(const QVector3D& newPosition);
  void rotationChanged(const QVector3D& newRotation);
  void scaleChanged(const QVector3D& newScale);
  void transformChanged();
  
  // 支撑信号
  void supportChanged(bool hasSupport);

private:
  QString generateUniqueId();
  void updateActorProperties();
  void updateTransform();

private:
  // 基本信息
  QString _id;                     // 唯一标识
  QString _name;                   // 显示名称
  SceneObjectV2* _parent;          // 父对象
  QList<SceneObjectV2*> _children; // 子对象列表

  // 状态属性
  bool _visible = true;   // 是否可见
  bool _selected = false;  // 是否选中
  bool _locked = false;   // 是否锁定

  // VTK渲染组件
  vtkSmartPointer<vtkActor> _actor;       // 渲染实体
  vtkSmartPointer<vtkPolyData> _polyData; // 几何数据
  vtkSmartPointer<vtkProperty> _property; // 外观属性
  vtkSmartPointer<vtkTransform> _transform; // 变换矩阵

  // 元数据
  ObjectMetadata _metadata;
  
  // 打印设置
  PrintSettings _printSettings;
  
  // 支撑
  bool _hasSupport = false;
  
  // 静态成员，用于生成唯一ID
  static uint64_t _autoId;
};

#endif // RS_SCENE_OBJECT_V2_H