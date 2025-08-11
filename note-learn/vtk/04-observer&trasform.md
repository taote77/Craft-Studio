
in this section, we will learn about observer，camera， Filter

first of all, we will add an observer to listern event, add will exec certain callback func


vtkShrinkPolyData 和 vtkTransform 是 VTK 中用于不同目的的两个类：

vtkShrinkPolyData：

vtkShrinkPolyData 是一个过滤器（filter），用于缩小多边形数据集的尺寸。    - 它通过减少每个单元格的尺寸来对输入数据进行缩小，但保持其形状不变。    - 此过滤器可用于减小模型的大小，使其更易于显示，而无需改变对象形状。
vtkTransform：

vtkTransform 是一种表示线性和非线性坐标变换的方法。
您可以使用 vtkTransform 来实现平移、旋转、缩放等变换操作，以控制对象在空间中的位置、方向和尺寸。
vtkTransform 通常用于在虚拟环境中对对象进行视觉上的变换，而不影响对象本身的几何数据。
因此，vtkShrinkPolyData 主要用于缩小数据集的尺寸，而 vtkTransform 则主要用于对对象进行平移、旋转或缩放等几何变换。这两者用途和行为不同，适用于不同的场景和需求。


## observer 


```python
#!/usr/bin/env python

#!/usr/bin/env python

# This simple example shows how to do basic rendering and pipeline
# creation.

import vtkmodules.vtkInteractionStyle

import vtkmodules.vtkCommonTransforms as vtkTransform
import vtkmodules.vtkRenderingOpenGL2
import vtkmodules.vtkFiltersGeneral as vtkFiltersGeneral
import vtkmodules.vtkRenderingCore as vtkRenderingCore
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCylinderSource
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkCamera,
)


def main(argv):
    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(0.9)
    cone.SetRadius(0.5)

    datamap = vtkPolyDataMapper()

    datamap.SetInputConnection(cone.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(datamap)

    render = vtkRenderer()
    render.AddActor(actor)

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)

    mo1 = vtkMyCallback(render, actor)
    render.AddObserver("StartEvent", mo1)

    # # interactive loop event
    # intter = vtkRenderWindowInteractor()
    # intter.SetRenderWindow(ren_win)
    # intter.Initialize()
    # intter.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        render.GetActiveCamera().Azimuth(1)


class vtkMyCallback(object):
    """
    Callback for the interaction.
    """

    def __init__(self, renderer, actor):
        self.renderer = renderer
        self.actor = actor

    def __call__(self, caller, ev):
        position = self.renderer.GetActiveCamera().GetPosition()
        print("({:5.2f}, {:5.2f}, {:5.2f})".format(*position))




if __name__ == "__main__":
    import sys

    main(sys.argv)


```



## camera




```python
#!/usr/bin/env python

# This simple example shows how to do basic rendering and pipeline
# creation.

import vtkmodules.vtkInteractionStyle

import vtkmodules.vtkCommonTransforms as vtkTransform
import vtkmodules.vtkRenderingOpenGL2
import vtkmodules.vtkFiltersGeneral as vtkFiltersGeneral
import vtkmodules.vtkRenderingCore as vtkRenderingCore
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCylinderSource
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkCamera,
)


def main(argv):
    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(0.9)
    cone.SetRadius(0.5)

    datamap = vtkPolyDataMapper()

    datamap.SetInputConnection(cone.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(datamap)

    camera = vtkCamera()
    camera.SetPosition(0, 0, 5)
    camera.SetFocalPoint(0, 0, 0)

    render = vtkRenderer()
    render.AddActor(actor)
    render.ResetCamera()
    render.SetActiveCamera(camera)

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)

    mo1 = vtkMyCallback(render, actor)
    render.AddObserver("StartEvent", mo1)

    # # interactive loop event
    # intter = vtkRenderWindowInteractor()
    # intter.SetRenderWindow(ren_win)
    # intter.Initialize()
    # intter.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        render.GetActiveCamera().Azimuth(1)


class vtkMyCallback(object):
    """
    Callback for the interaction.
    """

    def __init__(self, renderer, actor):
        self.renderer = renderer
        self.actor = actor

    def __call__(self, caller, ev):
        position = self.renderer.GetActiveCamera().GetPosition()
        print("({:5.2f}, {:5.2f}, {:5.2f})".format(*position))

        po = self.actor.GetPosition()
        print("({:5.2f}, {:5.2f}, {:5.2f})".format(*po))


if __name__ == "__main__":
    import sys

    main(sys.argv)


```



## shrink

......




