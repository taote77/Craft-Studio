#include "rs_scene_object_v2.h"

#include <vtkBoundingBox.h>
#include <vtkPolyData.h>
#include <vtkTransformPolyDataFilter.h>

// 静态成员初始化
uint64_t SceneObjectV2::_autoId = 0;

SceneObjectV2::SceneObjectV2(QString name, vtkSmartPointer<vtkActor> actor, QObject* parent)
  : QObject(parent)
  , _name(name)
  , _parent(nullptr)
  , _actor(actor)
{
  _id = generateUniqueId();
  
  // 初始化VTK组件
  if (!_actor)
  {
    _actor = vtkSmartPointer<vtkActor>::New();
  }
  
  _property = _actor->GetProperty();
  _polyData = vtkSmartPointer<vtkPolyData>::New();
  _transform = vtkSmartPointer<vtkTransform>::New();
  
  // 设置默认属性
  _property->SetColor(0.7, 0.7, 0.7); // 默认灰色
  _property->SetAmbient(0.1);
  
  // 应用初始变换
  _actor->SetUserTransform(_transform);
  
  // 初始化元数据
  _metadata.importTime = QDateTime::currentDateTime();
  _metadata.lastModified = QDateTime::currentDateTime();
  
  // 更新网格信息
  updateMeshInfo();
}

SceneObjectV2::~SceneObjectV2()
{
  // VTK智能指针会自动释放资源
}

QString SceneObjectV2::generateUniqueId()
{
  return QString("obj_%1").arg(++_autoId);
}

void SceneObjectV2::setId(const QString& id)
{
  if (_id != id)
  {
    _id = id;
    emit idChanged(_id);
  }
}

void SceneObjectV2::setName(const QString& name)
{
  if (_name != name)
  {
    _name = name;
    emit nameChanged(_name);
  }
}

void SceneObjectV2::setParent(SceneObjectV2* parent)
{
  if (_parent != parent)
  {
    SceneObjectV2* oldParent = _parent;
    _parent = parent;
    emit parentChanged(parent);
    
    if (oldParent)
    {
      oldParent->removeChild(this);
    }
  }
}

void SceneObjectV2::addChild(SceneObjectV2* child)
{
  if (child && !_children.contains(child))
  {
    _children.append(child);
    child->setParent(this);
    emit childAdded(child);
  }
}

void SceneObjectV2::removeChild(SceneObjectV2* child)
{
  if (child && _children.removeOne(child))
  {
    child->setParent(nullptr);
    emit childRemoved(child);
  }
}

void SceneObjectV2::setSelected(bool selected)
{
  if (_selected != selected)
  {
    _selected = selected;
    updateActorProperties();
    emit selectedChanged(_selected);
  }
}

void SceneObjectV2::setVisible(bool visible)
{
  if (_visible != visible)
  {
    _visible = visible;
    if (_actor)
    {
      _actor->SetVisibility(visible);
    }
    emit visibilityChanged(_visible);
  }
}

void SceneObjectV2::setLocked(bool locked)
{
  if (_locked != locked)
  {
    _locked = locked;
    updateActorProperties();
    emit lockedChanged(_locked);
  }
}

void SceneObjectV2::setActor(vtkSmartPointer<vtkActor> actor)
{
  if (_actor != actor)
  {
    _actor = actor;
    if (_actor)
    {
      _property = _actor->GetProperty();
      _actor->SetUserTransform(_transform);
      updateActorProperties();
    }
  }
}

void SceneObjectV2::setPolyData(vtkSmartPointer<vtkPolyData> polyData)
{
  if (_polyData != polyData)
  {
    _polyData = polyData;
    updateMeshInfo();
    emit geometryChanged();
  }
}

void SceneObjectV2::setProperty(vtkSmartPointer<vtkProperty> property)
{
  if (_property != property)
  {
    _property = property;
    if (_actor)
    {
      _actor->SetProperty(_property);
    }
    updateActorProperties();
  }
}

void SceneObjectV2::setTransform(vtkSmartPointer<vtkTransform> transform)
{
  if (_transform != transform)
  {
    _transform = transform;
    if (_actor)
    {
      _actor->SetUserTransform(_transform);
    }
    updateTransform();
  }
}

QVector3D SceneObjectV2::position() const
{
  if (!_transform)
    return QVector3D(0, 0, 0);
    
  double* pos = _transform->GetPosition();
  return QVector3D(pos[0], pos[1], pos[2]);
}

void SceneObjectV2::setPosition(const QVector3D& position)
{
  if (!_transform)
    return;
    
  double currentPos[3];
  _transform->GetPosition(currentPos);
  
  if (qAbs(currentPos[0] - position.x()) > 0.001 ||
      qAbs(currentPos[1] - position.y()) > 0.001 ||
      qAbs(currentPos[2] - position.z()) > 0.001)
  {
    _transform->Identity();
    _transform->Translate(position.x(), position.y(), position.z());
    _transform->Modified();
    emit positionChanged(position);
    emit transformChanged();
  }
}

QVector3D SceneObjectV2::rotation() const
{
  if (!_transform)
    return QVector3D(0, 0, 0);
    
  double* ori = _transform->GetOrientation();
  return QVector3D(ori[0], ori[1], ori[2]);
}

void SceneObjectV2::setRotation(const QVector3D& rotation)
{
  if (!_transform)
    return;
    
  double currentOri[3];
  _transform->GetOrientation(currentOri);
  
  if (qAbs(currentOri[0] - rotation.x()) > 0.001 ||
      qAbs(currentOri[1] - rotation.y()) > 0.001 ||
      qAbs(currentOri[2] - rotation.z()) > 0.001)
  {
    _transform->Identity();
    _transform->RotateX(rotation.x());
    _transform->RotateY(rotation.y());
    _transform->RotateZ(rotation.z());
    _transform->Modified();
    emit rotationChanged(rotation);
    emit transformChanged();
  }
}

QVector3D SceneObjectV2::scale() const
{
  if (!_transform)
    return QVector3D(1, 1, 1);
    
  double* scale = _transform->GetScale();
  return QVector3D(scale[0], scale[1], scale[2]);
}

void SceneObjectV2::setScale(const QVector3D& scale)
{
  if (!_transform)
    return;
    
  double currentScale[3];
  _transform->GetScale(currentScale);
  
  if (qAbs(currentScale[0] - scale.x()) > 0.001 ||
      qAbs(currentScale[1] - scale.y()) > 0.001 ||
      qAbs(currentScale[2] - scale.z()) > 0.001)
  {
    _transform->Identity();
    _transform->Scale(scale.x(), scale.y(), scale.z());
    _transform->Modified();
    emit scaleChanged(scale);
    emit transformChanged();
  }
}

void SceneObjectV2::applyTransform()
{
  if (_transform)
  {
    _transform->Modified();
    emit transformChanged();
  }
}

void SceneObjectV2::getBounds(double bounds[6]) const
{
  if (_actor)
  {
    _actor->GetBounds(bounds);
  }
  else
  {
    bounds[0] = bounds[2] = bounds[4] = 0.0;
    bounds[1] = bounds[3] = bounds[5] = 0.0;
  }
}

void SceneObjectV2::getLocalBounds(double bounds[6]) const
{
  if (_polyData)
  {
    _polyData->GetBounds(bounds);
  }
  else if (_actor)
  {
    _actor->GetBounds(bounds);
  }
  else
  {
    bounds[0] = bounds[2] = bounds[4] = 0.0;
    bounds[1] = bounds[3] = bounds[5] = 0.0;
  }
}

QVector3D SceneObjectV2::getSize() const
{
  double bounds[6];
  getBounds(bounds);
  
  return QVector3D(
    bounds[1] - bounds[0],  // X size
    bounds[3] - bounds[2],  // Y size
    bounds[5] - bounds[4]   // Z size
  );
}

QVector3D SceneObjectV2::getCenter() const
{
  double bounds[6];
  getBounds(bounds);
  
  return QVector3D(
    (bounds[0] + bounds[1]) / 2.0,  // X center
    (bounds[2] + bounds[3]) / 2.0,  // Y center
    (bounds[4] + bounds[5]) / 2.0   // Z center
  );
}

void SceneObjectV2::setMetadata(const ObjectMetadata& metadata)
{
  _metadata = metadata;
}

void SceneObjectV2::updateMeshInfo()
{
  if (!_polyData)
    return;
    
  // 获取顶点和面数
  vtkIdType numPoints = _polyData->GetNumberOfPoints();
  vtkIdType numCells = _polyData->GetNumberOfCells();
  
  // 计算体积和边界框
  double bounds[6];
  _polyData->GetBounds(bounds);
  
  double volume = 0.0;
  if (numCells > 0)
  {
    // 简化的体积计算（仅用于演示）
    double dx = bounds[1] - bounds[0];
    double dy = bounds[3] - bounds[2];
    double dz = bounds[5] - bounds[4];
    volume = dx * dy * dz;
  }
  
  // 更新元数据
  _metadata.meshInfo = QString("Vertices: %1, Faces: %2").arg(numPoints).arg(numCells);
  _metadata.volume = volume;
  
  // 估算重量（假设材料为PLA，密度约1.24 g/cm³）
  _metadata.weight = volume * 0.00124 * 0.001; // mm³ 转换为 g
  
  emit geometryChanged();
}

void SceneObjectV2::setPrintSettings(const PrintSettings& settings)
{
  _printSettings = settings;
}

void SceneObjectV2::setHasSupport(bool hasSupport)
{
  if (_hasSupport != hasSupport)
  {
    _hasSupport = hasSupport;
    emit supportChanged(_hasSupport);
  }
}

SceneObjectV2* SceneObjectV2::clone() const
{
  // 创建新的Actor和变换
  vtkSmartPointer<vtkActor> newActor = vtkSmartPointer<vtkActor>::New();
  if (_actor)
  {
    // 复制属性
    newActor->SetProperty(_property);
    
    // 复制变换
    vtkSmartPointer<vtkTransform> newTransform = vtkSmartPointer<vtkTransform>::New();
    newTransform->SetMatrix(_transform->GetMatrix());
    newActor->SetUserTransform(newTransform);
    
    // 复制mapper
    if (_actor->GetMapper())
    {
      newActor->SetMapper(_actor->GetMapper());
    }
  }
  
  // 创建新对象
  SceneObjectV2* newObj = new SceneObjectV2(_name + "_copy", newActor);
  
  // 复制属性
  newObj->_visible = _visible;
  newObj->_selected = false; // 新对象默认不选中
  newObj->_locked = _locked;
  
  // 复制元数据
  newObj->_metadata = _metadata;
  newObj->_metadata.importTime = QDateTime::currentDateTime();
  newObj->_metadata.lastModified = QDateTime::currentDateTime();
  
  // 复制打印设置
  newObj->_printSettings = _printSettings;
  
  return newObj;
}

void SceneObjectV2::onTransformChanged()
{
  emit geometryChanged();
  _metadata.lastModified = QDateTime::currentDateTime();
}

void SceneObjectV2::updateActorProperties()
{
  if (!_actor || !_property)
    return;
    
  if (_selected)
  {
    // 选中状态：黄色高亮
    _property->SetColor(1.0, 0.8, 0.0);
    _property->SetAmbient(0.3);
  }
  else if (_locked)
  {
    // 锁定状态：暗红色
    _property->SetColor(0.5, 0.3, 0.3);
    _property->SetAmbient(0.2);
  }
  else
  {
    // 默认状态：灰色
    _property->SetColor(0.7, 0.7, 0.7);
    _property->SetAmbient(0.1);
  }
}

void SceneObjectV2::updateTransform()
{
  if (_transform)
  {
    _transform->Modified();
    emit transformChanged();
  }
}