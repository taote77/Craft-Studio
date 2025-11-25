# 本例求两个三维点之间的距离平方和欧式距离
import math
from vtkmodules.vtkCommonCore import vtkMath

# 定义两个三维空间的点
p0 = (0, 0, 0)
p1 = (1, 1, 1)

# 首先使用vtkMath类的Distance2BetweenPoints方法计算两点之间的距离平方
distSquared = vtkMath.Distance2BetweenPoints(p0, p1)

# 然后开根得到欧式距离
dist = math.sqrt(distSquared)

# 输出结果
print('p0 = ', p0)
print('p1 = ', p1)
print('距离平方 = {:.2f}'.format(distSquared))
print('距离 = {:.2f}'.format(dist))


p2 = (0, 0, 1)
p3 = (0, 0, 10)

dist_squared = vtkMath.Distance2BetweenPoints(p2, p3)
dist = math.sqrt(dist_squared)
print('p2 = ', p2)
print('p3 = ', p3)
print('距离平方 = {:.2f}'.format(dist_squared))
print('距离 = {:.2f}'.format(dist))
