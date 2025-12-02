#!/usr/bin/env python3
import vtk
import os
import subprocess
import sys

def analyze_stl_file(filename):
    """分析STL文件的几何信息"""
    if not os.path.exists(filename):
        return None
        
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
        'filename': filename,
        'num_points': num_points,
        'num_cells': num_cells,
        'bounds': bounds,
        'center': center,
        'surface_area': surface_area
    }

def split_model(input_file, output_prefix):
    """调用拆分程序"""
    print(f"🔄 正在拆分: {input_file}")
    
    try:
        # 运行拆分程序
        result = subprocess.run([
            sys.executable, "04_split_obj.py", input_file, output_prefix
        ], capture_output=True, text=True, cwd=os.getcwd())
        
        if result.returncode == 0:
            print(f"   ✅ 拆分成功")
            # 查找生成的拆分文件
            split_files = []
            for i in range(20):  # 查找前20个可能的文件
                filename = f"{output_prefix}_{i:03d}.stl"
                if os.path.exists(filename):
                    split_files.append(filename)
            return split_files
        else:
            print(f"   ❌ 拆分失败: {result.stderr}")
            return []
            
    except Exception as e:
        print(f"   ❌ 拆分异常: {e}")
        return []

def test_connectivity_type(test_name, input_file):
    """测试特定连通类型的拆分效果"""
    print(f"\n{'='*60}")
    print(f"🧪 测试 {test_name}")
    print(f"{'='*60}")
    
    # 分析原始模型
    original_stats = analyze_stl_file(input_file)
    if original_stats is None:
        print(f"❌ 无法读取文件: {input_file}")
        return False
    
    print(f"📊 原始模型分析:")
    print(f"   文件: {input_file}")
    print(f"   点数: {original_stats['num_points']:,}")
    print(f"   面数: {original_stats['num_cells']:,}")
    print(f"   表面积: {original_stats['surface_area']:.2f}")
    print(f"   中心点: ({original_stats['center'][0]:.3f}, {original_stats['center'][1]:.3f}, {original_stats['center'][2]:.3f})")
    
    # 计算模型尺寸
    x_size = original_stats['bounds'][1] - original_stats['bounds'][0]
    y_size = original_stats['bounds'][3] - original_stats['bounds'][2]
    z_size = original_stats['bounds'][5] - original_stats['bounds'][4]
    print(f"   尺寸: {x_size:.2f} × {y_size:.2f} × {z_size:.2f}")
    
    # 拆分模型
    output_prefix = f"split_{test_name.replace(' ', '_').lower()}"
    split_files = split_model(input_file, output_prefix)
    
    if not split_files:
        return False
    
    # 分析拆分结果
    print(f"\n📊 拆分结果分析:")
    print(f"   检测到 {len(split_files)} 个壳体")
    
    total_area = 0
    total_points = 0
    total_cells = 0
    
    for i, split_file in enumerate(split_files):
        stats = analyze_stl_file(split_file)
        if stats:
            total_area += stats['surface_area']
            total_points += stats['num_points']
            total_cells += stats['num_cells']
            
            # 计算壳体尺寸
            x_size = stats['bounds'][1] - stats['bounds'][0]
            y_size = stats['bounds'][3] - stats['bounds'][2]
            z_size = stats['bounds'][5] - stats['bounds'][4]
            
            print(f"   壳体 {i+1}: {stats['num_cells']} 面, {stats['surface_area']:.2f} 面积")
            print(f"            尺寸: {x_size:.2f} × {y_size:.2f} × {z_size:.2f}")
            print(f"            中心: ({stats['center'][0]:.3f}, {stats['center'][1]:.3f}, {stats['center'][2]:.3f})")
    
    # 验证完整性
    print(f"\n✅ 完整性验证:")
    area_diff = abs(original_stats['surface_area'] - total_area)
    match_rate = (total_area / original_stats['surface_area'] * 100) if original_stats['surface_area'] > 0 else 0
    
    print(f"   原始面积: {original_stats['surface_area']:.2f}")
    print(f"   拆分总面积: {total_area:.2f}")
    print(f"   面积差异: {area_diff:.2f}")
    print(f"   匹配率: {match_rate:.1f}%")
    
    # 判断测试结果
    success = match_rate >= 99.0
    if success:
        print(f"   🎉 {test_name} 测试通过！")
    else:
        print(f"   ⚠️  {test_name} 测试结果需要检查")
    
    return success

def classify_geometry_by_bounds(stats):
    """根据边界框信息分类几何体类型"""
    x_size = stats['bounds'][1] - stats['bounds'][0]
    y_size = stats['bounds'][3] - stats['bounds'][2]
    z_size = stats['bounds'][5] - stats['bounds'][4]
    center = stats['center']
    
    # 避免除零错误
    min_size = min(x_size, y_size, z_size)
    if min_size < 1e-10:
        return "退化几何体"
    
    # 简单的几何体分类启发式规则
    size_ratio = max(x_size, y_size, z_size) / min_size
    
    if size_ratio < 1.3:  # 接近正方体
        if abs(center[0]) < 0.5 and abs(center[1]) < 0.5 and abs(center[2]) < 0.5:
            return "中心立方体"
        else:
            return "立方体"
    elif size_ratio < 2.0:  # 接近球形
        return "类球体"
    elif x_size < y_size * 0.3 or y_size < x_size * 0.3:  # 某一维度很小
        return "平板/圆盘"
    else:
        return "复杂几何体"

def analyze_spatial_distribution(shells):
    """分析壳体的空间分布特征"""
    if len(shells) < 2:
        return "单一壳体"
    
    centers = [s['center'] for s in shells]
    
    # 计算壳体间距离
    min_distance = float('inf')
    max_distance = 0
    
    for i in range(len(centers)):
        for j in range(i+1, len(centers)):
            dx = centers[i][0] - centers[j][0]
            dy = centers[i][1] - centers[j][1]
            dz = centers[i][2] - centers[j][2]
            distance = (dx*dx + dy*dy + dz*dz) ** 0.5
            
            min_distance = min(min_distance, distance)
            max_distance = max(max_distance, distance)
    
    if min_distance < 0.1:  # 非常接近
        if max_distance < 5.0:  # 整体距离不大
            return "接触/嵌套分布"
        else:
            return "局部接触分布"
    elif min_distance < 2.0:
        return "紧密分布"
    elif max_distance > 10.0:
        return "稀疏分布"
    else:
        return "中等分布"

def detailed_analysis(test_name, input_file, output_prefix):
    """对特定测试进行详细分析"""
    print(f"\n🔬 {test_name} 详细分析:")
    
    # 分析原始模型
    original_stats = analyze_stl_file(input_file)
    if original_stats is None:
        return
    
    # 查找拆分文件
    split_files = []
    for i in range(20):
        filename = f"{output_prefix}_{i:03d}.stl"
        if os.path.exists(filename):
            split_files.append(filename)
    
    if not split_files:
        return
    
    # 分析每个壳体
    shell_stats = []
    for i, split_file in enumerate(split_files):
        stats = analyze_stl_file(split_file)
        if stats:
            stats['geometry_type'] = classify_geometry_by_bounds(stats)
            shell_stats.append(stats)
    
    # 分析空间分布
    distribution_type = analyze_spatial_distribution(shell_stats)
    print(f"   空间分布类型: {distribution_type}")
    
    # 分析几何体组合
    geometry_types = [s['geometry_type'] for s in shell_stats]
    type_counts = {}
    for geom_type in geometry_types:
        type_counts[geom_type] = type_counts.get(geom_type, 0) + 1
    
    print(f"   几何体组合:")
    for geom_type, count in type_counts.items():
        print(f"     {geom_type}: {count}个")

def main():
    print("=== 高级连通类型批量测试验证 ===")
    
    # 测试文件列表
    test_cases = [
        ("接触型连通", "touching_geometries.stl"),
        ("穿透型连通", "penetrating_geometries.stl"),
        ("嵌套复杂连通", "nested_complex_geometries.stl"),
        ("稀疏分布", "sparse_distribution_geometries.stl"),
        ("混合复杂度", "mixed_complexity_geometries.stl"),
    ]
    
    results = []
    
    # 执行所有测试
    for test_name, test_file in test_cases:
        output_prefix = f"split_{test_name.replace(' ', '_').lower()}"
        success = test_connectivity_type(test_name, test_file)
        results.append((test_name, success))
        
        # 进行详细分析
        detailed_analysis(test_name, test_file, output_prefix)
    
    # 汇总结果
    print(f"\n{'='*60}")
    print("📋 测试结果汇总")
    print(f"{'='*60}")
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "✅ 通过" if success else "❌ 失败"
        print(f"{test_name:<20} : {status}")
        if success:
            passed += 1
    
    print(f"\n总计: {passed}/{total} 测试通过")
    
    if passed == total:
        print("🎉 所有高级连通类型测试均通过！")
        print("🚀 壳体拆分算法在各种复杂情况下表现优异！")
    else:
        print("⚠️  部分测试未通过，需要进一步优化算法。")
    
    return passed == total

if __name__ == "__main__":
    main()