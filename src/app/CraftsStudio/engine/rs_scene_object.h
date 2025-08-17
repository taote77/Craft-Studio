#ifndef RS_SCENE_OBJECT_H
#define RS_SCENE_OBJECT_H

#include <QObject>

#include <cstdint>
#include <qchar.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

class SceneObject : public QObject
{
  /**
   * @brief 唯一标识符，用于快速查找对象。
   * @details 该成员变量存储对象的唯一标识，通常用于在集合或数据库中快速定位和检索对象。
   */
  Q_OBJECT
public:
  // 唯一标识（用于快速查找）
  QString _id;
  // 显示名称（组织树节点文本）
  QString _name;
  // 父对象（维护层级关系，组织树的父子节点对应）
  SceneObject* _parent;
  // 子对象列表（树形结构的核心）
  QList<SceneObject*> _children;

  // VTK渲染相关组件（供渲染窗口使用）
  vtkSmartPointer<vtkActor> _actor;       // 渲染实体
  vtkSmartPointer<vtkPolyData> _polyData; // 几何数据
  vtkSmartPointer<vtkProperty> _property; // 外观属性（颜色、透明度等）

  // 状态属性（组织树和渲染窗口共享）
  bool _visible;  // 是否可见（树节点勾选状态 <-> 渲染可见性）
  bool _selected; // 是否选中（树节点高亮 <-> 渲染高亮）

public:
  explicit SceneObject(QString name, vtkSmartPointer<vtkActor> actor, QObject* parent = nullptr)
    : _name(name)
    , _actor(actor)
    , _parent(nullptr)
    , _visible(true)
    , _selected(false)
  {
    _id = genObjId();
  }

  QString genObjId()
  {
    SceneObject::obj_auto_id++;
    //
    return QString("obj_%1").arg(obj_auto_id);
  }
  // 层级操作：添加/移除子对象
  void addChild(SceneObject* child);

  void removeChild(SceneObject* child);

  void setSelected(bool selected);

signals:
  // 属性变化时发射信号（通知视图更新）
  void nameChanged(QString newName);

  void visibleChanged(bool visible);

  void selectedChanged(bool selected);

  void childAdded(SceneObject* child);

  void childRemoved(SceneObject* child);

private:
  static uint64_t obj_auto_id;
};

#endif // RS_SCENE_OBJECT_H