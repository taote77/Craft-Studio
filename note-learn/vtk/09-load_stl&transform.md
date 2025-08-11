

加载stl文件，并且反转显示

```python

    platform_file = "resources/platform.stl"

    reader = vtkSTLReader()
    reader.SetFileName(platform_file)
    reader.Update()

    transform = vtkTransform()
    # transform.RotateZ(180)  #a 旋转180度，使模型面向下

    transform_filter = vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(reader.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.GetTransform().Scale(-1, -1, -1)  #b 反转法线
    transform_filter.Update()

    mapper1 = vtkPolyDataMapper()
    mapper1.SetInputConnection(transform_filter.GetOutputPort())
    actor1 = vtkActor()
    actor1.SetMapper(mapper1)

    # a, b 任意一处代码可以反转模型180°显示

```


各个模块

Core Modules:

vtkCommonCore: 包含基础类型、数据结构、错误处理等核心组件。
vtkCommonMath: 数学运算相关的函数和类。
vtkCommonTransforms: 用于处理变换的类。
vtkCommonExecutionModel: 执行模型相关的组件，如算法执行框架。
vtkCommonDataModel: 数据模型组件，如多边形数据、点集等。
I/O Modules:

vtkIOCore: 输入输出核心接口。
vtkIOXML: XML格式的读写。
vtkIOImage: 图像数据读写，如PNG、JPEG等。
vtkIOGeometry: 几何数据读写。
Imaging Modules:

vtkImagingCore: 成像核心组件，包括图像处理的基本操作。
vtkImagingColor: 颜色空间转换等操作。
vtkImagingFourier: 傅叶变换操作。
Graphics Modules:

vtkRenderingCore: 渲染核心，包括渲染器、窗口、演员等。
vtkRenderingOpenGL2: 基于OpenGL 2.x的渲染器。
vtkRenderingVolume: 体积渲染相关。
vtkRenderingFreeType: 文本渲染，使用FreeType库。
Filters Modules:

vtkFiltersCore: 核心过滤器，如提取、裁剪、合并数据。
vtkFiltersGeneral: 通用过滤器，如属性操作、数据转换。
vtkFiltersGeometry: 几何过滤器，如布尔操作。
vtkFiltersSources: 数据源，如球体、立方体等几何体生成器。
vtkFiltersStatistics: 统计过滤器。
vtkFiltersExtraction: 抽取特定数据的过滤器。
vtkFiltersTexture: 纹理相关的过滤器。
Infovis Modules:

vtkInfovisCore: 信息可视化核心组件。
vtkInfovisLayout: 布局管理。
Parallel Modules:

vtkParallelCore: 并行计算核心。
vtkParallelMPI: 基于MPI的并行支持。
Widgets Modules:

vtkWidgetsCore: 用户界面小部件核心，如滑块、旋钮等。
Domains Specific Modules:

如 vtkDICOM: DICOM医学图像处理。
vtkWeb: Web支持相关组件。


