#ifndef RS_SCENE_MANAGER_H
#define RS_SCENE_MANAGER_H

#include "rs_scene_object.h"
#include <QHash>

// 变换模式枚举
enum class TransformMode
{
  None,      // 无变换模式
  Translate, // 平移模式
  Rotate,    // 旋转模式
  Scale      // 缩放模式
};

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

  // 通过Actor查找对象
  SceneObject* findObjectByActor(vtkActor* actor);

  // 获取当前选中的对象
  SceneObject* getSelectedObject() const { return m_selectedObject; }

  // 获取当前变换模式
  TransformMode getTransformMode() const { return m_transformMode; }

  // 设置变换模式
  void setTransformMode(TransformMode mode);

  // 清空所有选中状态
  void clearSelection();

public slots:
  // 添加对象（顶层对象直接加入根列表，子对象通过parent添加）
  void addObject(SceneObject* obj, SceneObject* parent = nullptr);

  // 删除对象（递归删除子对象）
  void removeObject(SceneObject* obj);

  // 更新对象属性（如可见性、选中状态）
  void setObjectVisible(SceneObject* obj, bool visible);

  void setObjectSelected(SceneObject* obj, bool selected);

  // 对象变换操作
  void setObjectTranslation(SceneObject* obj, double x, double y, double z);
  void setObjectRotation(SceneObject* obj, double x, double y, double z);
  void setObjectScale(SceneObject* obj, double x, double y, double z);

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

  // 变换状态变化信号
  void transformModeChanged(TransformMode newMode);
  void selectionChanged(SceneObject* selectedObject);

private:
  QList<SceneObject*> m_rootObjects;

  QHash<QString, SceneObject*> m_idToObject;      // id到对象的映射
  QHash<vtkActor*, SceneObject*> m_actorToObject; // Actor到对象的映射（用于快速查找）
  SceneObject* m_selectedObject = nullptr;
  TransformMode m_transformMode = TransformMode::None;
};

#endif // RS_SCENE_MANAGER_H