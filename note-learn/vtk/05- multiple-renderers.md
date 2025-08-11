

[tutorial](https://examples.vtk.org/site/Python/Tutorial/Tutorial_Step3/)

This example demonstrates how to use multiple renderers within a render window.


在 VTK 中，SetViewport 方法的参数表示视口的位置和大小。一般情况下，这些参数是 (xmin, ymin, xmax, ymax) 的形式，其中 (xmin, ymin) 是视口的左下角坐标，(xmax, ymax) 是右上角坐标。这四个值的范围通常是从 0.0 到 1.0，表示相对于渲染窗口的比例。

对于四个视口的布局位置，可以按如下方式描述：

左上角： SetViewport(0.0, 0.5, 0.5, 1.0)
右上角： SetViewport(0.5, 0.5, 1.0, 1.0)
左下角： SetViewport(0.0, 0.0, 0.5, 0.5)
右下角： SetViewport(0.5, 0.0, 1.0, 0.5)
通过设置不同的视口位置和大小，您可以实现将一个窗口分为四个区域，并在每个区域显示不同的内容或渲染器。在以上的示例中，我们使用了相应的视口设置来实现这种布局。

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
    colors = vtkNamedColors()

    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(3.0)
    cone.SetRadius(1.0)

    datamap = vtkPolyDataMapper()

    datamap.SetInputConnection(cone.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(datamap)
    actor.GetProperty().SetColor(colors.GetColor3d("MistyRose"))

    # camera = vtkCamera()
    # camera.SetPosition(0, 0, 5)
    # camera.SetFocalPoint(0, 0, 0)

    top_left = (0.0, 0.0, 0.5, 1.0)
    top_right = (0.5, 0.0, 1.0, 1.0)
    bottom_left = (0.0, 0.0, 0.5, 0.5)
    bottom_right = (0.5, 0.0, 1.0, 0.5)

    render = vtkRenderer()
    render.AddActor(actor)
    # render.ResetCamera()
    render.SetBackground(colors.GetColor3d("RoyalBlue"))

    render.SetViewport(top_left)

    render2 = vtkRenderer()
    render2.AddActor(actor)
    # render2.ResetCamera()
    render2.SetViewport(top_right)

    renderBL = vtkRenderer()
    renderBL.AddActor(actor)
    # render2.ResetCamera()
    renderBL.SetViewport(bottom_left)

    renderBR = vtkRenderer()
    renderBR.AddActor(actor)
    # render2.ResetCamera()
    renderBR.SetViewport(bottom_right)

    # render.SetActiveCamera(camera)

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)
    ren_win.AddRenderer(render2)
    ren_win.AddRenderer(renderBL)
    ren_win.AddRenderer(renderBR)
    ren_win.SetSize(600, 600)
    ren_win.SetWindowName("Multiple")

    render.ResetCamera()
    render.GetActiveCamera().Azimuth(90)

    render2.ResetCamera()
    render2.GetActiveCamera().Azimuth(180)

    renderBL.ResetCamera()
    renderBL.GetActiveCamera().Azimuth(270)

    # # interactive loop event
    # intter = vtkRenderWindowInteractor()
    # intter.SetRenderWindow(ren_win)
    # intter.Initialize()
    # intter.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        # render.GetActiveCamera().Azimuth(1)
        # render2.GetActiveCamera().Azimuth(1)


if __name__ == "__main__":
    import sys

    main(sys.argv)


```




