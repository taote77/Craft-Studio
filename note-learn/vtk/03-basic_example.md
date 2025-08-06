
[VTK tutorial](https://docs.vtk.org/en/latest/getting_started/index.html)


pip install vtk

```shel
python -m venv ./env
source ./env/bin/activate
pip install vtk

```


The basic setup of source -> mapper -> actor -> renderer -> renderwindow is typical of most VTK programs.



```python
#!/usr/bin/env python

# This simple example shows how to do basic rendering and pipeline
# creation.

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
    cone = vtkConeSource()
    cone.SetResolution(10)
    cone.SetHeight(20.5)
    cone.SetRadius(3)

    datamap = vtkPolyDataMapper()
    datamap.SetInputConnection(cone.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(datamap)

    render = vtkRenderer()
    render.AddActor(actor)

    ren_win = vtkRenderWindow()

    ren_win.AddRenderer(render)

    ## interactive loop event
    # intter = vtkRenderWindowInteractor()
    # intter.SetRenderWindow(ren_win)
    # intter.Initialize()
    # intter.Start()

    # manual control render
    for i in range(1, 1000):
        ren_win.Render()
        render.GetActiveCamera().Azimuth(1)


if __name__ == "__main__":
    import sys

    main(sys.argv)

```





[Qttesting tookit](https://github.com/Kitware/QtTesting.git)

