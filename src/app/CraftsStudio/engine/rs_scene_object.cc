#include "rs_scene_object.h"

#include <vtkActor.h>

uint64_t SceneObject::obj_auto_id = 0;

void SceneObject::addChild(SceneObject* child)
{
  if (!_children.contains(child))
  {
    child->_parent = this;
    _children.append(child);
    emit childAdded(child);
  }
}
void SceneObject::removeChild(SceneObject* child)
{
  if (_children.removeOne(child))
  {
    child->_parent = nullptr;
    emit childRemoved(child);
  }
}

void SceneObject::setSelected(bool selected)
{
  _selected = selected;
  if (_selected)
  {
    // 选中时改变颜色（例如红色）
    _property->SetColor(1, 0, 0); // vtkProperty设置颜色
  }
  else
  {
    // 未选中时恢复原始颜色
    // _property->SetColor(_originalColor); // 假设m_originalColor是对象的原始颜色
  }
  _actor->SetProperty(_property); // 应用属性
}