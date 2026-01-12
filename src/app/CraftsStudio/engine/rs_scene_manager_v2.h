#ifndef RS_SCENE_MANAGER_V2_H
#define RS_SCENE_MANAGER_V2_H

#include "rs_interactor_v2.h"
#include "rs_scene_object_v2.h"
#include <QHash>
#include <QRectF>
#include <QVector3D>
#include <vtkBoundingBox.h>
#include <vtkRenderer.h>

// 变换模式枚举
enum class TransformModeV2
{
  None,      // 无变换模式
  Translate, // 平移模式
  Rotate,    // 旋转模式
  Scale      // 缩放模式
};

// 构建平台参数
struct BuildPlatformParameters
{
  double width = 200.0;        // 平台宽度(mm)
  double height = 200.0;       // 平台高度(mm)
  double depth = 0.0;          // 平台深度(mm)
  double originX = 0.0;        // 原点X坐标
  double originY = 0.0;        // 原点Y坐标
  double gridSpacing = 10.0;   // 网格间距(mm)
  bool gridVisible = true;     // 网格是否可见
  bool platformVisible = true; // 平台是否可见
};

class SceneManagerV2 : public QObject
{
  Q_OBJECT
public:
  // 单例模式（全局唯一管理器）
  static SceneManagerV2* getInstance();

  // 获取所有根对象（组织树的顶层节点）
  QList<SceneObjectV2*> rootObjects();

  // 获取所有选中的对象（支持多选）
  QList<SceneObjectV2*> selectedObjects() const { return m_selectedObjects; }

  // 查找对象（通过ID快速定位）
  SceneObjectV2* findObject(const QString& id);

  // 通过Actor查找对象
  SceneObjectV2* findObjectByActor(vtkActor* actor);

  // 获取当前选中的对象（保持向后兼容）
  SceneObjectV2* getSelectedObject() const
  {
    return m_selectedObjects.isEmpty() ? nullptr : m_selectedObjects.first();
  }

  // 获取渲染器
  vtkSmartPointer<vtkRenderer> renderer() const { return m_renderer; }
  void setRenderer(vtkSmartPointer<vtkRenderer> renderer);

  // 获取当前变换模式
  TransformModeV2 getTransformMode() const { return m_transformMode; }

  // 设置变换模式
  void setTransformMode(TransformModeV2 mode);

  // 清空所有选中状态
  void clearSelection();

  // 构建平台管理
  BuildPlatformParameters& buildPlatform() { return m_buildPlatform; }
  void setBuildPlatform(const BuildPlatformParameters& platform);
  void updateBuildPlatformVisibility();

  // 相机视图控制
  void setViewType(ViewType type);
  ViewType getViewType() const { return m_viewType; }
  void resetCamera();
  void fitAllObjects();
  void fitSelectedObjects();

  // 多选支持
  void setMultiSelectionEnabled(bool enabled) { m_multiSelectionEnabled = enabled; }
  bool isMultiSelectionEnabled() const { return m_multiSelectionEnabled; }

  // 对象选择（支持多选）
  void setObjectSelected(SceneObjectV2* obj, bool selected, bool addToSelection = false);

  // 选择管理
  void selectAll();
  void invertSelection();
  void selectObjectsInRect(const QRectF& rect);

  // 获取场景边界
  void getSceneBounds(double bounds[6]);
  void getSelectedObjectsBounds(double bounds[6]);

  // 辅助函数
  double getMaxObjectDepth(const QList<SceneObjectV2*>& objects);

public slots:
  // 添加对象（顶层对象直接加入根列表，子对象通过parent添加）
  void addObject(SceneObjectV2* obj, SceneObjectV2* parent = nullptr);

  // 删除对象（递归删除子对象）
  void removeObject(SceneObjectV2* obj);

  // 批量删除对象
  void removeObjects(const QList<SceneObjectV2*>& objects);

  // 更新对象属性（如可见性、选中状态）
  void setObjectVisible(SceneObjectV2* obj, bool visible);
  void setObjectsVisible(const QList<SceneObjectV2*>& objects, bool visible);

  // 对象变换操作
  void setObjectTranslation(SceneObjectV2* obj, double x, double y, double z);
  void setObjectRotation(SceneObjectV2* obj, double x, double y, double z);
  void setObjectScale(SceneObjectV2* obj, double x, double y, double z);

  // 批量变换操作
  void setObjectsTranslation(const QList<SceneObjectV2*>& objects, double x, double y, double z);
  void setObjectsRotation(const QList<SceneObjectV2*>& objects, double x, double y, double z);
  void setObjectsScale(const QList<SceneObjectV2*>& objects, double x, double y, double z);

  // 优化布局功能
  void autoLayoutObjects(double spacing = 5.0);
  void arrangeInGrid(int columns = 3);

signals:
  // 通知组织树更新（节点增删、名称/层级变化）
  void objectAdded(SceneObjectV2* obj);
  void objectRemoved(SceneObjectV2* obj);
  void rootObjectAdded(SceneObjectV2* obj);
  void rootObjectRemoved(SceneObjectV2* obj);

  // 通知渲染窗口更新（可见性、选中状态、几何数据变化）
  void objectVisibilityChanged(SceneObjectV2* obj);
  void objectSelectionChanged(SceneObjectV2* obj);
  void objectGeometryChanged(SceneObjectV2* obj); // 模型修改时触发

  // 多选相关信号
  void multiSelectionChanged(const QList<SceneObjectV2*>& selectedObjects);

  // 构建平台信号
  void buildPlatformChanged();

  // 视图变化信号
  void viewTypeChanged(ViewType newType);
  void cameraReset();
  void cameraFitAll();
  void cameraFitSelected();

  // 变换状态变化信号
  void transformModeChanged(TransformModeV2 newMode);
  void selectionChanged(SceneObjectV2* selectedObject); // 保持向后兼容

private slots:
  void onObjectGeometryChanged(SceneObjectV2* obj);

private:
  SceneManagerV2();
  ~SceneManagerV2() = default;

  // 禁用拷贝构造和赋值
  SceneManagerV2(const SceneManagerV2&) = delete;
  SceneManagerV2& operator=(const SceneManagerV2&) = delete;

  // 更新Actor映射
  void updateActorMapping(SceneObjectV2* obj);
  void removeActorMapping(SceneObjectV2* obj);

  // 计算优化布局
  QVector3D calculateNextPosition(const QList<SceneObjectV2*>& placedObjects, SceneObjectV2* newObject,
    double spacing, int maxColumns);

  // 内部状态
  QList<SceneObjectV2*> m_rootObjects;
  QHash<QString, SceneObjectV2*> m_idToObject;      // id到对象的映射
  QHash<vtkActor*, SceneObjectV2*> m_actorToObject; // Actor到对象的映射（用于快速查找）
  QList<SceneObjectV2*> m_selectedObjects;          // 当前选中的对象列表（支持多选）
  TransformModeV2 m_transformMode = TransformModeV2::None;
  ViewType m_viewType = ViewType::Perspective;
  BuildPlatformParameters m_buildPlatform; // 构建平台参数
  bool m_multiSelectionEnabled = true;     // 是否启用多选

  vtkSmartPointer<vtkRenderer> m_renderer; // VTK渲染器引用
};

#endif // RS_SCENE_MANAGER_V2_H