#!/usr/bin/env python

import vtkmodules.vtkInteractionStyle
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCylinderSource
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
)

def main(argv):
    # cone = vtkConeSource() # 锥形
    # cone.SetResolution(10)
    # cone.SetHeight(20.5)
    # cone.SetRadius(3)

    cylinder = vtkCylinderSource()
    cylinder.SetHeight(3.0)  # 设置圆柱体高度
    cylinder.SetRadius(1.0)  # 设置圆柱体半径
    cylinder.SetResolution(100)  # 设置圆柱体分辨率，数值越大越光滑

    # 将几何数据转换为可渲染的图形数据。
    # 这一步就像是把原材料加工成可以展示的成品，
    # vtkPolyDataMapper 就像是一个 “加工机器”，
    # 将几何数据转换成计算机能够理解和显示的图形数据。
    datamap = vtkPolyDataMapper()
    datamap.SetInputConnection(cylinder.GetOutputPort())

    # 定义对象的显示属性（颜色、位置、旋转等）。
    # 这一步就像是给成品进行装饰和摆放，
    # 让它以更加美观和合适的方式展示出来。
    actor = vtkActor()
    actor.SetMapper(datamap)
    actor.GetProperty().SetColor(1,0,0) # red color

    # 搭建渲染场景：组合渲染器、窗口和交互器。
    # 这一步就像是搭建一个展示舞台，将所有的元素组合在一起，呈现出最终的效果。
    render = vtkRenderer()
    render.AddActor(actor)

    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(render)

    intter = vtkRenderWindowInteractor()
    intter.SetRenderWindow(ren_win)
    intter.Initialize()
    intter.Start()

if __name__ == "__main__":
    import sys
    main(sys.argv)
