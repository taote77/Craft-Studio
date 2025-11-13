#include "rs_scene_manager.h"

#include <vtkActor.h>

SceneManager* SceneManager::getInstance()
{
  static SceneManager instance;
  return &instance;
}

// 获取所有根对象（组织树的顶层节点）
QList<SceneObject*> SceneManager::rootObjects()
{
  return m_rootObjects;
}

// 查找对象（通过ID快速定位）
SceneObject* SceneManager::findObject(const QString& id)
{
  return m_idToObject.value(id, nullptr);
}

// 添加对象（顶层对象直接加入根列表，子对象通过parent添加）
void SceneManager::addObject(SceneObject* obj, SceneObject* parent)
{
  if (!obj || m_idToObject.contains(obj->_id))
    return;

  m_idToObject[obj->_id] = obj;
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
void SceneManager::removeObject(SceneObject* obj)
{
  if (!obj)
    return;

  // 递归删除所有子对象
  for (auto child : obj->_children)
  {
    removeObject(child);
  }

  // 从父对象或根列表中移除
  if (obj->_parent)
  {
    obj->_parent->removeChild(obj);
  }
  else
  {
    m_rootObjects.removeOne(obj);
    emit rootObjectRemoved(obj);
  }

  // m_idToObject.remove(obj->id);
  emit objectRemoved(obj);
  delete obj; // 释放内存
}

// 更新对象属性（如可见性、选中状态）
void SceneManager::setObjectVisible(SceneObject* obj, bool visible)
{
  if (obj && obj->_visible != visible)
  {
    obj->_visible = visible;
    obj->_actor->SetVisibility(visible); // 同步VTK渲染状态
    emit objectVisibilityChanged(obj);
  }
}

// 通过Actor查找对象
SceneObject* SceneManager::findObjectByActor(vtkActor* actor)
{
  return m_actorToObject.value(actor, nullptr);
}

// 设置变换模式
void SceneManager::setTransformMode(TransformMode mode)
{
  if (m_transformMode != mode)
  {
    m_transformMode = mode;
    emit transformModeChanged(mode);
  }
}

// 清空所有选中状态
void SceneManager::clearSelection()
{
  if (m_selectedObject)
  {
    setObjectSelected(m_selectedObject, false);
    m_selectedObject = nullptr;
    emit selectionChanged(nullptr);
  }
}

void SceneManager::setObjectSelected(SceneObject* obj, bool selected)
{
  if (obj && obj->_selected != selected)
  {
    // 如果选择新对象，先取消之前的选中
    if (selected && m_selectedObject && m_selectedObject != obj)
    {
      setObjectSelected(m_selectedObject, false);
    }
    
    obj->_selected = selected;
    
    // 选中高亮（例如修改VTK属性）
    if (selected)
    {
      obj->_property->SetColor(1.0, 0.8, 0.0); // 选中时黄色高亮
      obj->_property->SetAmbient(0.3); // 增加环境光增强高亮效果
      m_selectedObject = obj;
    }
    else
    {
      obj->_property->SetColor(0.7, 0.7, 0.7); // 默认灰色
      obj->_property->SetAmbient(0.1);
      if (m_selectedObject == obj)
      {
        m_selectedObject = nullptr;
      }
    }
    
    emit objectSelectionChanged(obj);
    emit selectionChanged(m_selectedObject);
  }
}

// 设置对象平移
void SceneManager::setObjectTranslation(SceneObject* obj, double x, double y, double z)
{
  if (!obj)
    return;

  // 应用平移变换到对象的actor
  obj->_actor->SetPosition(x, y, z);
  
  // 发出几何变化信号
  emit objectGeometryChanged(obj);
}

// 设置对象旋转
void SceneManager::setObjectRotation(SceneObject* obj, double x, double y, double z)
{
  if (!obj)
    return;

  // 应用旋转变换到对象的actor
  obj->_actor->SetOrientation(x, y, z);
  
  // 发出几何变化信号
  emit objectGeometryChanged(obj);
}

// 设置对象缩放
void SceneManager::setObjectScale(SceneObject* obj, double x, double y, double z)
{
  if (!obj)
    return;

  // 应用缩放变换到对象的actor
  obj->_actor->SetScale(x, y, z);
  
  // 发出几何变化信号
  emit objectGeometryChanged(obj);
}
