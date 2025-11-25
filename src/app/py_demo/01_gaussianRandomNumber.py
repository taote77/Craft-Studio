# 本例从平均值为0.0、标准偏差为2.0的高斯分布中生成三个随机数
from vtkmodules.vtkCommonCore import vtkBoxMuellerRandomSequence

# 设置随机数的数量为3
num_rand = 3
# 设置平均值为0.0
mean = 0.0
# 设置标准偏差为2.0
standard_deviation = 2.0
# vtkBoxMuellerRandomSequence类是用Box-Mueller变换实现的高斯分布的伪随机数序列
random_sequence = vtkBoxMuellerRandomSequence()
for i in range(num_rand):
    # GetScaledValue方法根据给定的平均值和标准差返回计算出的高斯分布值
    a = random_sequence.GetScaledValue(mean, standard_deviation)
    # Next方法移动到随机序列中的下一个数字
    random_sequence.Next()
    print(f'第{i}个随机数是 : {a:6.3f}')