import vtk
import sys
import os

def read_mesh(filename):
    """根据文件扩展名自动选择读取器"""
    _, ext = os.path.splitext(filename.lower())
    if ext == ".stl":
        reader = vtk.vtkSTLReader()
    elif ext in (".vtk", ".vtp"):
        reader = vtk.vtkPolyDataReader() if ext == ".vtk" else vtk.vtkXMLPolyDataReader()
    else:
        raise ValueError(f"Unsupported file format: {ext}")
    
    reader.SetFileName(filename)
    reader.Update()
    polydata = reader.GetOutput()
    if polydata.GetNumberOfPoints() == 0:
        raise IOError(f"Failed to read mesh from {filename}")
    return polydata

def clean_mesh(polydata):
    """清理网格：合并重复点，修复拓扑"""
    cleaner = vtk.vtkCleanPolyData()
    cleaner.SetInputData(polydata)
    cleaner.PointMergingOn()
    cleaner.ConvertLinesToPointsOff()
    cleaner.ConvertPolysToLinesOff()
    cleaner.ConvertStripsToPolysOff()
    cleaner.Update()
    return cleaner.GetOutput()

def extract_all_shells(polydata):
    """提取所有连通壳体，返回壳体列表"""
    connectivity = vtk.vtkPolyDataConnectivityFilter()
    connectivity.SetInputData(polydata)
    connectivity.SetExtractionModeToAllRegions()
    connectivity.ColorRegionsOn()  # 为不同区域分配标量值
    connectivity.Update()

    num_regions = connectivity.GetNumberOfExtractedRegions()
    print(f"检测到 {num_regions} 个壳体")

    shells = []
    for i in range(num_regions):
        extractor = vtk.vtkPolyDataConnectivityFilter()
        extractor.SetInputData(polydata)
        extractor.SetExtractionModeToSpecifiedRegions()
        extractor.AddSpecifiedRegion(i)
        extractor.Update()
        shells.append(vtk.vtkPolyData())
        shells[-1].DeepCopy(extractor.GetOutput())
    return shells

def save_shell(shell, output_path):
    """保存单个壳体为 STL 文件"""
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(output_path)
    writer.SetInputData(shell)
    writer.SetFileTypeToASCII()  # 或 SetFileTypeToBinary() 以减小体积
    writer.Write()

def visualize_shells(shells):
    """可视化所有壳体（不同颜色）"""
    renderer = vtk.vtkRenderer()
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer)
    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)

    num = len(shells)
    for i, shell in enumerate(shells):
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(shell)
        mapper.ScalarVisibilityOff()

        actor = vtk.vtkActor()
        actor.SetMapper(mapper)
        
        # 设置不同颜色（HSV 色环）
        hue = i / max(1, num - 1) if num > 1 else 0.0
        rgb = [0, 0, 0]
        vtk.vtkMath.HSVToRGB(hue, 1.0, 1.0, rgb[0], rgb[1], rgb[2])
        actor.GetProperty().SetColor(rgb)

        renderer.AddActor(actor)

    renderer.SetBackground(0.1, 0.2, 0.3)
    render_window.Render()
    interactor.Start()

def main(input_file, output_prefix="shell", visualize=False):
    print(f"正在处理: {input_file}")
    
    # 1. 读取网格
    mesh = read_mesh(input_file)
    
    # 2. 清理网格
    cleaned = clean_mesh(mesh)
    
    # 3. 提取所有壳体
    shells = extract_all_shells(cleaned)
    
    # 4. 保存每个壳体
    base_dir = os.path.dirname(input_file) or "."
    for i, shell in enumerate(shells):
        out_name = f"{output_prefix}_{i:03d}.stl"
        out_path = os.path.join(base_dir, out_name)
        save_shell(shell, out_path)
        print(f"已保存: {out_path}")
    
    # 5. 可视化（可选）
    if visualize:
        print("启动可视化窗口...")
        visualize_shells(shells)


import vtk

def create_concentric_spheres(output_file, radii=[1.0, 2.0, 3.0]):
    append_filter = vtk.vtkAppendPolyData()
    
    for r in radii:
        sphere = vtk.vtkSphereSource()
        sphere.SetRadius(r)
        sphere.SetThetaResolution(32)   # 控制球面精度
        sphere.SetPhiResolution(32)
        sphere.Update()
        append_filter.AddInputData(sphere.GetOutput())
    
    append_filter.Update()
    
    writer = vtk.vtkSTLWriter()
    writer.SetFileName(output_file)
    writer.SetInputData(append_filter.GetOutput())
    writer.SetFileTypeToASCII()
    writer.Write()
    print(f"✅ 已生成嵌套壳体 STL: {output_file}")
    print(f"   包含 {len(radii)} 个独立球壳（半径: {radii})")


if __name__ == "__main__":

    create_concentric_spheres("nested_shells.stl", radii=[1.0, 2.0, 3.0])


    if len(sys.argv) < 2:
        print("用法: python shell_extractor.py <input.stl> [output_prefix] [--visualize]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_prefix = sys.argv[2] if len(sys.argv) > 2 else "shell"
    visualize = "--visualize" in sys.argv

    main(input_file, output_prefix, visualize)