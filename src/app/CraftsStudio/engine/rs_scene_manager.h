#ifndef RS_SCENE_MANAGER_H
#define RS_SCENE_MANAGER_H

#include "rs_scene_object.h"
#include <QHash>

class SceneManager : public QObject
{
  Q_OBJECT
public:
  // 单例模式（全局唯一管理器）
  static SceneManager* getInstance();

  // 获取所有根对象（组织树的顶层节点）
  QList<SceneObject*> rootObjects();

  // 查找对象（通过ID快速定位）
  SceneObject* findObject(const QString& id);

public slots:
  // 添加对象（顶层对象直接加入根列表，子对象通过parent添加）
  void addObject(SceneObject* obj, SceneObject* parent = nullptr);

  // 删除对象（递归删除子对象）
  void removeObject(SceneObject* obj);

  // 更新对象属性（如可见性、选中状态）
  void setObjectVisible(SceneObject* obj, bool visible);

  void setObjectSelected(SceneObject* obj, bool selected);

signals:
  // 通知组织树更新（节点增删、名称/层级变化）
  void objectAdded(SceneObject* obj);
  void objectRemoved(SceneObject* obj);
  void rootObjectAdded(SceneObject* obj);
  void rootObjectRemoved(SceneObject* obj);

  // 通知渲染窗口更新（可见性、选中状态、几何数据变化）
  void objectVisibilityChanged(SceneObject* obj);
  void objectSelectionChanged(SceneObject* obj);
  void objectGeometryChanged(SceneObject* obj); // 模型修改时触发

private:
  QList<SceneObject*> m_rootObjects;

  QHash<QString, SceneObject*> m_idToObject;      // id到对象的映射
  QHash<vtkActor*, SceneObject*> m_actorToObject; // Actor到对象的映射（用于快速查找）
  SceneObject* m_selectedObject = nullptr;
};

#endif // RS_SCENE_MANAGER_H