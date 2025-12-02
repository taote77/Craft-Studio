#!/usr/bin/env python3
import vtk
import os

def analyze_stl_file(filename):
    """分析STL文件的几何信息"""
    reader = vtk.vtkSTLReader()
    reader.SetFileName(filename)
    reader.Update()
    
    polydata = reader.GetOutput()
    
    # 获取基本统计信息
    num_points = polydata.GetNumberOfPoints()
    num_cells = polydata.GetNumberOfCells()
    
    # 计算边界框
    bounds = polydata.GetBounds()
    center = polydata.GetCenter()
    
    # 计算表面积
    mass_filter = vtk.vtkMassProperties()
    mass_filter.SetInputData(polydata)
    mass_filter.Update()
    surface_area = mass_filter.GetSurfaceArea()
    
    return {
        'num_points': num_points,
        'num_cells': num_cells,
        'bounds': bounds,
        'center': center,
        'surface_area': surface_area
    }

def generate_offscreen_image(input_files, output_image="shells_visualization.png"):
    """生成离屏渲染的图像"""
    renderer = vtk.vtkRenderer()
    render_window = vtk.vtkRenderWindow()
    render_window.SetOffScreenRendering(1)  # 离屏渲染
    render_window.AddRenderer(renderer)
    render_window.SetSize(800, 600)
    
    # 设置背景
    renderer.SetBackground(0.2, 0.3, 0.4)
    
    colors = [
        (1.0, 0.0, 0.0),  # 红色
        (0.0, 1.0, 0.0),  # 绿色
        (0.0, 0.0, 1.0),  # 蓝色
        (1.0, 1.0, 0.0),  # 黄色
        (1.0, 0.0, 1.0),  # 洋红色
        (0.0, 1.0, 1.0),  # 青色
    ]
    
    for i, filename in enumerate(input_files):
        if not os.path.exists(filename):
            print(f"文件不存在: {filename}")
            continue
            
        reader = vtk.vtkSTLReader()
        reader.SetFileName(filename)
        reader.Update()
        
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(reader.GetOutput())
        mapper.ScalarVisibilityOff()
        
        actor = vtk.vtkActor()
        actor.SetMapper(mapper)
        
        # 设置颜色
        color = colors[i % len(colors)]
        actor.GetProperty().SetColor(color)
        actor.GetProperty().SetOpacity(0.8)
        
        renderer.AddActor(actor)
    
    # 设置相机位置
    renderer.ResetCamera()
    camera = renderer.GetActiveCamera()
    camera.SetPosition(5, 5, 5)
    camera.SetFocalPoint(0, 0, 0)
    camera.SetViewUp(0, 0, 1)
    
    # 渲染并保存图像
    render_window.Render()
    
    window_to_image = vtk.vtkWindowToImageFilter()
    window_to_image.SetInput(render_window)
    window_to_image.Update()
    
    writer = vtk.vtkPNGWriter()
    writer.SetFileName(output_image)
    writer.SetInputConnection(window_to_image.GetOutputPort())
    writer.Write()
    
    print(f"✅ 已保存可视化图像: {output_image}")

def main():
    # 分析原始文件和拆分结果
    input_files = [
        "nested_shells.stl",
        "split_result_000.stl", 
        "split_result_001.stl",
        "split_result_002.stl"
    ]
    
    print("=== STL文件分析结果 ===")
    
    for filename in input_files:
        if os.path.exists(filename):
            stats = analyze_stl_file(filename)
            print(f"\n📁 {filename}:")
            print(f"   点数: {stats['num_points']}")
            print(f"   面数: {stats['num_cells']}")
            print(f"   表面积: {stats['surface_area']:.2f}")
            print(f"   中心点: ({stats['center'][0]:.2f}, {stats['center'][1]:.2f}, {stats['center'][2]:.2f})")
            print(f"   边界框: X[{stats['bounds'][0]:.2f}, {stats['bounds'][1]:.2f}], "
                  f"Y[{stats['bounds'][2]:.2f}, {stats['bounds'][3]:.2f}], "
                  f"Z[{stats['bounds'][4]:.2f}, {stats['bounds'][5]:.2f}]")
        else:
            print(f"❌ 文件不存在: {filename}")
    
    # 生成可视化图像
    shell_files = ["split_result_000.stl", "split_result_001.stl", "split_result_002.stl"]
    generate_offscreen_image(shell_files, "shell_split_visualization.png")
    
    # 验证拆分完整性
    print("\n=== 拆分完整性验证 ===")
    original_stats = analyze_stl_file("nested_shells.stl")
    total_area = 0
    total_points = 0
    total_cells = 0
    
    for filename in shell_files:
        if os.path.exists(filename):
            stats = analyze_stl_file(filename)
            total_area += stats['surface_area']
            total_points += stats['num_points']
            total_cells += stats['num_cells']
    
    print(f"原始模型 - 表面积: {original_stats['surface_area']:.2f}")
    print(f"拆分总和 - 表面积: {total_area:.2f}")
    print(f"面积差异: {abs(original_stats['surface_area'] - total_area):.2f}")
    print(f"面积匹配率: {(total_area / original_stats['surface_area'] * 100):.1f}%")

if __name__ == "__main__":
    main()