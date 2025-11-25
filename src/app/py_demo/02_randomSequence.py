
# https://examples.vtk.org/site/Cxx/SimpleOperations/RandomSequence/
# 本例演示使用随机序列生成器产生随机数的方法
# vtkMinimalStandardRandomSequence类是一个统计上独立的伪随机数序列，均匀分布在0.0到1.0之间。
# 算法来自Stephen K. Park and Keith W. Miller在1988年发表的论文
# https://dl.acm.org/doi/10.1145/63039.63042
# 《Random number generators: good ones are hard to find》
from vtkmodules.vtkCommonCore import vtkMinimalStandardRandomSequence

# 创建一个随机序列生成器
sequence = vtkMinimalStandardRandomSequence()
# 对随机序列进行初始化
sequence.SetSeed(1)

# 获得3个随机数
# 如果使用GetRangeValue(min, max)方法代替GetValue方法，可以改变随机数的取值范围
x = sequence.GetValue()
print(f'随机数x = {x:.6f}')
sequence.Next()
y = sequence.GetValue()
print(f'随机数y = {y:.6f}')
sequence.Next()
z = sequence.GetValue()
print(f'随机数z = {z:.6f}')