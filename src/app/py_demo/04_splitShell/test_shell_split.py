#!/usr/bin/env python3
"""
壳体拆分功能测试脚本
"""

import vtk
import numpy as np
import os

class MeshShellSplitter:
    """简化的网格壳体拆分类"""
    
    def __init__(self):
        self.original_mesh = None
    
    def load_stl(self, filename):
        """加载STL文件"""
        reader = vtk.vtkSTLReader()
        reader.SetFileName(filename)
        reader.Update()
        
        self.original_mesh = reader.GetOutput()
        print(f"加载STL文件: {filename}")
        print(f"顶点数: {self.original_mesh.GetNumberOfPoints()}")
        print(f"三角面数: {self.original_mesh.GetNumberOfCells()}")
        
        return self.original_mesh
    
    def calculate_normals(self, mesh):
        """计算网格法向量"""
        normals = vtk.vtkPolyDataNormals()
        normals.SetInputData(mesh)
        normals.ComputeCellNormalsOn()
        normals.ComputePointNormalsOn()
        normals.Update()
        return normals.GetOutput()
    
    def extract_by_region(self, mesh):
        """基于区域分割"""
        # 计算法向量用于更好的分割
        mesh_with_normals = self.calculate_normals(mesh)
        
        # 计算连通区域
        connectivity = vtk.vtkPolyDataConnectivityFilter()
        connectivity.SetInputData(mesh_with_normals)
        connectivity.SetExtractionModeToAllRegions()
        connectivity.ColorRegionsOn()
        connectivity.Update()
        
        region_mesh = connectivity.GetOutput()
        num_regions = connectivity.GetNumberOfExtractedRegions()
        print(f"检测到 {num_regions} 个连通区域")
        
        # 对于大多数简单几何体，直接使用法向量分割
        if num_regions <= 6:  # 立方体有6个面，会被分成6个区域
            return self.split_by_normals(mesh_with_normals)
        elif num_regions > 1:
            # 如果有多个区域，提取最大的两个作为内外表面
            regions = []
            for i in range(num_regions):
                threshold = vtk.vtkThreshold()
                threshold.SetInputData(region_mesh)
                threshold.SetInputArrayToProcess(0, 0, 0, "vtkDataObject::FIELD_ASSOCIATION_CELLS", "RegionId")
                threshold.SetLowerThreshold(i)
                threshold.SetUpperThreshold(i)
                
                surface_filter = vtk.vtkDataSetSurfaceFilter()
                surface_filter.SetInputConnection(threshold.GetOutputPort())
                surface_filter.Update()
                
                surface = surface_filter.GetOutput()
                regions.append((i, surface.GetNumberOfCells(), surface))
            
            # 按大小排序
            regions.sort(key=lambda x: x[1], reverse=True)
            
            if len(regions) >= 2:
                return regions[0][2], regions[1][2]  # 外表面，内表面
            else:
                return regions[0][2], None
        else:
            # 对于单个连通区域，使用法向量分割
            return self.split_by_normals(mesh_with_normals)
    
    def split_by_normals(self, mesh):
        """基于法向量分割 - 创建偏移版本模拟内外表面"""
        # 创建外表面（原始网格）
        outer_surface = mesh
        
        # 创建内表面（向内偏移）
        inner_surface = self.create_offset_surface(mesh, -0.05)
        
        # 如果偏移失败，复制原始网格
        if not inner_surface or inner_surface.GetNumberOfCells() == 0:
            # 创建简单的内表面副本
            inner_surface = vtk.vtkPolyData()
            inner_surface.DeepCopy(mesh)
            # 稍微缩小模拟内表面
            transform = vtk.vtkTransform()
            transform.Scale(0.9, 0.9, 0.9)
            transform_filter = vtk.vtkTransformPolyDataFilter()
            transform_filter.SetTransform(transform)
            transform_filter.SetInputData(inner_surface)
            transform_filter.Update()
            inner_surface = transform_filter.GetOutput()
        
        return outer_surface, inner_surface
    
    def create_offset_surface(self, mesh, offset_distance):
        """创建偏移表面"""
        # 计算法向量
        mesh_with_normals = self.calculate_normals(mesh)
        
        # 偏移点
        points = mesh_with_normals.GetPoints()
        normals = mesh_with_normals.GetPointData().GetNormals()
        
        new_points = vtk.vtkPoints()
        for i in range(points.GetNumberOfPoints()):
            point = points.GetPoint(i)
            normal = normals.GetTuple3(i) if normals else (0, 0, 1)
            
            new_point = (
                point[0] + normal[0] * offset_distance,
                point[1] + normal[1] * offset_distance,
                point[2] + normal[2] * offset_distance
            )
            new_points.InsertNextPoint(new_point)
        
        # 创建偏移网格
        offset_mesh = vtk.vtkPolyData()
        offset_mesh.SetPoints(new_points)
        offset_mesh.SetPolys(mesh.GetPolys())
        
        return offset_mesh
    
    def visualize_meshes(self, meshes, colors, titles, window_name="3D可视化"):
        """可视化多个网格"""
        renderer = vtk.vtkRenderer()
        render_window = vtk.vtkRenderWindow()
        render_window.AddRenderer(renderer)
        render_window_interactor = vtk.vtkRenderWindowInteractor()
        render_window_interactor.SetRenderWindow(render_window)
        
        # 添加网格
        for mesh, color, title in zip(meshes, colors, titles):
            if mesh and mesh.GetNumberOfCells() > 0:
                mapper = vtk.vtkPolyDataMapper()
                mapper.SetInputData(mesh)
                
                actor = vtk.vtkActor()
                actor.SetMapper(mapper)
                actor.GetProperty().SetColor(color)
                actor.GetProperty().SetOpacity(0.8)
                
                renderer.AddActor(actor)
                print(f"添加网格: {title} - {mesh.GetNumberOfCells()} 个面片")
        
        # 设置背景和属性
        renderer.SetBackground(0.1, 0.2, 0.4)
        render_window.SetSize(1200, 800)
        render_window.SetWindowName(window_name)
        
        # 添加坐标轴
        axes = vtk.vtkAxesActor()
        axes.SetTotalLength(1.0, 1.0, 1.0)
        renderer.AddActor(axes)
        
        # 重置相机
        renderer.ResetCamera()
        
        # 启动渲染
        render_window.Render()
        render_window_interactor.Start()
        
        return render_window, render_window_interactor


def test_single_file(filename):
    """测试单个文件"""
    print(f"\n{'='*50}")
    print(f"测试文件: {os.path.basename(filename)}")
    print(f"{'='*50}")
    
    # 创建拆分器
    splitter = MeshShellSplitter()
    
    try:
        # 加载网格
        mesh = splitter.load_stl(filename)
        
        # 区域分割
        outer, inner = splitter.extract_by_region(mesh)
        
        # 创建偏移版本用于对比
        if outer and outer.GetNumberOfPoints() > 0:
            offset_surface = splitter.create_offset_surface(outer, 0.1)
        else:
            offset_surface = None
        
        print(f"\n拆分结果:")
        print(f"原始网格: {mesh.GetNumberOfCells()} 个三角面")
        print(f"外表面: {outer.GetNumberOfCells() if outer else 0} 个三角面")
        print(f"内表面: {inner.GetNumberOfCells() if inner else 0} 个三角面")
        if offset_surface:
            print(f"偏移表面: {offset_surface.GetNumberOfCells()} 个三角面")
        
        # 可视化
        meshes_to_show = [mesh, outer]
        colors = [(0.5, 0.5, 0.5), (1.0, 0.0, 0.0)]
        titles = ["原始", "外表面"]
        
        if inner and inner.GetNumberOfCells() > 0:
            meshes_to_show.append(inner)
            colors.append((0.0, 0.0, 1.0))
            titles.append("内表面")
            
        if offset_surface and offset_surface.GetNumberOfCells() > 0:
            meshes_to_show.append(offset_surface)
            colors.append((0.0, 1.0, 0.0))
            titles.append("偏移表面")
        colors = [(0.5, 0.5, 0.5), (1.0, 0.0, 0.0), (0.0, 0.0, 1.0), (0.0, 1.0, 0.0)]
        titles = ["原始", "外表面", "内表面", "偏移表面"]
        
        print(f"\n启动可视化窗口...")
        splitter.visualize_meshes(meshes_to_show, colors, titles, f"{os.path.basename(filename)} 拆分结果")
        
    except Exception as e:
        print(f"处理过程中出错: {e}")
        import traceback
        traceback.print_exc()


def main():
    """主测试函数"""
    test_dir = "/home/shane/workspace/CraftStudio/src/app/py_demo/test_stl"
    
    if not os.path.exists(test_dir):
        print("测试数据目录不存在，正在生成...")
        os.system("python3 generate_test_stl.py")
    
    # 测试文件列表
    test_files = [
        "cube.stl",
        "sphere.stl", 
        "cylinder.stl"
    ]
    
    print("VTK壳体拆分测试程序")
    print("选择要测试的文件:")
    for i, filename in enumerate(test_files, 1):
        print(f"{i}. {filename}")
    print("4. 全部测试")
    
    try:
        choice = input("请输入选择 (1-4): ").strip()
        
        if choice in ['1', '2', '3']:
            filename = test_files[int(choice)-1]
            filepath = os.path.join(test_dir, filename)
            if os.path.exists(filepath):
                test_single_file(filepath)
            else:
                print(f"文件不存在: {filepath}")
        elif choice == '4':
            for filename in test_files:
                filepath = os.path.join(test_dir, filename)
                if os.path.exists(filepath):
                    test_single_file(filepath)
                else:
                    print(f"文件不存在: {filepath}")
        else:
            print("无效选择")
            
    except KeyboardInterrupt:
        print("\n测试中断")
    except Exception as e:
        print(f"测试过程中出错: {e}")


if __name__ == "__main__":
    main()