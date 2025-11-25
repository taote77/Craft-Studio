#include "build_platform.h"

#include <vtkCubeSource.h>
#include <vtkPlaneSource.h>
#include <vtkLineSource.h>
#include <vtkConeSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkAppendPolyData.h>

BuildPlatform::BuildPlatform(QObject* parent)
  : QObject(parent)
{
  // 初始化VTK组件
  m_platformPolyData = vtkSmartPointer<vtkPolyData>::New();
  m_gridPolyData = vtkSmartPointer<vtkPolyData>::New();
  m_axisPolyData = vtkSmartPointer<vtkPolyData>::New();
  
  m_platformActor = vtkSmartPointer<vtkActor>::New();
  m_gridActor = vtkSmartPointer<vtkActor>::New();
  m_axisActor = vtkSmartPointer<vtkActor>::New();
  m_rulerActor = vtkSmartPointer<vtkCaptionActor2D>::New();
  
  // 创建平台几何体
  createPlatformGeometry();
  createGridGeometry();
  createAxisGeometry();
  createRulerGeometry();
  
  // 设置属性
  m_platformActor->GetProperty()->SetColor(0.8, 0.8, 0.8); // 浅灰色
  m_platformActor->GetProperty()->SetOpacity(0.8);       // 半透明
  m_platformActor->GetProperty()->SetAmbient(0.3);
  m_platformActor->SetPickable(false);                    // 不可拾取
  
  m_gridActor->GetProperty()->SetColor(0.5, 0.5, 0.5);    // 深灰色
  m_gridActor->GetProperty()->SetLineWidth(1.0);
  m_gridActor->SetPickable(false);                        // 不可拾取
  
  m_axisActor->GetProperty()->SetAmbient(0.3);
  m_axisActor->SetPickable(false);                         // 不可拾取
  
  // 设置标尺属性 - 暂时注释掉，后续完善
  /*
  vtkSmartPointer<vtkTextProperty> textProp = m_rulerActor->GetTextProperty();
  textProp->SetColor(0, 0, 0);
  textProp->SetFontFamilyToArial();
  textProp->SetFontSize(12);
  textProp->SetJustificationToCentered();
  textProp->SetVerticalJustificationToCentered();
  */
  
  m_rulerActor->SetPickable(false);
}

BuildPlatform::~BuildPlatform()
{
  // VTK智能指针会自动清理资源
}

void BuildPlatform::setWidth(double width)
{
  if (qAbs(m_width - width) > 0.001)
  {
    m_width = width;
    updatePlatform();
    emit sizeChanged(m_width, m_height, m_depth);
    emit platformUpdated();
  }
}

void BuildPlatform::setHeight(double height)
{
  if (qAbs(m_height - height) > 0.001)
  {
    m_height = height;
    updatePlatform();
    emit sizeChanged(m_width, m_height, m_depth);
    emit platformUpdated();
  }
}

void BuildPlatform::setDepth(double depth)
{
  if (qAbs(m_depth - depth) > 0.001)
  {
    m_depth = depth;
    updatePlatform();
    emit sizeChanged(m_width, m_height, m_depth);
    emit platformUpdated();
  }
}

void BuildPlatform::setOriginX(double x)
{
  if (qAbs(m_originX - x) > 0.001)
  {
    m_originX = x;
    update();
    emit originChanged(m_originX, m_originY);
    emit platformUpdated();
  }
}

void BuildPlatform::setOriginY(double y)
{
  if (qAbs(m_originY - y) > 0.001)
  {
    m_originY = y;
    update();
    emit originChanged(m_originX, m_originY);
    emit platformUpdated();
  }
}

void BuildPlatform::setGridSpacing(double spacing)
{
  if (qAbs(m_gridSpacing - spacing) > 0.001)
  {
    m_gridSpacing = spacing;
    updateGrid();
    emit gridSpacingChanged(m_gridSpacing);
    emit platformUpdated();
  }
}

void BuildPlatform::setPlatformVisible(bool visible)
{
  if (m_platformVisible != visible)
  {
    m_platformVisible = visible;
    m_platformActor->SetVisibility(visible);
    emit platformVisibilityChanged(visible);
  }
}

void BuildPlatform::setGridVisible(bool visible)
{
  if (m_gridVisible != visible)
  {
    m_gridVisible = visible;
    m_gridActor->SetVisibility(visible);
    emit gridVisibilityChanged(visible);
  }
}

void BuildPlatform::setAxisVisible(bool visible)
{
  if (m_axisVisible != visible)
  {
    m_axisVisible = visible;
    m_axisActor->SetVisibility(visible);
    emit axisVisibilityChanged(visible);
  }
}

void BuildPlatform::setRulerVisible(bool visible)
{
  if (m_rulerVisible != visible)
  {
    m_rulerVisible = visible;
    m_rulerActor->SetVisibility(visible);
    emit rulerVisibilityChanged(visible);
  }
}

void BuildPlatform::addToRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  if (!renderer)
    return;
    
  m_renderer = renderer;
  m_weakRenderer = renderer;
  
  renderer->AddActor(m_platformActor);
  renderer->AddActor(m_gridActor);
  renderer->AddActor(m_axisActor);
  renderer->AddActor(m_rulerActor);
  
  updateVisibility();
}

void BuildPlatform::removeFromRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
  if (!renderer)
    return;
    
  renderer->RemoveActor(m_platformActor);
  renderer->RemoveActor(m_gridActor);
  renderer->RemoveActor(m_axisActor);
  renderer->RemoveActor(m_rulerActor);
  
  m_renderer = nullptr;
  m_weakRenderer = nullptr;
}

void BuildPlatform::update()
{
  updatePlatform();
  updateGrid();
  updateAxis();
  updateRuler();
}

void BuildPlatform::updatePlatform()
{
  createPlatformGeometry();
}

void BuildPlatform::updateGrid()
{
  createGridGeometry();
}

void BuildPlatform::updateAxis()
{
  createAxisGeometry();
}

void BuildPlatform::updateRuler()
{
  createRulerGeometry();
}

void BuildPlatform::getBounds(double bounds[6]) const
{
  bounds[0] = m_originX - m_width / 2.0;  // Xmin
  bounds[1] = m_originX + m_width / 2.0;  // Xmax
  bounds[2] = m_originY - m_height / 2.0; // Ymin
  bounds[3] = m_originY + m_height / 2.0; // Ymax
  bounds[4] = m_depth;                     // Zmin (如果depth为负，则表示向下)
  bounds[5] = 0.0;                         // Zmax (平台表面)
}

bool BuildPlatform::isPointInside(double x, double y) const
{
  return (x >= m_originX - m_width / 2.0 && 
          x <= m_originX + m_width / 2.0 && 
          y >= m_originY - m_height / 2.0 && 
          y <= m_originY + m_height / 2.0);
}

void BuildPlatform::getPrintArea(double bounds[6]) const
{
  // 打印区域比平台略小一些
  double margin = 2.0; // 2mm边距
  bounds[0] = m_originX - m_width / 2.0 + margin;  // Xmin
  bounds[1] = m_originX + m_width / 2.0 - margin;  // Xmax
  bounds[2] = m_originY - m_height / 2.0 + margin; // Ymin
  bounds[3] = m_originY + m_height / 2.0 - margin; // Ymax
  bounds[4] = 0.0;                                 // Zmin
  bounds[5] = 200.0;                               // Zmax (默认最大高度200mm)
}

void BuildPlatform::onPlatformPropertyChanged()
{
  update();
}

void BuildPlatform::createPlatformGeometry()
{
  // 创建立方体作为平台
  vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
  cube->SetXLength(m_width);
  cube->SetYLength(m_height);
  cube->SetZLength(m_depth);
  
  // 创建变换，将平台居中在原点
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Translate(m_originX, m_originY, -m_depth / 2.0);
  
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = 
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputConnection(cube->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();
  
  // 设置几何数据和mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(transformFilter->GetOutputPort());
  
  m_platformActor->SetMapper(mapper);
}

void BuildPlatform::createGridGeometry()
{
  // 创建网格线
  vtkSmartPointer<vtkPolyData> gridPolyData = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  
  // 计算网格线数量
  int numLinesX = static_cast<int>(m_width / m_gridSpacing) + 1;
  int numLinesY = static_cast<int>(m_height / m_gridSpacing) + 1;
  
  double xMin = m_originX - m_width / 2.0;
  double xMax = m_originX + m_width / 2.0;
  double yMin = m_originY - m_height / 2.0;
  double yMax = m_originY + m_height / 2.0;
  
  // 添加X方向的网格线
  for (int i = 0; i < numLinesY; ++i)
  {
    double y = yMin + i * m_gridSpacing;
    if (y > yMax) y = yMax;
    
    vtkIdType pointIds[2];
    pointIds[0] = points->InsertNextPoint(xMin, y, 0.0);
    pointIds[1] = points->InsertNextPoint(xMax, y, 0.0);
    lines->InsertNextCell(2, pointIds);
  }
  
  // 添加Y方向的网格线
  for (int i = 0; i < numLinesX; ++i)
  {
    double x = xMin + i * m_gridSpacing;
    if (x > xMax) x = xMax;
    
    vtkIdType pointIds[2];
    pointIds[0] = points->InsertNextPoint(x, yMin, 0.0);
    pointIds[1] = points->InsertNextPoint(x, yMax, 0.0);
    lines->InsertNextCell(2, pointIds);
  }
  
  gridPolyData->SetPoints(points);
  gridPolyData->SetLines(lines);
  
  // 设置几何数据和mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(gridPolyData);
  
  m_gridActor->SetMapper(mapper);
}

void BuildPlatform::createAxisGeometry()
{
  // 创建三个坐标轴(X-红色, Y-绿色, Z-蓝色)
  vtkSmartPointer<vtkPolyData> axisPolyData = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  
  // 坐标轴长度
  double axisLength = qMax(m_width, m_height) * 0.3;
  
  // 添加坐标轴点
  vtkIdType origin = points->InsertNextPoint(0, 0, 0);
  vtkIdType xAxis = points->InsertNextPoint(axisLength, 0, 0);  // X轴
  vtkIdType yAxis = points->InsertNextPoint(0, axisLength, 0);  // Y轴
  vtkIdType zAxis = points->InsertNextPoint(0, 0, axisLength); // Z轴
  
  // 添加坐标轴线 - 暂时注释掉，后续完善
  /*
  lines->InsertNextCell(2, &origin, 1);
  lines->InsertNextCell(2, &origin, 2);
  lines->InsertNextCell(2, &origin, 3);
  */
  
  axisPolyData->SetPoints(points);
  axisPolyData->SetLines(lines);
  
  // 创建坐标轴箭头
  vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetRadius(axisLength * 0.03);
  cone->SetHeight(axisLength * 0.1);
  cone->SetResolution(10);
  
  // 为每个轴创建箭头
  vtkSmartPointer<vtkTransform> xTransform = vtkSmartPointer<vtkTransform>::New();
  xTransform->Translate(axisLength, 0, 0);
  xTransform->RotateZ(-90);
  
  vtkSmartPointer<vtkTransformPolyDataFilter> xConeFilter = 
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  xConeFilter->SetInputConnection(cone->GetOutputPort());
  xConeFilter->SetTransform(xTransform);
  
  vtkSmartPointer<vtkTransform> yTransform = vtkSmartPointer<vtkTransform>::New();
  yTransform->Translate(0, axisLength, 0);
  
  vtkSmartPointer<vtkTransformPolyDataFilter> yConeFilter = 
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  yConeFilter->SetInputConnection(cone->GetOutputPort());
  yConeFilter->SetTransform(yTransform);
  
  vtkSmartPointer<vtkTransform> zTransform = vtkSmartPointer<vtkTransform>::New();
  zTransform->Translate(0, 0, axisLength);
  zTransform->RotateX(90);
  
  vtkSmartPointer<vtkTransformPolyDataFilter> zConeFilter = 
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  zConeFilter->SetInputConnection(cone->GetOutputPort());
  zConeFilter->SetTransform(zTransform);
  
  // 合并所有几何体
  vtkSmartPointer<vtkAppendPolyData> appendFilter = 
    vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputData(axisPolyData);
  appendFilter->AddInputConnection(xConeFilter->GetOutputPort());
  appendFilter->AddInputConnection(yConeFilter->GetOutputPort());
  appendFilter->AddInputConnection(zConeFilter->GetOutputPort());
  appendFilter->Update();
  
  // 设置几何数据和mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(appendFilter->GetOutputPort());
  
  m_axisActor->SetMapper(mapper);
}

void BuildPlatform::createRulerGeometry()
{
  // 设置标尺文本
  QString rulerText = QString("%1 x %2 x %3 mm")
                    .arg(m_width, 0, 'f', 1)
                    .arg(m_height, 0, 'f', 1)
                    .arg(m_depth > 0 ? m_depth : 0.0, 0, 'f', 1);
  
  // TODO: 实现标尺文本显示 - 需要替换为适当的VTK文本组件
  // m_rulerActor->SetCaption(rulerText.toStdString().c_str());
  // m_rulerActor->SetAttachmentPoint(m_originX, m_originY - m_height / 2.0 - 10, 0);
}

void BuildPlatform::updateVisibility()
{
  m_platformActor->SetVisibility(m_platformVisible);
  m_gridActor->SetVisibility(m_gridVisible);
  m_axisActor->SetVisibility(m_axisVisible);
  m_rulerActor->SetVisibility(m_rulerVisible);
}