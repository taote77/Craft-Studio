#ifndef BUILD_PLATFORM_H
#define BUILD_PLATFORM_H

#include <QObject>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>

// 构建平台类，负责显示和管理3D打印构建平台
class BuildPlatform : public QObject
{
  Q_OBJECT

public:
  explicit BuildPlatform(QObject* parent = nullptr);
  ~BuildPlatform();

  // 获取构建平台参数
  double width() const { return m_width; }
  void setWidth(double width);
  
  double height() const { return m_height; }
  void setHeight(double height);
  
  double depth() const { return m_depth; }
  void setDepth(double depth);
  
  double originX() const { return m_originX; }
  void setOriginX(double x);
  
  double originY() const { return m_originY; }
  void setOriginY(double y);
  
  double gridSpacing() const { return m_gridSpacing; }
  void setGridSpacing(double spacing);
  
  // 可见性控制
  bool isPlatformVisible() const { return m_platformVisible; }
  void setPlatformVisible(bool visible);
  
  bool isGridVisible() const { return m_gridVisible; }
  void setGridVisible(bool visible);
  
  bool isAxisVisible() const { return m_axisVisible; }
  void setAxisVisible(bool visible);
  
  bool isRulerVisible() const { return m_rulerVisible; }
  void setRulerVisible(bool visible);

  // VTK组件
  vtkSmartPointer<vtkActor> platformActor() const { return m_platformActor; }
  vtkSmartPointer<vtkActor> gridActor() const { return m_gridActor; }
  vtkSmartPointer<vtkActor> axisActor() const { return m_axisActor; }
  vtkSmartPointer<vtkActor2D> rulerActor() const { return m_rulerActor; }

  // 添加到渲染器
  void addToRenderer(vtkSmartPointer<vtkRenderer> renderer);
  
  // 从渲染器移除
  void removeFromRenderer(vtkSmartPointer<vtkRenderer> renderer);
  
  // 更新显示
  void update();
  void updatePlatform();
  void updateGrid();
  void updateAxis();
  void updateRuler();
  
  // 获取边界
  void getBounds(double bounds[6]) const;
  
  // 检查点是否在平台内
  bool isPointInside(double x, double y) const;
  
  // 获取打印区域
  void getPrintArea(double bounds[6]) const;

public slots:
  void onPlatformPropertyChanged();

signals:
  // 参数变化信号
  void sizeChanged(double width, double height, double depth);
  void originChanged(double x, double y);
  void gridSpacingChanged(double spacing);
  
  // 可见性变化信号
  void platformVisibilityChanged(bool visible);
  void gridVisibilityChanged(bool visible);
  void axisVisibilityChanged(bool visible);
  void rulerVisibilityChanged(bool visible);
  
  // 更新信号
  void platformUpdated();

private:
  // 创建平台几何体
  void createPlatformGeometry();
  
  // 创建网格几何体
  void createGridGeometry();
  
  // 创建坐标轴
  void createAxisGeometry();
  
  // 创建标尺
  void createRulerGeometry();
  
  // 更新所有可见性
  void updateVisibility();

private:
  // 平台参数
  double m_width = 200.0;      // 平台宽度(mm)
  double m_height = 200.0;     // 平台高度(mm)
  double m_depth = 0.0;        // 平台深度(mm)
  double m_originX = 0.0;      // 原点X坐标
  double m_originY = 0.0;      // 原点Y坐标
  double m_gridSpacing = 10.0; // 网格间距(mm)
  
  // 可见性状态
  bool m_platformVisible = true; // 平台是否可见
  bool m_gridVisible = true;     // 网格是否可见
  bool m_axisVisible = true;     // 坐标轴是否可见
  bool m_rulerVisible = true;    // 标尺是否可见
  
  // VTK组件
  vtkSmartPointer<vtkActor> m_platformActor; // 平台Actor
  vtkSmartPointer<vtkActor> m_gridActor;     // 网格Actor
  vtkSmartPointer<vtkActor> m_axisActor;      // 坐标轴Actor
  vtkSmartPointer<vtkActor2D> m_rulerActor;   // 标尺Actor
  
  // VTK数据
  vtkSmartPointer<vtkPolyData> m_platformPolyData; // 平台几何数据
  vtkSmartPointer<vtkPolyData> m_gridPolyData;     // 网格几何数据
  vtkSmartPointer<vtkPolyData> m_axisPolyData;     // 坐标轴几何数据
  
  // 渲染器引用
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkWeakPointer<vtkRenderer> m_weakRenderer;
};

#endif // BUILD_PLATFORM_H