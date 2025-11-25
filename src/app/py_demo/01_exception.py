from vtkmodules.vtkCommonCore import vtkFloatingPointExceptions
vtkFloatingPointExceptions.Enable()

try:
    x = 0.0
    y = 1.0 / x
    print(f'x = {x:.2f}, y =  {y:.2f}')
except ZeroDivisionError:
    print("错误 : 被零除")
