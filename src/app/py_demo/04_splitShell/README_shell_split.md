# VTK三维模型壳体拆分工具

本工具集基于Python VTK实现了三维模型的壳体拆分功能，能够提取内外表面并可视化显示。

## 文件说明

### 核心程序

1. **generate_test_stl.py** - STL测试数据生成器
   - 生成各种几何形状的STL文件用于测试
   - 包括：球体、立方体、圆柱体、环面、复杂组合形状

2. **04_split_obj.py** - 壳体拆分主程序
   - 实现多种壳体拆分算法
   - 支持曲率法、区域生长法、偏移法
   - 提供完整的可视化功能

3. **test_shell_split.py** - 简化测试程序
   - 简化版壳体拆分测试工具
   - 交互式选择测试文件
   - 实时可视化结果

4. **interactive_shell_test.py** - GUI交互程序
   - 基于Tkinter的图形界面
   - 提供文件选择、参数设置等功能
   - 集成VTK可视化窗口

## 功能特性

### 壳体拆分算法

1. **曲率法** - 基于表面曲率特征分割
   - 计算平均曲率
   - 根据曲率正负区分内外表面
   - 适用于复杂几何体

2. **区域生长法** - 基于连通区域分割
   - 检测连通区域
   - 按区域大小提取主要表面
   - 适用于分离明显的壳体结构

3. **偏移法** - 基于表面偏移
   - 创建内外表面偏移
   - 可控制壁厚参数
   - 适用于壳体厚度分析

### 可视化功能

- 多网格叠加显示
- 颜色编码区分不同表面
- 交互式3D操作（旋转、缩放、平移）
- 坐标轴显示
- 透明度控制

## 使用方法

### 1. 生成测试数据

```bash
cd /home/shane/workspace/CraftStudio/src/app/py_demo
python3 generate_test_stl.py
```

生成的测试文件位于 `test_stl/` 目录：
- sphere.stl - 球体
- cube.stl - 立方体  
- cylinder.stl - 圆柱体
- torus.stl - 环面
- complex.stl - 复杂组合形状

### 2. 运行测试程序

#### 方法一：交互式测试
```bash
python3 test_shell_split.py
```
程序会显示菜单，选择要测试的文件。

#### 方法二：GUI界面
```bash
python3 interactive_shell_test.py
```
启动图形界面，提供完整的文件操作和参数设置。

#### 方法三：完整功能
```bash
python3 04_split_obj.py
```
运行完整的壳体拆分程序，自动处理所有测试文件。

## 程序输出

### 拆分结果
- 原始网格：灰色半透明显示
- 外表面：红色显示
- 内表面：蓝色显示
- 偏移表面：绿色显示

### 文件输出
拆分结果保存在 `test_stl/split_results/` 目录：
- `{原文件名}_outer_{方法}.stl` - 外表面
- `{原文件名}_inner_{方法}.stl` - 内表面

### 统计信息
程序会输出详细的处理统计：
```
原始网格: 12 个三角面
外表面: 12 个三角面  
内表面: 12 个三角面
```

## 技术实现

### 核心类

#### MeshShellSplitter
主要壳体拆分类，包含以下方法：

- `load_stl()` - 加载STL文件
- `calculate_normals()` - 计算网格法向量
- `extract_surface_features()` - 基于曲率提取表面特征
- `split_shell_by_offset()` - 偏移法拆分
- `split_shell_by_region_growing()` - 区域生长法拆分
- `visualize_split_result()` - 可视化拆分结果

### VTK组件使用

- `vtkSTLReader` - 读取STL文件
- `vtkPolyDataNormals` - 计算法向量
- `vtkCurvatures` - 计算曲率
- `vtkThreshold` - 阈值分割
- `vtkPolyDataConnectivityFilter` - 连通区域分析
- `vtkTransformPolyDataFilter` - 几何变换
- `vtkRenderWindow` - 渲染窗口
- `vtkRenderer` - 渲染器

## 依赖要求

- Python 3.6+
- VTK 8.0+
- NumPy
- Tkinter (GUI程序需要)

## 扩展功能

### 自定义参数
可以通过修改类属性调整算法参数：
```python
splitter = MeshShellSplitter()
splitter.wall_thickness = 0.2  # 调整壁厚
```

### 批处理
可以扩展程序支持批量处理多个文件：
```python
file_list = ["model1.stl", "model2.stl"]
for file in file_list:
    splitter.process_mesh(file, method="curvature")
```

## 注意事项

1. **文件格式** - 目前仅支持STL格式
2. **内存使用** - 大型模型可能需要较多内存
3. **可视化窗口** - VTK渲染窗口可能需要图形环境支持
4. **算法选择** - 不同几何体适用不同的拆分算法

## 示例结果

### 立方体拆分
- 原始：12个三角面
- 外表面：12个三角面
- 内表面：12个三角面（缩放版本）

### 球体拆分
- 原始：2000+个三角面
- 外表面：半数三角面（正曲率）
- 内表面：半数三角面（负曲率）

程序提供了完整的3D交互功能，可以通过鼠标操作旋转、缩放视图，详细观察拆分效果。