#!/usr/bin/env python3
import vtk
import os
import math

def create_touching_geometries():
    """创建接触型连通测试数据 - 两个几何体相互接触但独立"""
    print("🔨 创建接触型连通测试数据...")
    
    append_filter = vtk.vtkAppendPolyData()
    
    # 创建第一个球体 - 左侧
    sphere1 = vtk.vtkSphereSource()
    sphere1.SetRadius(1.0)
    sphere1.SetCenter(-1.0, 0, 0)
    sphere1.SetThetaResolution(30)
    sphere1.SetPhiResolution(30)
    sphere1.Update()
    append_filter.AddInputData(sphere1.GetOutput())
    
    # 创建第二个球体 - 右侧，与第一个球体接触
    sphere2 = vtk.vtkSphereSource()
    sphere2.SetRadius(1.0)
    sphere2.SetCenter(1.0, 0, 0)
    sphere2.SetThetaResolution(30)
    sphere2.SetPhiResolution(30)
    sphere2.Update()
    append_filter.AddInputData(sphere2.GetOutput())
    
    # 创建立方体 - 位于两球之间，与两个球体都接触
    cube = vtk.vtkCubeSource()
    cube.SetXLength(0.8)
    cube.SetYLength(0.8)
    cube.SetZLength(0.8)
    cube.SetCenter(0, 0, 0)
    cube.Update()
    append_filter.AddInputData(cube.GetOutput())
    
    append_filter.Update()
    
    # 保存文件
    writer = vtk.vtkSTLWriter()
    writer.SetFileName("touching_geometries.stl")
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    
    print("✅ 已生成接触型连通测试数据: touching_geometries.stl")
    print("   包含3个相互接触的几何体（2个球体+1个立方体）")
    return "touching_geometries.stl"

def create_penetrating_geometries():
    """创建穿透型连通测试数据 - 几何体相互穿透"""
    print("🔨 创建穿透型连通测试数据...")
    
    append_filter = vtk.vtkAppendPolyData()
    
    # 创建第一个立方体
    cube1 = vtk.vtkCubeSource()
    cube1.SetXLength(3.0)
    cube1.SetYLength(3.0)
    cube1.SetZLength(3.0)
    cube1.SetCenter(0, 0, 0)
    cube1.Update()
    append_filter.AddInputData(cube1.GetOutput())
    
    # 创建第二个立方体，与第一个立方体交叉
    cube2 = vtk.vtkCubeSource()
    cube2.SetXLength(3.0)
    cube2.SetYLength(3.0)
    cube2.SetZLength(3.0)
    cube2.SetCenter(1.0, 1.0, 1.0)
    cube2.Update()
    append_filter.AddInputData(cube2.GetOutput())
    
    # 创建球体，穿透两个立方体
    sphere = vtk.vtkSphereSource()
    sphere.SetRadius(2.0)
    sphere.SetCenter(0.5, 0.5, 0.5)
    sphere.SetThetaResolution(30)
    sphere.SetPhiResolution(30)
    sphere.Update()
    append_filter.AddInputData(sphere.GetOutput())
    
    append_filter.Update()
    
    # 保存文件
    writer = vtk.vtkSTLWriter()
    writer.SetFileName("penetrating_geometries.stl")
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    
    print("✅ 已生成穿透型连通测试数据: penetrating_geometries.stl")
    print("   包含3个相互穿透的几何体（2个立方体+1个球体）")
    return "penetrating_geometries.stl"

def create_nested_complex():
    """创建嵌套复杂连通测试数据 - 多个几何体嵌套组合"""
    print("🔨 创建嵌套复杂连通测试数据...")
    
    append_filter = vtk.vtkAppendPolyData()
    
    # 创建外层立方体
    outer_cube = vtk.vtkCubeSource()
    outer_cube.SetXLength(6.0)
    outer_cube.SetYLength(6.0)
    outer_cube.SetZLength(6.0)
    outer_cube.SetCenter(0, 0, 0)
    outer_cube.Update()
    append_filter.AddInputData(outer_cube.GetOutput())
    
    # 创建中层球壳
    middle_sphere = vtk.vtkSphereSource()
    middle_sphere.SetRadius(2.5)
    middle_sphere.SetCenter(0, 0, 0)
    middle_sphere.SetThetaResolution(30)
    middle_sphere.SetPhiResolution(30)
    middle_sphere.Update()
    append_filter.AddInputData(middle_sphere.GetOutput())
    
    # 创建内层圆柱体
    inner_cylinder = vtk.vtkCylinderSource()
    inner_cylinder.SetRadius(1.0)
    inner_cylinder.SetHeight(4.0)
    inner_cylinder.SetResolution(20)
    inner_cylinder.Update()
    
    # 旋转圆柱体
    cylinder_transform = vtk.vtkTransform()
    cylinder_transform.RotateX(90)
    
    cylinder_transform_filter = vtk.vtkTransformPolyDataFilter()
    cylinder_transform_filter.SetTransform(cylinder_transform)
    cylinder_transform_filter.SetInputData(inner_cylinder.GetOutput())
    cylinder_transform_filter.Update()
    
    append_filter.AddInputData(cylinder_transform_filter.GetOutput())
    
    # 创建小立方体 - 放置在内层
    small_cube = vtk.vtkCubeSource()
    small_cube.SetXLength(1.0)
    small_cube.SetYLength(1.0)
    small_cube.SetZLength(1.0)
    small_cube.SetCenter(0, 0, 0)
    small_cube.Update()
    append_filter.AddInputData(small_cube.GetOutput())
    
    append_filter.Update()
    
    # 保存文件
    writer = vtk.vtkSTLWriter()
    writer.SetFileName("nested_complex_geometries.stl")
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    
    print("✅ 已生成嵌套复杂连通测试数据: nested_complex_geometries.stl")
    print("   包含4个嵌套几何体（外立方体+球壳+圆柱+内立方体）")
    return "nested_complex_geometries.stl"

def create_sparse_distribution():
    """创建稀疏分布测试数据 - 几何体空间分散分布"""
    print("🔨 创建稀疏分布测试数据...")
    
    append_filter = vtk.vtkAppendPolyData()
    
    # 在3D空间中分散放置多个几何体
    geometries = [
        # 球体在不同位置
        {"type": "sphere", "pos": [0, 0, 0], "size": 1.0},
        {"type": "sphere", "pos": [5, 0, 0], "size": 0.8},
        {"type": "sphere", "pos": [-5, 0, 0], "size": 1.2},
        
        # 立方体在不同位置
        {"type": "cube", "pos": [0, 5, 0], "size": 1.5},
        {"type": "cube", "pos": [0, -5, 0], "size": 1.0},
        
        # 圆锥体在不同位置
        {"type": "cone", "pos": [0, 0, 5], "size": 1.0},
        {"type": "cone", "pos": [0, 0, -5], "size": 0.8},
        
        # 圆柱体在对角位置
        {"type": "cylinder", "pos": [3, 3, 3], "size": 0.6},
        {"type": "cylinder", "pos": [-3, -3, -3], "size": 0.7},
    ]
    
    for i, geom in enumerate(geometries):
        if geom["type"] == "sphere":
            source = vtk.vtkSphereSource()
            source.SetRadius(geom["size"])
            source.SetCenter(*geom["pos"])
            source.SetThetaResolution(20)
            source.SetPhiResolution(20)
            
        elif geom["type"] == "cube":
            source = vtk.vtkCubeSource()
            source.SetXLength(geom["size"] * 2)
            source.SetYLength(geom["size"] * 2)
            source.SetZLength(geom["size"] * 2)
            source.SetCenter(*geom["pos"])
            
        elif geom["type"] == "cone":
            source = vtk.vtkConeSource()
            source.SetRadius(geom["size"])
            source.SetHeight(geom["size"] * 2)
            source.SetCenter(*geom["pos"])
            source.SetResolution(15)
            
        elif geom["type"] == "cylinder":
            source = vtk.vtkCylinderSource()
            source.SetRadius(geom["size"])
            source.SetHeight(geom["size"] * 3)
            source.SetResolution(15)
            
            # 旋转圆柱体
            transform = vtk.vtkTransform()
            transform.RotateX(90)
            transform.Translate(*geom["pos"])
            
            transform_filter = vtk.vtkTransformPolyDataFilter()
            transform_filter.SetTransform(transform)
            transform_filter.SetInputConnection(source.GetOutputPort())
            
            transform_filter.Update()
            append_filter.AddInputData(transform_filter.GetOutput())
            continue
        
        source.Update()
        append_filter.AddInputData(source.GetOutput())
    
    append_filter.Update()
    
    # 保存文件
    writer = vtk.vtkSTLWriter()
    writer.SetFileName("sparse_distribution_geometries.stl")
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    
    print("✅ 已生成稀疏分布测试数据: sparse_distribution_geometries.stl")
    print(f"   包含{len(geometries)}个空间分散的几何体")
    return "sparse_distribution_geometries.stl"

def create_mixed_complexity():
    """创建混合复杂度测试数据 - 包含不同复杂度的几何体"""
    print("🔨 创建混合复杂度测试数据...")
    
    append_filter = vtk.vtkAppendPolyData()
    
    # 简单几何体 - 平面
    plane = vtk.vtkPlaneSource()
    plane.SetOrigin(-2, -2, 0)
    plane.SetPoint1(2, -2, 0)
    plane.SetPoint2(-2, 2, 0)
    plane.SetXResolution(10)
    plane.SetYResolution(10)
    plane.Update()
    append_filter.AddInputData(plane.GetOutput())
    
    # 中等复杂度 - 参数化表面
    superquadric = vtk.vtkSuperquadricSource()
    superquadric.SetToroidal(0)
    superquadric.SetPhiRoundness(2.0)
    superquadric.SetThetaRoundness(2.0)
    superquadric.SetScale(1.5, 1.0, 0.8)
    superquadric.SetThetaResolution(30)
    superquadric.SetPhiResolution(30)
    superquadric.Update()
    
    transform = vtk.vtkTransform()
    transform.Translate(3, 0, 0)
    
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetTransform(transform)
    transform_filter.SetInputData(superquadric.GetOutput())
    transform_filter.Update()
    append_filter.AddInputData(transform_filter.GetOutput())
    
    # 复杂几何体 - 环面
    torus = vtk.vtkParametricTorus()
    torus.SetRingRadius(1.5)
    torus.SetCrossSectionRadius(0.3)
    
    torus_source = vtk.vtkParametricFunctionSource()
    torus_source.SetParametricFunction(torus)
    torus_source.SetUResolution(40)
    torus_source.SetVResolution(25)
    torus_source.Update()
    
    torus_transform = vtk.vtkTransform()
    torus_transform.Translate(-3, 0, 0)
    
    torus_transform_filter = vtk.vtkTransformPolyDataFilter()
    torus_transform_filter.SetTransform(torus_transform)
    torus_transform_filter.SetInputData(torus_source.GetOutput())
    torus_transform_filter.Update()
    append_filter.AddInputData(torus_transform_filter.GetOutput())
    
    # 精细几何体 - 高分辨率球体
    fine_sphere = vtk.vtkSphereSource()
    fine_sphere.SetRadius(0.8)
    fine_sphere.SetCenter(0, 3, 0)
    fine_sphere.SetThetaResolution(50)
    fine_sphere.SetPhiResolution(50)
    fine_sphere.Update()
    append_filter.AddInputData(fine_sphere.GetOutput())
    
    append_filter.Update()
    
    # 保存文件
    writer = vtk.vtkSTLWriter()
    writer.SetFileName("mixed_complexity_geometries.stl")
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    
    print("✅ 已生成混合复杂度测试数据: mixed_complexity_geometries.stl")
    print("   包含4个不同复杂度的几何体（平面+超二次曲面+环面+高精度球体）")
    return "mixed_complexity_geometries.stl"

def main():
    print("=== 创建高级连通类型测试数据 ===\n")
    
    test_files = []
    
    # 创建各种类型的测试数据
    test_files.append(create_touching_geometries())
    test_files.append(create_penetrating_geometries())
    test_files.append(create_nested_complex())
    test_files.append(create_sparse_distribution())
    test_files.append(create_mixed_complexity())
    
    print(f"\n🎉 所有测试数据创建完成！")
    print(f"   生成了 {len(test_files)} 个不同连通类型的测试文件:")
    for file in test_files:
        print(f"   📁 {file}")
    
    return test_files

if __name__ == "__main__":
    main()