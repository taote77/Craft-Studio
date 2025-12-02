#!/usr/bin/env python3
"""
STL测试数据生成器
创建各种几何形状的STL文件用于壳体拆分测试
"""

import vtk
import numpy as np
import os

def create_sphere_stl(filename, radius=1.0, resolution=20):
    """创建球体STL文件"""
    sphere = vtk.vtkSphereSource()
    sphere.SetRadius(radius)
    sphere.SetThetaResolution(resolution)
    sphere.SetPhiResolution(resolution)
    sphere.Update()
    
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(filename)
    writer.SetInputConnection(sphere.GetOutputPort())
    writer.Write()
    print(f"创建球体STL: {filename}")

def create_cube_stl(filename, size=2.0):
    """创建立方体STL文件"""
    cube = vtk.vtkCubeSource()
    cube.SetXLength(size)
    cube.SetYLength(size)
    cube.SetZLength(size)
    cube.Update()
    
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(filename)
    writer.SetInputConnection(cube.GetOutputPort())
    writer.Write()
    print(f"创建立方体STL: {filename}")

def create_cylinder_stl(filename, radius=1.0, height=2.0, resolution=30):
    """创建圆柱体STL文件"""
    cylinder = vtk.vtkCylinderSource()
    cylinder.SetRadius(radius)
    cylinder.SetHeight(height)
    cylinder.SetResolution(resolution)
    cylinder.Update()
    
    # 旋转圆柱体使其沿Z轴
    transform = vtk.vtkTransform()
    transform.RotateX(90)
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetTransform(transform)
    transform_filter.SetInputConnection(cylinder.GetOutputPort())
    transform_filter.Update()
    
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(filename)
    writer.SetInputConnection(transform_filter.GetOutputPort())
    writer.Write()
    print(f"创建圆柱体STL: {filename}")

def create_torus_stl(filename, ring_radius=1.0, cross_radius=0.3, resolution=20):
    """创建环面STL文件"""
    torus = vtk.vtkParametricTorus()
    torus.SetRingRadius(ring_radius)
    torus.SetCrossSectionRadius(cross_radius)
    
    source = vtk.vtkParametricFunctionSource()
    source.SetParametricFunction(torus)
    source.SetUResolution(resolution)
    source.SetVResolution(resolution)
    source.Update()
    
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(filename)
    writer.SetInputConnection(source.GetOutputPort())
    writer.Write()
    print(f"创建环面STL: {filename}")

def create_complex_shape_stl(filename):
    """创建复杂组合形状STL文件"""
    # 创建球体
    sphere = vtk.vtkSphereSource()
    sphere.SetRadius(1.0)
    sphere.SetThetaResolution(20)
    sphere.SetPhiResolution(20)
    
    # 创建立方体
    cube = vtk.vtkCubeSource()
    cube.SetXLength(1.5)
    cube.SetYLength(1.5)
    cube.SetZLength(1.5)
    
    # 组合形状
    append_filter = vtk.vtkAppendPolyData()
    append_filter.AddInputConnection(sphere.GetOutputPort())
    append_filter.AddInputConnection(cube.GetOutputPort())
    append_filter.Update()
    
    # 清理和三角化
    cleaner = vtk.vtkCleanPolyData()
    cleaner.SetInputConnection(append_filter.GetOutputPort())
    cleaner.Update()
    
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(filename)
    writer.SetInputConnection(cleaner.GetOutputPort())
    writer.Write()
    print(f"创建复杂形状STL: {filename}")

def main():
    """生成所有测试STL文件"""
    output_dir = "/home/shane/workspace/CraftStudio/src/app/py_demo/test_stl"
    os.makedirs(output_dir, exist_ok=True)
    
    # 生成各种测试形状
    create_sphere_stl(f"{output_dir}/sphere.stl")
    create_cube_stl(f"{output_dir}/cube.stl")
    create_cylinder_stl(f"{output_dir}/cylinder.stl")
    create_torus_stl(f"{output_dir}/torus.stl")
    create_complex_shape_stl(f"{output_dir}/complex.stl")
    
    print(f"\n所有测试STL文件已生成到: {output_dir}")

if __name__ == "__main__":
    main()