#!/usr/bin/env python

import sys


from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QFrame,
    QGridLayout,
    QAction,
    QMenu,
    QMenuBar,
)

from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor

from vtkmodules.vtkFiltersSources import vtkSphereSource

from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderer,
    vtkTextActor,
    vtkTexture,
)

from vtkmodules.vtkIOGeometry import (
    vtkSTLWriter,
    vtkSTLReader,
)

from vtkmodules.vtkCommonDataModel import vtkImageData

from vtkmodules.vtkCommonTransforms import vtkTransform

from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter

from vtkmodules.vtkCommonColor import vtkNamedColors

from vtkmodules.vtkInteractionWidgets import vtkTextRepresentation, vtkTextWidget

from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkCylinderSource,
    vtkSphereSource,
    vtkPlaneSource,
)

from vtkmodules.vtkCommonCore import vtkPoints

from vtkmodules.vtkCommonDataModel import (
    vtkPolyData,
    vtkCellArray,
    vtkPolyLine,
)

from vtkmodules.vtkFiltersGeneral import (
    vtkBooleanOperationPolyDataFilter,
)


from vtkmodules.vtkFiltersCore import (
    vtkTriangleFilter,
    vtkPolyDataNormals,
)

from vtkmodules.vtkCommonCore import VTK_UNSIGNED_CHAR


# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2


from application import Application

import mainwindow

import modeling

import img_process

import cv2
import numpy as np


# vtkCommonCore: 包含基础类型、数据结构、错误处理等核心组件。
# vtkCommonMath: 数学运算相关的函数和类。
# vtkCommonTransforms: 用于处理变换的类。
# vtkCommonExecutionModel: 执行模型相关的组件，如算法执行框架。
# vtkCommonDataModel: 数据模型组件，如多边形数据、点集等。
# I/O Modules:

# vtkIOCore: 输入输出核心接口。
# vtkIOXML: XML格式的读写。
# vtkIOImage: 图像数据读写，如PNG、JPEG等。
# vtkIOGeometry: 几何数据读写。
# Imaging Modules:

# vtkImagingCore: 成像核心组件，包括图像处理的基本操作。
# vtkImagingColor: 颜色空间转换等操作。
# vtkImagingFourier: 傅叶变换操作。
# Graphics Modules:

# vtkRenderingCore: 渲染核心，包括渲染器、窗口、演员等。
# vtkRenderingOpenGL2: 基于OpenGL 2.x的渲染器。
# vtkRenderingVolume: 体积渲染相关。
# vtkRenderingFreeType: 文本渲染，使用FreeType库。
# Filters Modules:

# vtkFiltersCore: 核心过滤器，如提取、裁剪、合并数据。
# vtkFiltersGeneral: 通用过滤器，如属性操作、数据转换。
# vtkFiltersGeometry: 几何过滤器，如布尔操作。
# vtkFiltersSources: 数据源，如球体、立方体等几何体生成器。
# vtkFiltersStatistics: 统计过滤器。
# vtkFiltersExtraction: 抽取特定数据的过滤器。
# vtkFiltersTexture: 纹理相关的过滤器。

import vtk


def prepare_polydata(polydata):
    """
    准备多边形数据进行布尔运算：计算法线，清理并确保几何体良好。
    """

    normals = polydata.GetPointData().GetNormals()
    # 为了简化，我们只打印第一个点的法线
    first_point_id = 0
    print("normals:", normals)
    # normal_at_first_point = normals.GetTuple3(first_point_id)

    # print(f"法线在第一个点({first_point_id})的方向为: ({normal_at_first_point[0]}, {normal_at_first_point[1]}, {normal_at_first_point[2]})")

    # 计算法线确保方向正确
    normals_filter = vtk.vtkPolyDataNormals()
    normals_filter.SetInputData(polydata)
    normals_filter.SplittingOff()  # 防止生成额外的多边形
    normals_filter.ConsistencyOn()  # 保持多边形一致性
    normals_filter.Update()
    
    # 清理并确保多边形良好
    clean_filter = vtk.vtkCleanPolyData()
    clean_filter.SetInputConnection(normals_filter.GetOutputPort())
    clean_filter.PointMergingOn()  # 合并接近的点
    clean_filter.Update()
    
    return clean_filter.GetOutput()

def boolean_operation(polydata1, polydata2, operation_type):
    """
    对两个vtkPolyData对象执行布尔运算。
    
    参数:
    polydata1: 第一个vtkPolyData对象。
    polydata2: 第二个vtkPolyData对象。
    operation_type: 布尔运算类型，可选值为"union", "intersection", "difference"。
    """
    boolean_filter = vtkBooleanOperationPolyDataFilter()

    # 根据操作类型选择过滤器类型
    if operation_type == "union":
        boolean_filter.SetOperationToUnion()
    elif operation_type == "intersection":
        boolean_filter.SetOperationToIntersection()
    elif operation_type == "difference":
        boolean_filter.SetOperationToDifference()

    else:
        raise ValueError("Invalid operation type. Choose 'union', 'intersection', or 'difference'.")
    
    boolean_filter.SetInputData(0, polydata1)
    boolean_filter.SetInputData(1, polydata2)
    boolean_filter.Update()

    # 获取运算结果
    # result_polydata = boolean_filter.GetOutput()
    # return result_polydata
    return boolean_filter


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



def GenSurface(points):
    vtk_points = vtkPoints()
    for point in points:
        x, y= point
        vtk_points.InsertNextPoint(x, y, 0)

    polygon = vtk.vtkPolygon()
    polygon.GetPointIds().SetNumberOfIds(vtk_points.GetNumberOfPoints()-1)
    for i in range(vtk_points.GetNumberOfPoints()-1):
        polygon.GetPointIds().SetId(i, i)

    poly_data = vtkPolyData()
    poly_data.SetPoints(vtk_points)

    cells = vtkCellArray()
    cells.InsertNextCell(polygon)
    poly_data.SetPolys(cells)

    return poly_data


if __name__ == "__main__":
    app = QApplication(sys.argv)
    colors = vtkNamedColors()
    colors.SetColor("bkg", [0.1, 0.2, 0.4, 1.0])

    win = mainwindow.MainWindow()

    application = Application(win.GetRenderer())

    win.create_menu(application)
    win.create_toolbar(application)
    win.show()

    img_src = "resources/S000003_P.png"

    # 读取PNG图像
    image = cv2.imread(img_src, cv2.IMREAD_GRAYSCALE)

    # 设置阈值
    threshold_value = 60  # 阈值设为200，可以根据需求调整

    # 应用阈值化处理来获取白色区域（灰度值大于阈值的部分）
    ret, thresh = cv2.threshold(image, threshold_value, 255, cv2.THRESH_BINARY)

    # contours, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)   # CHAIN_APPROX_NONE
    # contours, _ = cv2.findContours(thresh, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE) 

    # 3. 创建 VTK 组件并在 Z 方向拉伸 10mm
    vtk_points = vtkPoints()

    vtk_points_neg = vtkPoints()

    for contour_index, contour in enumerate(contours):
        hierarchy_info = hierarchy[0, contour_index]

        if hierarchy_info[3] == -1:  # 如果第四个元素为-1，表示没有父轮廓，即外轮廓
            print(f"轮廓{contour_index}是外轮廓", len(contour))
            for point in contour:
                x, y = point[0]
                # print("x, y:", x, y)
                vtk_points.InsertNextPoint(x, y, 0)  # 将点添加到VTK Points
        elif hierarchy_info[3] == 0:  # 如果第四个元素为3，表示这是一个内轮廓

            if(contour_index == 3):
                print(f"轮廓{contour_index}是内轮廓（空洞）", len(contour))

                for point in contour[::-1]:
                    x, y = point[0]
                    # print("x, y:", x, y)
                    vtk_points_neg.InsertNextPoint(x, y, 0)  # 将点添加到VTK Points
            # else:
                # 


        # x0, y0 = contour[0][0]
        # vtk_points.InsertNextPoint(x0, y0, 0)


    print("Number of contours:", len(contours))

    polygon = vtk.vtkPolygon()
    polygon.GetPointIds().SetNumberOfIds(vtk_points.GetNumberOfPoints()-1)
    for i in range(vtk_points.GetNumberOfPoints()-1):
        polygon.GetPointIds().SetId(i, i)

    polygon_neg = vtk.vtkPolygon()
    polygon_neg.GetPointIds().SetNumberOfIds(vtk_points_neg.GetNumberOfPoints()-1)
    for i in range(vtk_points_neg.GetNumberOfPoints()-1):
        polygon_neg.GetPointIds().SetId(i, i)

    # 构建多边形
    # lines = vtkCellArray()
    # num_points = vtk_points.GetNumberOfPoints()
    # print("Number of num_points:", num_points)

    # lines.InsertNextCell(num_points)
    # for i in range(num_points):
    #     # print("index:", i)
    #     lines.InsertCellPoint(i)

    # 创建多边形数据
    poly_data = vtkPolyData()
    poly_data.SetPoints(vtk_points)

    cells = vtkCellArray()
    cells.InsertNextCell(polygon)
    poly_data.SetPolys(cells)
    
    # 创建多边形数据
    poly_data_neg = vtkPolyData()
    poly_data_neg.SetPoints(vtk_points_neg)

    cells_neg = vtkCellArray()
    cells_neg.InsertNextCell(polygon_neg)
    poly_data_neg.SetPolys(cells_neg)
    

    # extrude = vtk.vtkLinearExtrusionFilter() # 创建拉伸滤波器
    # extrude.SetInputData(poly_data)
    # extrude.SetScaleFactor(10)  # 设置Z方向拉伸因子
    transform = vtk.vtkTransform()
    # 沿Z轴拉伸2倍，这里的参数分别是旋转角度(绕X,Y,Z)，平移距离(X,Y,Z)，缩放比例(X,Y,Z)
    # 因为我们只关心拉伸，旋转和平移设为默认(0,0,0)，缩放沿Z轴设置为2
    transform.Scale(1, 1, 2)

    transformFilter = vtkTransformPolyDataFilter()
    transformFilter.SetInputData(poly_data)
    transformFilter.SetTransform(transform)
    transformFilter.Update()


    # Step 9: 使用 vtkExtrusionFilter 沿Z轴拉伸平面
    extrusionFilter = vtk.vtkLinearExtrusionFilter()
    extrusionFilter.SetInputData(poly_data)  # 设置输入为之前的多边形数据
    extrusionFilter.SetVector(0, 0, 1)      # 指定拉伸方向为Z轴正方向
    extrusionFilter.SetScaleFactor(90)       # 设置拉伸的长度因子，可以根据需要调整

    extrusionFilter.Update()

    extrusionFilter1 = vtk.vtkLinearExtrusionFilter()
    extrusionFilter1.SetInputData(poly_data_neg)  # 设置输入为之前的多边形数据
    extrusionFilter1.SetVector(0, 0, 1)      # 指定拉伸方向为Z轴正方向
    extrusionFilter1.SetScaleFactor(90)       # 设置拉伸的长度因子，可以根据需要调整
    extrusionFilter1.Update()


    # ploy_d = extrusionFilter.GetOutput()
    # ploy_d1 = extrusionFilter1.GetOutput()

    com = BooleanOperation(extrusionFilter, extrusionFilter1, 0)
    # com = BooleanOperation(extrusionFilter1, extrusionFilter, 0)
    # com = BooleanOperation(extrusionFilter1, extrusionFilter, 0)

    append_filter = vtk.vtkAppendPolyData()
    # append_filter.AddInputData(poly_data)

    # reverse_sense = vtk.vtkReverseSense()
    # reverse_sense.SetInputData(poly_data_neg)

    # append_filter.AddInputData(poly_data_neg)

    prepare1 = prepare_polydata(poly_data)
    prepare2 = prepare_polydata(poly_data_neg)
    
    # b_ret = boolean_operation(poly_data, poly_data_neg, "difference")
    # b_ret = boolean_operation(prepare1, prepare2, "difference")

    mapper = vtkPolyDataMapper()
    # mapper.SetInputData(poly_data)

    mapper_neg = vtkPolyDataMapper()
    mapper_neg.SetInputData(poly_data_neg)

    # mapper.SetInputData(append_filter)
    # mapper.SetInputConnection(extrusionFilter.GetOutputPort())
    # mapper_neg.SetInputConnection(extrusionFilter1.GetOutputPort())
    mapper.SetInputConnection(com.GetOutputPort())
    # mapper.SetInputConnection(b_ret.GetOutputPort())


    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(1.0, 0.0, 0.0)  # 设置颜色，例如红色

    actor1 = vtkActor()
    actor1.SetMapper(mapper_neg)
    actor1.GetProperty().SetColor(0.0, 0.0, 1.0)  # 设置颜色，例如红色

    win.GetRenderer().AddActor(actor)
    # win.GetRenderer().AddActor(actor1)


    # There's some contours, I need show them using vtk  vtkPolyData


    # img = img_process.ReadImg(img_src)

    # contour = img_process.extract_contour(img_src)

    # ploy_data = modeling.contour_to_vtk_polydata(np.array(contour))

    # extruded_polydata = modeling.extrude_along_z(ploy_data)

    # p_map = vtkPolyDataMapper()
    # p_map.SetInputData(extruded_polydata)

    textActor = vtkTextActor()
    textActor.SetTextScaleModeToProp()
    # textActor.SetDisplayPosition(100, 100)  # 设置初始位置，相对窗口左下角
    textActor.GetTextProperty().SetColor(1.0, 1.0, 1.0)  # 设置文本颜色

    textActor.SetInput("Device Type: Ultracore Reflex RS")
    textActor.GetTextProperty().SetColor(colors.GetColor3d("Lime"))

    textActor.GetActualPosition2Coordinate().SetCoordinateSystemToNormalizedViewport()
    textActor.GetPosition2Coordinate().SetValue(0.9, 0.0)

    textActor.GetTextProperty().SetFontSize(1)
    textActor.GetTextProperty().SetFontFamilyToArial()

    win.GetRenderer().SetBackground(vtkNamedColors().GetColor3d("SlateGray"))

    win.GetRenderer().AddViewProp(textActor)

    # camera = win.GetRenderer().GetActiveCamera()
    # camera.SetPosition(0, 0, -10)
    # camera.SetFocalPoint(0, 0, 0)
    # camera.SetViewUp(0, -1, 0)
    # camera.SetViewAngle(60)

    # camera.Azimuth(10)  # 绕y轴旋转，仅为演示效果
    # # camera.Elevation(30)  # 绕x轴旋转，仅为演示效果
    # camera.Dolly(1.5)  # 缩放，仅为演示效果
    # camera.SetClippingRange(1, 1000)

    win.GetRenderer().ResetCameraClippingRange()  # 自动调整远近裁剪面

    win.GetIren().Initialize()

    sys.exit(app.exec_())
