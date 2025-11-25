
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCylinderSource
from vtkmodules.vtkRenderingCore import vtkPolyDataMapper, vtkActor, \
    vtkRenderer, vtkRenderWindow, vtkRenderWindowInteractor
import vtkmodules.vtkRenderingOpenGL2
import vtkmodules.vtkInteractionStyle

# 步骤1 设置数据源sources
colors = vtkNamedColors()

# 设置绘图窗口背景颜色
colors.SetColor("BkgColor", [26, 51, 102, 255])

# 创建有8个侧面的圆柱体
cylinder = vtkCylinderSource()
cylinder.SetResolution(8)

# 步骤2（可选） 设置筛选程序filters

# 步骤3 设置映射程序mappers
cylinderMapper = vtkPolyDataMapper()
cylinderMapper.SetInputConnection(cylinder.GetOutputPort())

# 步骤4 设置道具和演员，props和actors
cylinderActor = vtkActor()
cylinderActor.SetMapper(cylinderMapper)

# 设置圆柱体颜色
cylinderActor.GetProperty().SetColor(colors.GetColor3d("Tomato"))
cylinderActor.RotateX(30.0)
cylinderActor.RotateY(-45.0)

# 步骤5 设置渲染器vtkRenderer
renderer = vtkRenderer()
renderer.AddActor(cylinderActor)
renderer.SetBackground(colors.GetColor3d("BkgColor"))
renderer.ResetCamera()
renderer.GetActiveCamera().Zoom(1.5)

# 步骤6 设置渲染窗口vtkRenderWindow
renderWindow = vtkRenderWindow()
renderWindow.SetSize(300, 300)
renderWindow.AddRenderer(renderer)
renderWindow.SetWindowName("Cylinder")

renderWindow.Render()

# 步骤7 设置渲染窗口交互器vtkRenderWindowInteractor
renderWindowInteractor = vtkRenderWindowInteractor()
renderWindowInteractor.SetRenderWindow(renderWindow)
renderWindowInteractor.Start()
