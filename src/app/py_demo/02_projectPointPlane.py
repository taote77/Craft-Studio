# 原始C++代码来自
# https://examples.vtk.org/site/Cxx/SimpleOperations/ProjectPointPlane/
# 本例演示了如何使用vtkPlane类对一个三维点进行投影
from vtkmodules.vtkCommonDataModel import vtkPlane

# 设置平面的原点和法向量
plane = vtkPlane()
plane.SetOrigin(0.0, 0.0, 0.0)
plane.SetNormal(0.0, 0.0, 1.0)

# 一个三维点 
p = (23.1, 54.6, 9.2)

# 使用vtkPlane对象的ProjectPoint方法，把点p投影到平面，然后把结果保存到projected里面
origin = (0.0, 0.0, 0.0)
normal = (0.0, 0.0, 1.0)
projected = [0.0] * 3
plane.ProjectPoint(p, origin, normal, projected)

print(f'投影到平面上的点 : {projected}')