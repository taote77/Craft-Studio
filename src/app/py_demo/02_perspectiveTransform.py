# 本例给定一个三维点和一个矩阵，分别使用vtkPerspectiveTransform和vtkTransform进行变换
# 然后输出二者的结果
from vtkmodules.vtkCommonMath import vtkMatrix4x4
from vtkmodules.vtkCommonTransforms import vtkPerspectiveTransform, vtkTransform

# 一个三维点
p = (1.0, 3.0, 3.0)

# 一个矩阵
m = vtkMatrix4x4()

# 更简洁的初始化：用二维列表 + 双层循环赋值
_values = [
    [1, 2, 3, 4],
    [2, 2, 3, 4],
    [3, 2, 3, 4],
    [4, 2, 3, 4],
]
for i in range(4):
    for j in range(4):
        m.SetElement(i, j, float(_values[i][j]))

# 透视变换
perspective_transform = vtkPerspectiveTransform()
perspective_transform.SetMatrix(m)
perspective_projection = [0.0] * 3
# 透视投影
perspective_transform.TransformPoint(p, perspective_projection)
result = ', '.join([f"{x:.2f}" for x in perspective_projection])
print(f'透视投影 : ({result})')

# 普通变换
transform = vtkTransform()
transform.SetMatrix(m)
normal_projection = [0.0] * 3
# 标准投影
transform.TransformPoint(p, normal_projection)
result = ', '.join([f"{x:.2f}" for x in normal_projection])
print(f'标准投影 : ({result})')