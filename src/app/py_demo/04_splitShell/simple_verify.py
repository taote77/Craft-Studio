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

def main():
    print("=== 三维模型壳体拆分验证报告 ===\n")
    
    # 分析原始文件和拆分结果
    files_to_analyze = [
        ("原始模型", "nested_shells.stl"),
        ("壳体 1 (内层)", "split_result_000.stl"), 
        ("壳体 2 (中层)", "split_result_001.stl"),
        ("壳体 3 (外层)", "split_result_002.stl")
    ]
    
    total_area = 0
    total_points = 0
    total_cells = 0
    
    for name, filename in files_to_analyze:
        if os.path.exists(filename):
            stats = analyze_stl_file(filename)
            print(f"🔍 {name}: {filename}")
            print(f"   📊 点数: {stats['num_points']:,}")
            print(f"   📊 面数: {stats['num_cells']:,}")
            print(f"   📊 表面积: {stats['surface_area']:,.2f}")
            print(f"   🎯 中心点: ({stats['center'][0]:.3f}, {stats['center'][1]:.3f}, {stats['center'][2]:.3f})")
            
            # 计算边界框尺寸
            x_size = stats['bounds'][1] - stats['bounds'][0]
            y_size = stats['bounds'][3] - stats['bounds'][2]
            z_size = stats['bounds'][5] - stats['bounds'][4]
            print(f"   📏 尺寸: X={x_size:.3f}, Y={y_size:.3f}, Z={z_size:.3f}")
            print(f"   📦 边界: [{stats['bounds'][0]:.3f}, {stats['bounds'][1]:.3f}] × "
                  f"[{stats['bounds'][2]:.3f}, {stats['bounds'][3]:.3f}] × "
                  f"[{stats['bounds'][4]:.3f}, {stats['bounds'][5]:.3f}]")
            print()
            
            if filename != "nested_shells.stl":
                total_area += stats['surface_area']
                total_points += stats['num_points']
                total_cells += stats['num_cells']
        else:
            print(f"❌ 文件不存在: {filename}\n")
    
    # 验证拆分完整性
    print("=== 拆分完整性验证 ===")
    if os.path.exists("nested_shells.stl"):
        original_stats = analyze_stl_file("nested_shells.stl")
        print(f"📈 原始模型总表面积: {original_stats['surface_area']:,.2f}")
        print(f"📈 拆分壳体总面积: {total_area:,.2f}")
        
        area_diff = abs(original_stats['surface_area'] - total_area)
        match_rate = (total_area / original_stats['surface_area'] * 100) if original_stats['surface_area'] > 0 else 0
        
        print(f"🔢 面积差异: {area_diff:,.2f}")
        print(f"✅ 匹配率: {match_rate:.1f}%")
        
        if match_rate > 99.0:
            print("🎉 拆分成功！壳体完整性保持良好。")
        elif match_rate > 95.0:
            print("⚠️  拆分基本成功，存在少量精度损失。")
        else:
            print("❌ 拆分可能存在问题，请检查。")
    
    print(f"\n📋 拆分统计:")
    print(f"   拆分出壳体数量: 3")
    print(f"   总点数: {total_points:,}")
    print(f"   总面数: {total_cells:,}")
    print(f"   平均每个壳体点数: {total_points//3:,}")
    print(f"   平均每个壳体面数: {total_cells//3:,}")

if __name__ == "__main__":
    main()