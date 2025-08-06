
how to add more than one object to the scene


```python
#!/usr/bin/env python

# Boolen operate

import os

from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QFrame,
)

from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor

import vtkmodules.vtkInteractionStyle

import vtkmodules.vtkCommonTransforms as vtkTransform
import vtkmodules.vtkRenderingOpenGL2
import vtkmodules.vtkFiltersGeneral as vtkFiltersGeneral
import vtkmodules.vtkRenderingCore as vtkRenderingCore
from vtkmodules.vtkCommonColor import vtkNamedColors

from vtkmodules.vtkInteractionWidgets import vtkBoxWidget
from vtkmodules.vtkCommonCore import vtkPoints

import vtk


from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkCamera,
)

from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
)


def main(argv):

    app = QApplication(argv)

    colors = vtkNamedColors()

    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(3.0)
    cone.SetRadius(1.0)

    datamap = vtkPolyDataMapper()

    datamap.SetInputConnection(cone.GetOutputPort())

    cube = vtkCubeSource()
    cube_map = vtkPolyDataMapper()
    cube_map.SetInputConnection(cube.GetOutputPort())
    cube_act = vtkActor()
    cube_act.SetMapper(cube_map)

    actor = vtkActor()
    actor.SetMapper(datamap)
    actor.GetProperty().SetColor(colors.GetColor3d("MistyRose"))

    render = vtkRenderer()
    render.AddActor(actor)
    render.AddActor(cube_act)

    render.SetBackground(colors.GetColor3d("RoyalBlue"))

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)

    ren_win.SetSize(600, 600)
    ren_win.SetWindowName("Multiple")

    render.ResetCamera()
    render.GetActiveCamera().Azimuth(90)

    # interactive loop event
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    boxWidget = vtkBoxWidget()
    boxWidget.SetInteractor(iren)
    boxWidget.SetPlaceFactor(1.25)
    boxWidget.GetOutlineProperty().SetColor(colors.GetColor3d("Gold"))

    boxWidget.SetProp3D(actor)
    boxWidget.PlaceWidget()

    iren.Initialize()
    iren.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        # render.GetActiveCamera().Azimuth(1)
        # render2.GetActiveCamera().Azimuth(1)

# 添加模型 
def vtkAdd(model, render):
    actor=vtkActor()
    mapper=vtkPolyDataMapper()
    mapper.SetInputConnection(model.GetOutputPort())
    # mapper.SetScalarVisibility(False)
    actor.SetMapper(mapper)
    render.AddActor(actor)

def vtkAdd2(model, render):
    actor = vtkActor()
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(model.GetOutput())
    actor.SetMapper(mapper)
    render.AddActor(actor)


if __name__ == "__main__":
    import sys

    main(sys.argv)


```

when run application, it cannot work as we expected! shit 

thers's similar problem: https://stackoverflow.com/questions/59934308/how-to-add-multiple-objects-to-the-scene-in-vtk


then, how to solve it?

```python
#!/usr/bin/env python


from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QFrame,
)

from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor

import vtkmodules.vtkInteractionStyle

import vtkmodules.vtkCommonTransforms as vtkTransform
import vtkmodules.vtkRenderingOpenGL2
import vtkmodules.vtkRenderingCore as vtkRenderingCore
from vtkmodules.vtkCommonColor import vtkNamedColors

from vtkmodules.vtkInteractionWidgets import vtkBoxWidget
from vtkmodules.vtkCommonCore import vtkPoints

from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkCamera,
)

from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
)

from vtkmodules.vtkFiltersGeneral import (
    vtkBooleanOperationPolyDataFilter,
)

from vtkmodules.vtkFiltersCore import (
    vtkTriangleFilter,
)


def main(argv):

    app = QApplication(argv)

    colors = vtkNamedColors()

    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(3.0)
    cone.SetRadius(1.0)
    cone.Update()

    cube = vtkCubeSource()
    cube.Update()

    render = vtkRenderer()

    BooleanOperation(cone, cube, 1)

    vtkAdd2(BooleanOperation(cone, cube, 2), render)

    render.SetBackground(colors.GetColor3d("RoyalBlue"))

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)

    ren_win.SetSize(600, 600)
    ren_win.SetWindowName("Multiple")

    render.ResetCamera()
    render.GetActiveCamera().Azimuth(90)

    # interactive loop event
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    iren.Initialize()
    iren.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        # render.GetActiveCamera().Azimuth(1)
        # render2.GetActiveCamera().Azimuth(1)


# 添加模型
def vtkAdd(model, render):
    actor = vtkActor()
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(model.GetOutputPort())
    mapper.SetScalarVisibility(False)
    actor.SetMapper(mapper)
    render.AddActor(actor)


def vtkAdd2(model, render):
    actor = vtkActor()
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(model.GetOutput())
    actor.SetMapper(mapper)
    mapper.SetScalarVisibility(False)

    actor.GetProperty().SetColor(1.0, 0.5, 0.3)  # 设置交集形状的颜色为橙色
    actor.GetProperty().SetLineWidth(2)  # 设置线条宽度为2
    actor.GetProperty().SetEdgeColor(0.8, 0.3, 0.2)  # 设置线条颜色为蓝色
    render.AddActor(actor)


# comment the funtion to use
def BooleanOperation(model1, model2, op_type: int):
    operator = vtkBooleanOperationPolyDataFilter()

    # this is esencially
    tri1 = vtkTriangleFilter()
    tri1.SetInputData(model1.GetOutput())
    tri1.Update()

    tri2 = vtkTriangleFilter()
    tri2.SetInputData(model2.GetOutput())
    tri2.Update()

    operator.SetInputData(0, tri1.GetOutput())
    operator.SetInputData(1, tri2.GetOutput())
    if op_type == 0:
        operator.SetOperationToDifference()
        print("Difference")
    elif op_type == 1:
        operator.SetOperationToUnion()
        print("Union")
    elif op_type == 2:
        operator.SetOperationToIntersection()
        print("Intersection")
    else:
        print("Error")
        return
    operator.Update()

    return operator


if __name__ == "__main__":
    import sys

    main(sys.argv)

```



save the geometry to stl file

```python

#!/usr/bin/env python


from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QFrame,
)

import vtkmodules.vtkInteractionStyle

import vtkmodules.vtkCommonTransforms as vtkTransform
import vtkmodules.vtkRenderingOpenGL2

from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkInteractionWidgets import vtkBoxWidget
from vtkmodules.vtkCommonCore import vtkPoints


from vtkmodules.vtkIOGeometry import (
    vtkSTLWriter,
)

from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkCamera,
)

from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
)

from vtkmodules.vtkFiltersGeneral import (
    vtkBooleanOperationPolyDataFilter,
)

from vtkmodules.vtkFiltersCore import (
    vtkTriangleFilter,
)


def main(argv):

    app = QApplication(argv)

    colors = vtkNamedColors()

    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(3.0)
    cone.SetRadius(1.0)
    cone.Update()

    cube = vtkCubeSource()
    cube.Update()

    render = vtkRenderer()

    vtkAdd2(BooleanOperation(cone, cube, 0), render)

    SaveToStl(BooleanOperation(cone, cube, 0), "multiple.stl")

    render.SetBackground(colors.GetColor3d("RoyalBlue"))

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)

    ren_win.SetSize(600, 600)
    ren_win.SetWindowName("Multiple")

    render.ResetCamera()
    render.GetActiveCamera().Azimuth(90)

    # interactive loop event
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    iren.Initialize()
    iren.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        # render.GetActiveCamera().Azimuth(1)
        # render2.GetActiveCamera().Azimuth(1)

    return 0


# 添加模型
def vtkAdd(model, render):
    actor = vtkActor()
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(model.GetOutputPort())
    mapper.SetScalarVisibility(False)
    actor.SetMapper(mapper)
    render.AddActor(actor)


def vtkAdd2(model, render):
    actor = vtkActor()
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(model.GetOutput())
    actor.SetMapper(mapper)
    mapper.SetScalarVisibility(False)

    actor.GetProperty().SetColor(1.0, 0.5, 0.3)  # 设置交集形状的颜色为橙色
    actor.GetProperty().SetLineWidth(2)  # 设置线条宽度为2
    actor.GetProperty().SetEdgeColor(0.8, 0.3, 0.2)  # 设置线条颜色为蓝色
    render.AddActor(actor)


# comment the funtion to use
def BooleanOperation(model1, model2, op_type: int):
    operator = vtkBooleanOperationPolyDataFilter()

    # this is esencially
    tri1 = vtkTriangleFilter()
    tri1.SetInputData(model1.GetOutput())
    tri1.Update()

    tri2 = vtkTriangleFilter()
    tri2.SetInputData(model2.GetOutput())
    tri2.Update()

    operator.SetInputData(0, tri1.GetOutput())
    operator.SetInputData(1, tri2.GetOutput())
    if op_type == 0:
        operator.SetOperationToDifference()
        print("Difference")
    elif op_type == 1:
        operator.SetOperationToUnion()
        print("Union")
    elif op_type == 2:
        operator.SetOperationToIntersection()
        print("Intersection")
    else:
        print("Error")
        return
    operator.Update()

    return operator


def SaveToStl(model, path):
    writer = vtkSTLWriter()
    writer.SetInputConnection(model.GetOutputPort())
    writer.SetFileName(path)
    writer.Write()


if __name__ == "__main__":
    import sys

    main(sys.argv)


```