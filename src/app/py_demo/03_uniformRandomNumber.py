# 原始C++代码来自
# https://examples.vtk.org/site/Cxx/SimpleOperations/UniformRandomNumber/
# 本例演示从均匀分布中生成0到2之间的3个随机数
# 仍然使用vtkMinimalStandardRandomSequence类
from vtkmodules.vtkCommonCore import vtkMinimalStandardRandomSequence
from datetime import datetime

# 设置随机数的数目
numRand = 3

# 创建一个随机序列生成器
randomSequence = vtkMinimalStandardRandomSequence()
# 对随机序列进行初始化，这种方式每次运行本例时得到的随机数序列是固定的
randomSequence.SetSeed(8775070)

# 也可以使用以下语句得到随时间而变换的种子
# 这种方式每次运行本例时得到的随机数序列是变换的
# randomSequence.SetSeed(int(datetime.now().timestamp()))

# 使用GetRangeValue(min, max)方法
for i in range(numRand):
    a = randomSequence.GetRangeValue(0.0, 2.0)
    randomSequence.Next()
    print(f'第{i}个随机数是 : {a:.6f}')