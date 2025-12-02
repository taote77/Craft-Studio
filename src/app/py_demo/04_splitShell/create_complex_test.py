#!/usr/bin/env python3
import vtk
import os

def create_complex_test_data():
    """创建包含多个独立几何体的复杂测试数据"""
    print("🔨 正在创建复杂测试数据...")
    
    append_filter = vtk.vtkAppendPolyData()
    
    # 1. 创建球体
    sphere = vtk.vtkSphereSource()
    sphere.SetRadius(1.5)
    sphere.SetCenter(0, 0, 0)
    sphere.SetThetaResolution(30)
    sphere.SetPhiResolution(30)
    sphere.Update()
    append_filter.AddInputData(sphere.GetOutput())
    print("   ✅ 添加球体 (半径=1.5, 中心=[0,0,0])")
    
    # 2. 创建立方体
    cube = vtk.vtkCubeSource()
    cube.SetXLength(2.0)
    cube.SetYLength(2.0) 
    cube.SetZLength(2.0)
    cube.SetCenter(4, 0, 0)
    cube.Update()
    append_filter.AddInputData(cube.GetOutput())
    print("   ✅ 添加立方体 (尺寸=2×2×2, 中心=[4,0,0])")
    
    # 3. 创建圆锥体
    cone = vtk.vtkConeSource()
    cone.SetRadius(1.0)
    cone.SetHeight(3.0)
    cone.SetCenter(-4, 0, 0)
    cone.SetDirection(0, 0, 1)
    cone.SetResolution(20)
    cone.Update()
    append_filter.AddInputData(cone.GetOutput())
    print("   ✅ 添加圆锥体 (半径=1.0, 高度=3.0, 中心=[-4,0,0])")
    
    # 4. 创建圆环体
    torus = vtk.vtkParametricTorus()
    torus.SetRingRadius(2.0)
    torus.SetCrossSectionRadius(0.5)
    
    torus_source = vtk.vtkParametricFunctionSource()
    torus_source.SetParametricFunction(torus)
    torus_source.SetUResolution(30)
    torus_source.SetVResolution(20)
    torus_source.Update()
    
    # 移动圆环到新位置
    transform = vtk.vtkTransform()
    transform.Translate(0, 4, 0)
    
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetTransform(transform)
    transform_filter.SetInputData(torus_source.GetOutput())
    transform_filter.Update()
    
    append_filter.AddInputData(transform_filter.GetOutput())
    print("   ✅ 添加圆环体 (环半径=2.0, 截面半径=0.5, 中心=[0,4,0])")
    
    # 5. 创建圆柱体
    cylinder = vtk.vtkCylinderSource()
    cylinder.SetRadius(0.8)
    cylinder.SetHeight(4.0)
    cylinder.SetResolution(20)
    cylinder.Update()
    
    # 旋转并移动圆柱体
    cylinder_transform = vtk.vtkTransform()
    cylinder_transform.RotateX(90)
    cylinder_transform.Translate(0, -4, 0)
    
    cylinder_transform_filter = vtk.vtkTransformPolyDataFilter()
    cylinder_transform_filter.SetTransform(cylinder_transform)
    cylinder_transform_filter.SetInputData(cylinder.GetOutput())
    cylinder_transform_filter.Update()
    
    append_filter.AddInputData(cylinder_transform_filter.GetOutput())
    print("   ✅ 添加圆柱体 (半径=0.8, 高度=4.0, 中心=[0,-4,0])")
    
    # 合并所有几何体
    append_filter.Update()
    
    # 保存为STL文件
    writer = vtk.vtkSTLWriter()
    writer.SetFileName("complex_test_model.stl")
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    
    print("✅ 已生成复杂测试模型: complex_test_model.stl")
    print("   包含 5 个独立几何体：球体、立方体、圆锥体、圆环体、圆柱体")
    
    return "complex_test_model.stl"

def test_complex_model(filename):
    """测试复杂模型的拆分"""
    print(f"\n🧪 开始拆分复杂测试模型: {filename}")
    
    # 运行拆分
    os.system(f"python 04_split_obj.py {filename} complex_shell --visualize=false 2>/dev/null")
    
    # 分析结果
    from simple_verify import analyze_stl_file
    
    print("\n📊 拆分结果分析:")
    
    # 检查原始模型
    original_stats = analyze_stl_file(filename)
    print(f"原始模型: {original_stats['num_cells']} 个面, {original_stats['surface_area']:.2f} 面积")
    
    # 查找拆分结果
    split_files = []
    for i in range(10):  # 查找前10个可能的文件
        filename = f"complex_shell_{i:03d}.stl"
        if os.path.exists(filename):
            split_files.append(filename)
    
    total_area = 0
    print(f"\n发现 {len(split_files)} 个拆分壳体:")
    
    for i, file in enumerate(split_files):
        stats = analyze_stl_file(file)
        total_area += stats['surface_area']
        
        # 根据边界框判断可能的几何体类型
        bounds = stats['bounds']
        x_size = bounds[1] - bounds[0]
        y_size = bounds[3] - bounds[2]
        z_size = bounds[5] - bounds[4]
        center = stats['center']
        
        # 简单的几何体判断
        geom_type = "未知"
        if abs(x_size - y_size) < 0.5 and abs(y_size - z_size) < 0.5:
            if abs(center[0]) < 0.5 and abs(center[1]) < 0.5 and abs(center[2]) < 0.5:
                geom_type = "球体"
            else:
                geom_type = "立方体"
        elif x_size > y_size * 1.5:
            geom_type = "圆柱体/圆锥体"
        elif min(x_size, y_size) < max(x_size, y_size) * 0.3:
            geom_type = "圆环体"
        
        print(f"  壳体 {i+1}: {geom_type} - {stats['num_cells']} 面, "
              f"面积={stats['surface_area']:.2f}, 中心=({center[0]:.1f},{center[1]:.1f},{center[2]:.1f})")
    
    # 验证完整性
    match_rate = (total_area / original_stats['surface_area'] * 100) if original_stats['surface_area'] > 0 else 0
    print(f"\n✅ 拆分完整性: {match_rate:.1f}% (总面积匹配)")
    
    if len(split_files) == 5 and match_rate > 99:
        print("🎉 复杂模型拆分测试通过！")
        return True
    else:
        print("⚠️  拆分结果与预期不符")
        return False

def main():
    print("=== Python VTK 三维模型壳体拆分测试 ===\n")
    
    # 创建复杂测试数据
    test_file = create_complex_test_data()
    
    # 测试拆分功能
    success = test_complex_model(test_file)
    
    if success:
        print("\n🌟 所有测试完成！壳体拆分功能工作正常。")
    else:
        print("\n❌ 测试发现问题，请检查拆分算法。")

if __name__ == "__main__":
    main()