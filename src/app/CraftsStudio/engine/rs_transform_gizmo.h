#ifndef RS_TRANSFORM_GIZMO_H
#define RS_TRANSFORM_GIZMO_H

#include <QObject>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// 操纵器手柄类型
enum class GizmoHandleType
{
  None,        // 无手柄
  TranslateX,  // X轴平移手柄
  TranslateY,  // Y轴平移手柄
  TranslateZ,  // Z轴平移手柄
  RotateX,     // X轴旋转环
  RotateY,     // Y轴旋转环
  RotateZ,     // Z轴旋转环
  ScaleX,      // X轴缩放手柄
  ScaleY,      // Y轴缩放手柄
  ScaleZ,      // Z轴缩放手柄
  UniformScale // 均匀缩放手柄
};

class TransformGizmo : public QObject
{
  Q_OBJECT

public:
  explicit TransformGizmo(QObject* parent = nullptr);
  ~TransformGizmo();

  // 设置渲染器
  void setRenderer(vtkRenderer* renderer);
  
  // 设置目标对象
  void setTarget(vtkActor* target);

  // 设置变换模式
  void setTransformMode(int mode);

  // 显示/隐藏操纵器
  void show();
  void hide();

  // 更新操纵器位置和方向
  void updatePosition();

  // 获取所有操纵器Actor
  QList<vtkActor*> getGizmoActors() const;

  // 检测鼠标是否在操纵器手柄上
  GizmoHandleType pickHandle(const double pickPoint[3]);
  bool PickHandle(int x, int y, int& handleType, int& axisIndex);
  
  // 精确的射线拾取检测
  bool rayPickHandle(int x, int y, GizmoHandleType& handleType, int& axisIndex);
  
  // 视觉反馈功能
  void highlightHandle(GizmoHandleType handleType, int axisIndex);
  void clearHighlight();
  void startTransformPreview();
  void updateTransformPreview(double dx, double dy, int axisIndex, GizmoHandleType handleType);
  void endTransformPreview();

  // 获取/设置手柄高亮状态
  bool isHandleHighlighted() const { return m_isHandleHighlighted; }
  GizmoHandleType getHighlightedHandle() const { return m_highlightedHandle; }
  int getHighlightedAxis() const { return m_highlightedAxis; }

  // 执行变换操作
  void translate(double delta[3]);
  void rotate(double angle, int axis);
  void scale(double factor, int axis);
  void ApplyTranslation(double dx, double dy, int axisIndex);
  void ApplyRotation(double dx, double dy, int axisIndex);
  void ApplyScale(double dx, double dy, int axisIndex);

  // 操纵器设置
  void SetTargetObject(vtkActor* target);
  void SetMode(int mode);
  void Show();
  void Hide();

signals:
  // 变换操作完成信号
  void transformApplied();

private:
  // 创建操纵器组件
  void createTranslateGizmo();
  void createRotateGizmo();
  void createScaleGizmo();

  // 创建手柄Actor
  vtkActor* createArrowHandle(int axis, double color[3]);
  vtkActor* createRingHandle(int axis, double color[3]);
  vtkActor* createCubeHandle(int axis, double color[3]);
  vtkActor* createUniformScaleHandle();

  // 设置手柄属性
  void setupHandleProperties(vtkActor* actor, double color[3]);

private:
  vtkActor* m_target = nullptr;              // 目标对象
  vtkSmartPointer<vtkTransform> m_transform; // 变换矩阵

  // 操纵器组件
  QList<vtkActor*> m_translateHandles;      // 平移手柄
  QList<vtkActor*> m_rotateHandles;         // 旋转环
  QList<vtkActor*> m_scaleHandles;          // 缩放手柄
  vtkActor* m_uniformScaleHandle = nullptr; // 均匀缩放手柄

  // 拾取相关组件
  vtkSmartPointer<vtkPropPicker> m_picker;  // 拾取器
  vtkSmartPointer<vtkRenderer> m_renderer;  // 渲染器

  int m_currentMode = 0;  // 当前变换模式
  bool m_visible = false; // 是否可见

  // 拾取参数
  double m_pickTolerance = 0.05;           // 拾取容差

  // 视觉反馈状态
  bool m_isHandleHighlighted = false;     // 是否正在高亮手柄
  GizmoHandleType m_highlightedHandle = GizmoHandleType::None; // 当前高亮的手柄
  int m_highlightedAxis = -1;              // 当前高亮的轴向

  // 变换预览状态
  bool m_isTransforming = false;           // 是否正在进行变换
  double m_previewTransform[3] = {0.0, 0.0, 0.0}; // 预览变换数据
  vtkSmartPointer<vtkActor> m_previewActor = nullptr; // 预览Actor
  vtkSmartPointer<vtkTransform> m_originalTransform = nullptr; // 原始变换

  // 手柄颜色定义
  static constexpr double RED_COLOR[3] = { 1.0, 0.0, 0.0 };    // X轴
  static constexpr double GREEN_COLOR[3] = { 0.0, 1.0, 0.0 };  // Y轴
  static constexpr double BLUE_COLOR[3] = { 0.0, 0.0, 1.0 };   // Z轴
  static constexpr double YELLOW_COLOR[3] = { 1.0, 1.0, 0.0 }; // 均匀缩放
};

#endif // RS_TRANSFORM_GIZMO_H