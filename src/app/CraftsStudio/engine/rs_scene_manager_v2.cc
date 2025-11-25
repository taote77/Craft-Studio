#include "rs_scene_manager_v2.h"

#include <vtkActor.h>
#include <vtkBoundingBox.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

SceneManagerV2* SceneManagerV2::getInstance()
{
  static SceneManagerV2 instance;
  return &instance;
}

SceneManagerV2::SceneManagerV2()
{
  // 初始化构建平台
  m_buildPlatform.width = 200.0;
  m_buildPlatform.height = 200.0;
  m_buildPlatform.depth = 0.0;
  m_buildPlatform.gridSpacing = 10.0;
  m_buildPlatform.gridVisible = true;
  m_buildPlatform.platformVisible = true;
}

// 获取所有根对象（组织树的顶层节点）
QList<SceneObjectV2*> SceneManagerV2::rootObjects()
{
  return m_rootObjects;
}

// 查找对象（通过ID快速定位）
SceneObjectV2* SceneManagerV2::findObject(const QString& id)
{
  return m_idToObject.value(id, nullptr);
}

// 添加对象（顶层对象直接加入根列表，子对象通过parent添加）
void SceneManagerV2::addObject(SceneObjectV2* obj, SceneObjectV2* parent)
{
  if (!obj || m_idToObject.contains(obj->id()))
    return;

  m_idToObject[obj->id()] = obj;

  // 更新Actor映射
  updateActorMapping(obj);

  // 连接信号（使用lambda处理无参数的geometryChanged信号）
  connect(
    obj, &SceneObjectV2::geometryChanged, this, [this, obj]() { onObjectGeometryChanged(obj); });

  if (parent)
  {
    parent->addChild(obj); // 子对象交给父对象管理
  }
  else
  {
    m_rootObjects.append(obj); // 根对象加入顶层列表
    emit rootObjectAdded(obj);
  }
  emit objectAdded(obj);
}

// 删除对象（递归删除子对象）
void SceneManagerV2::removeObject(SceneObjectV2* obj)
{
  if (!obj)
    return;

  // 从选中列表中移除
  if (m_selectedObjects.contains(obj))
  {
    m_selectedObjects.removeOne(obj);
    emit multiSelectionChanged(m_selectedObjects);
  }

  // 递归删除所有子对象
  for (auto child : obj->children())
  {
    removeObject(child);
  }

  // 从父对象或根列表中移除
  if (obj->parent())
  {
    obj->parent()->removeChild(obj);
  }
  else
  {
    m_rootObjects.removeOne(obj);
    emit rootObjectRemoved(obj);
  }

  // 移除映射
  removeActorMapping(obj);

  // 断开信号
  disconnect(obj, nullptr, this, nullptr);

  m_idToObject.remove(obj->id());
  emit objectRemoved(obj);
  delete obj; // 释放内存
}

// 批量删除对象
void SceneManagerV2::removeObjects(const QList<SceneObjectV2*>& objects)
{
  // 创建副本以避免迭代器失效
  QList<SceneObjectV2*> objectsToRemove = objects;

  for (auto obj : objectsToRemove)
  {
    removeObject(obj);
  }
}

// 更新对象属性（如可见性、选中状态）
void SceneManagerV2::setObjectVisible(SceneObjectV2* obj, bool visible)
{
  if (obj && obj->isVisible() != visible)
  {
    obj->setVisible(visible);
    emit objectVisibilityChanged(obj);
  }
}

// 批量设置可见性
void SceneManagerV2::setObjectsVisible(const QList<SceneObjectV2*>& objects, bool visible)
{
  for (auto obj : objects)
  {
    setObjectVisible(obj, visible);
  }
}

// 通过Actor查找对象
SceneObjectV2* SceneManagerV2::findObjectByActor(vtkActor* actor)
{
  return m_actorToObject.value(actor, nullptr);
}

// 设置变换模式
void SceneManagerV2::setTransformMode(TransformModeV2 mode)
{
  if (m_transformMode != mode)
  {
    m_transformMode = mode;
    emit transformModeChanged(mode);
  }
}

// 清空所有选中状态
void SceneManagerV2::clearSelection()
{
  if (!m_selectedObjects.isEmpty())
  {
    for (auto obj : m_selectedObjects)
    {
      obj->setSelected(false);
      // 恢复默认外观
      obj->property()->SetColor(0.7, 0.7, 0.7); // 默认灰色
      obj->property()->SetAmbient(0.1);
      emit objectSelectionChanged(obj);
    }

    m_selectedObjects.clear();
    emit multiSelectionChanged(m_selectedObjects);
    emit selectionChanged(nullptr);
  }
}

// 设置对象选中状态（支持多选）
void SceneManagerV2::setObjectSelected(SceneObjectV2* obj, bool selected, bool addToSelection)
{
  if (!obj)
    return;

  // 处理多选逻辑
  if (selected)
  {
    // 如果对象已在选中列表中，无需处理
    if (m_selectedObjects.contains(obj))
      return;

    // 如果不支持多选，先清空选择
    if (!m_multiSelectionEnabled || !addToSelection)
    {
      clearSelection();
    }

    // 添加到选中列表
    m_selectedObjects.append(obj);
    obj->setSelected(true);

    // 高亮显示
    obj->property()->SetColor(1.0, 0.8, 0.0); // 选中时黄色高亮
    obj->property()->SetAmbient(0.3);         // 增加环境光增强高亮效果
  }
  else
  {
    // 从选中列表中移除
    if (!m_selectedObjects.removeOne(obj))
      return; // 对象不在选中列表中

    obj->setSelected(false);

    // 恢复默认外观
    obj->property()->SetColor(0.7, 0.7, 0.7); // 默认灰色
    obj->property()->SetAmbient(0.1);
  }

  emit objectSelectionChanged(obj);
  emit multiSelectionChanged(m_selectedObjects);

  // 保持向后兼容
  SceneObjectV2* firstSelected = m_selectedObjects.isEmpty() ? nullptr : m_selectedObjects.first();
  emit selectionChanged(firstSelected);
}

// 选择所有对象
void SceneManagerV2::selectAll()
{
  clearSelection();

  for (auto obj : m_rootObjects)
  {
    setObjectSelected(obj, true, true); // 添加到选择中
  }
}

// 反选
void SceneManagerV2::invertSelection()
{
  QList<SceneObjectV2*> newSelection;

  // 获取所有对象
  QList<SceneObjectV2*> allObjects;
  for (auto rootObj : m_rootObjects)
  {
    allObjects.append(rootObj);
    // 递归添加所有子对象
    // 这里简化处理，实际应该实现递归函数
  }

  for (auto obj : allObjects)
  {
    if (obj->isSelected())
    {
      obj->setSelected(false);
      obj->property()->SetColor(0.7, 0.7, 0.7);
      obj->property()->SetAmbient(0.1);
      emit objectSelectionChanged(obj);
    }
    else
    {
      newSelection.append(obj);
    }
  }

  m_selectedObjects = newSelection;

  for (auto obj : m_selectedObjects)
  {
    obj->setSelected(true);
    obj->property()->SetColor(1.0, 0.8, 0.0);
    obj->property()->SetAmbient(0.3);
    emit objectSelectionChanged(obj);
  }

  emit multiSelectionChanged(m_selectedObjects);
  emit selectionChanged(m_selectedObjects.isEmpty() ? nullptr : m_selectedObjects.first());
}

void SceneManagerV2::selectObjectsInRect(const QRectF& rect)
{
  //
  //
}

// 设置对象平移
void SceneManagerV2::setObjectTranslation(SceneObjectV2* obj, double x, double y, double z)
{
  if (!obj)
    return;

  // 应用平移变换到对象的actor
  obj->actor()->SetPosition(x, y, z);

  // 发出几何变化信号
  emit objectGeometryChanged(obj);
}

// 设置对象旋转
void SceneManagerV2::setObjectRotation(SceneObjectV2* obj, double x, double y, double z)
{
  if (!obj)
    return;

  // 应用旋转变换到对象的actor
  obj->actor()->SetOrientation(x, y, z);

  // 发出几何变化信号
  emit objectGeometryChanged(obj);
}

// 设置对象缩放
void SceneManagerV2::setObjectScale(SceneObjectV2* obj, double x, double y, double z)
{
  if (!obj)
    return;

  // 应用缩放变换到对象的actor
  obj->actor()->SetScale(x, y, z);

  // 发出几何变化信号
  emit objectGeometryChanged(obj);
}

// 批量平移
void SceneManagerV2::setObjectsTranslation(
  const QList<SceneObjectV2*>& objects, double x, double y, double z)
{
  for (auto obj : objects)
  {
    setObjectTranslation(obj, x, y, z);
  }
}

// 批量旋转
void SceneManagerV2::setObjectsRotation(
  const QList<SceneObjectV2*>& objects, double x, double y, double z)
{
  for (auto obj : objects)
  {
    setObjectRotation(obj, x, y, z);
  }
}

// 批量缩放
void SceneManagerV2::setObjectsScale(
  const QList<SceneObjectV2*>& objects, double x, double y, double z)
{
  for (auto obj : objects)
  {
    setObjectScale(obj, x, y, z);
  }
}

// 设置构建平台
void SceneManagerV2::setBuildPlatform(const BuildPlatformParameters& platform)
{
  m_buildPlatform = platform;
  emit buildPlatformChanged();
}

// 更新构建平台可见性
void SceneManagerV2::updateBuildPlatformVisibility()
{
  // 实现构建平台的显示/隐藏逻辑
  emit buildPlatformChanged();
}

// 设置视图类型
void SceneManagerV2::setViewType(ViewType type)
{
  if (m_viewType != type)
  {
    m_viewType = type;
    emit viewTypeChanged(type);
  }
}

// 重置相机
void SceneManagerV2::resetCamera()
{
  emit cameraReset();
}

// 适配所有对象
void SceneManagerV2::fitAllObjects()
{
  emit cameraFitAll();
}

// 适配选中对象
void SceneManagerV2::fitSelectedObjects()
{
  emit cameraFitSelected();
}

// 获取场景边界
void SceneManagerV2::getSceneBounds(double bounds[6])
{
  vtkBoundingBox bbox;

  for (auto obj : m_rootObjects)
  {
    double objBounds[6];
    obj->actor()->GetBounds(objBounds);
    bbox.AddBounds(objBounds);
  }

  if (bbox.IsValid())
  {
    bbox.GetBounds(bounds);
  }
  else
  {
    // 无效边界，返回默认值
    bounds[0] = bounds[2] = bounds[4] = -10.0;
    bounds[1] = bounds[3] = bounds[5] = 10.0;
  }
}

// 获取选中对象边界
void SceneManagerV2::getSelectedObjectsBounds(double bounds[6])
{
  vtkBoundingBox bbox;

  for (auto obj : m_selectedObjects)
  {
    double objBounds[6];
    obj->actor()->GetBounds(objBounds);
    bbox.AddBounds(objBounds);
  }

  if (bbox.IsValid())
  {
    bbox.GetBounds(bounds);
  }
  else
  {
    // 无效边界，返回默认值
    bounds[0] = bounds[2] = bounds[4] = -10.0;
    bounds[1] = bounds[3] = bounds[5] = 10.0;
  }
}

// 自动布局对象
void SceneManagerV2::autoLayoutObjects(double spacing)
{
  if (m_rootObjects.isEmpty())
    return;

  arrangeInGrid(3); // 默认3列布局
}

// 网格布局
void SceneManagerV2::arrangeInGrid(int columns)
{
  if (m_rootObjects.isEmpty())
    return;

  double spacing = 5.0; // 默认间距
  double currentX = -m_buildPlatform.width / 2.0;
  double currentY = m_buildPlatform.height / 2.0;
  int col = 0;

  for (auto obj : m_rootObjects)
  {
    // 获取对象边界
    double bounds[6];
    obj->actor()->GetBounds(bounds);
    double objWidth = bounds[1] - bounds[0];
    double objDepth = bounds[3] - bounds[2];

    // 计算位置（使对象居中）
    double posX = currentX + objWidth / 2.0;
    double posY = currentY - objDepth / 2.0;
    double posZ = 0.0;

    // 应用位置
    setObjectTranslation(obj, posX, posY, posZ);

    // 更新下一个位置
    currentX += objWidth + spacing;
    col++;

    // 如果达到最大列数，换行
    if (col >= columns)
    {
      col = 0;
      currentX = -m_buildPlatform.width / 2.0;
      currentY -= getMaxObjectDepth(m_rootObjects) + spacing;
    }
  }
}

// 更新Actor映射
void SceneManagerV2::updateActorMapping(SceneObjectV2* obj)
{
  if (obj && obj->actor())
  {
    m_actorToObject[obj->actor()] = obj;
  }
}

// 移除Actor映射
void SceneManagerV2::removeActorMapping(SceneObjectV2* obj)
{
  if (obj && obj->actor())
  {
    m_actorToObject.remove(obj->actor());
  }
}

// 获取对象最大深度
double SceneManagerV2::getMaxObjectDepth(const QList<SceneObjectV2*>& objects)
{
  double maxDepth = 0.0;

  for (auto obj : objects)
  {
    double bounds[6];
    obj->actor()->GetBounds(bounds);
    double depth = bounds[3] - bounds[2];
    if (depth > maxDepth)
      maxDepth = depth;
  }

  return maxDepth;
}

// 计算下一个放置位置
QVector3D SceneManagerV2::calculateNextPosition(const QList<SceneObjectV2*>& placedObjects,
  SceneObjectV2* newObject, double spacing, int maxColumns)
{
  Q_UNUSED(placedObjects)
  Q_UNUSED(newObject)
  Q_UNUSED(spacing)
  Q_UNUSED(maxColumns)
  // 简化实现，返回默认位置
  return QVector3D(0, 0, 0);
}

// 处理对象几何变化
void SceneManagerV2::onObjectGeometryChanged(SceneObjectV2* obj)
{
  // 当对象几何变化时，可以在这里执行一些额外操作
  // 例如：检查对象是否仍在构建平台内，更新约束等
  Q_UNUSED(obj)
}