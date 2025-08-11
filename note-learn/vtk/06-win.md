

```python
#!/usr/bin/env python

# This simple example shows how to do basic rendering and pipeline
# creation.

from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout

from vtk.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor

import vtkmodules.vtkInteractionStyle

import vtkmodules.vtkCommonTransforms as vtkTransform
import vtkmodules.vtkRenderingOpenGL2
import vtkmodules.vtkFiltersGeneral as vtkFiltersGeneral
import vtkmodules.vtkRenderingCore as vtkRenderingCore
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkCylinderSource
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkInteractionWidgets import vtkBoxWidget

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

    # interactive loop event
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    boxWidget = vtkBoxWidget()
    boxWidget.SetInteractor(iren)
    boxWidget.SetPlaceFactor(1.25)
    boxWidget.GetOutlineProperty().SetColor(colors.GetColor3d("Gold"))

    boxWidget.SetProp3D(actor)
    boxWidget.PlaceWidget()
    callback = vtkMyCallback()
    boxWidget.AddObserver("InteractionEvent", callback)

    iren.Initialize()
    iren.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        # render.GetActiveCamera().Azimuth(1)
        # render2.GetActiveCamera().Azimuth(1)


class vtkMyCallback(object):
    """
    Callback for the interaction.
    """

    def __call__(self, caller, ev):
        t = vtkTransform()
        widget = caller
        widget.GetTransform(t)
        widget.GetProp3D().SetUserTransform(t)


if __name__ == "__main__":
    import sys

    main(sys.argv)




```