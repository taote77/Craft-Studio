
import math
from vtkmodules.vtkCommonCore import reference
from vtkmodules.vtkCommonDataModel import vtkLine

# 一条线段的两个端点
lineP0 = (0.0, 0.0, 0.0)
lineP1 = (2.0, 0.0, 0.0)

# 两个三维点
p0 = (1.0, 0, 0)
p1 = (1.0, 2.0, 0)

print('p0 = ', p0)
print('p1 = ', p1)

# 使用vtkLine类的DistanceToLine方法（重载1）计算p1到线段的距离平方
# DistanceToLine有3个参数，参数1是需要计算的三维点坐标，参数2是线段起点坐标，
# 参数3是线段终点坐标
dist0 = math.sqrt(vtkLine.DistanceToLine(p0, lineP0, lineP1))
print(f'p0到线段的距离 : {dist0:.2f}')

dist1 = math.sqrt(vtkLine.DistanceToLine(p1, lineP0, lineP1))
print(f'p1到线段的距离 : {dist1:.2f}')

# 使用vtkLine类的DistanceToLine方法（重载2）计算p1到线段的距离平方
# DistanceToLine有5个参数，参数1是需要计算的三维点坐标，参数2是线段起点坐标，
# 参数3是线段终点坐标，参数4是输出参数，表示投影点在线段上的参数化位置，取值范围[0, 1],
# 参数5是输出参数，表示线段上离参数1最近的垂足点坐标
# 根据https://docs.vtk.org/en/latest/advanced/PythonWrappers.html的Pass by Reference说明
# 参数4必须使用reference类型
t = reference(0.0)
closest = [0.0] * 3
dist0 = math.sqrt(vtkLine.DistanceToLine(p0, lineP0, lineP1, t, closest))
print(f'p0到线段的距离 : {dist0:.2f}, 最近的点是 : {closest}, t: {t}')

dist1 = math.sqrt(vtkLine.DistanceToLine(p1, lineP0, lineP1, t, closest))
print(f'p1到线段的距离 : {dist1:.2f}, 最近的点是 : {closest}, t: {t}')