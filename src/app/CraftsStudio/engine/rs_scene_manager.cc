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

void SceneManager::setObjectSelected(SceneObject* obj, bool selected)
{
  if (obj && obj->_selected != selected)
  {
    obj->_selected = selected;
    // 选中高亮（例如修改VTK属性）
    obj->_property->SetColor(selected ? 1.0 : 0.5, 0, 0); // 选中变红
    emit objectSelectionChanged(obj);
  }
}
