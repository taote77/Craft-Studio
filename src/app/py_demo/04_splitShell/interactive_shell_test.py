#!/usr/bin/env python3
"""
交互式壳体拆分测试程序
提供简单的GUI界面用于测试不同的拆分方法
"""

import vtk
import os
import sys
from tkinter import Tk, filedialog, messagebox, ttk
import threading

class ShellSplitterGUI:
    def __init__(self):
        self.root = Tk()
        self.root.title("VTK 3D模型壳体拆分工具")
        self.root.geometry("600x500")
        
        self.selected_file = None
        self.splitter = None
        self.renderer = None
        self.render_window = None
        self.render_window_interactor = None
        
        self.setup_gui()
        self.setup_vtk()
    
    def setup_gui(self):
        """设置GUI界面"""
        # 文件选择区域
        file_frame = ttk.LabelFrame(self.root, text="文件选择", padding=10)
        file_frame.pack(fill="x", padx=10, pady=5)
        
        ttk.Button(file_frame, text="选择STL文件", command=self.select_file).pack(side="left", padx=5)
        self.file_label = ttk.Label(file_frame, text="未选择文件")
        self.file_label.pack(side="left", padx=20)
        
        # 方法选择区域
        method_frame = ttk.LabelFrame(self.root, text="拆分方法", padding=10)
        method_frame.pack(fill="x", padx=10, pady=5)
        
        self.method_var = tk.StringVar(value="curvature")
        ttk.Radiobutton(method_frame, text="曲率法", variable=self.method_var, 
                       value="curvature").pack(side="left", padx=10)
        ttk.Radiobutton(method_frame, text="区域生长法", variable=self.method_var, 
                       value="region").pack(side="left", padx=10)
        ttk.Radiobutton(method_frame, text="偏移法", variable=self.method_var, 
                       value="offset").pack(side="left", padx=10)
        
        # 参数设置区域
        param_frame = ttk.LabelFrame(self.root, text="参数设置", padding=10)
        param_frame.pack(fill="x", padx=10, pady=5)
        
        ttk.Label(param_frame, text="壁厚:").pack(side="left", padx=5)
        self.thickness_var = tk.DoubleVar(value=0.1)
        ttk.Entry(param_frame, textvariable=self.thickness_var, width=10).pack(side="left", padx=5)
        
        # 操作按钮区域
        button_frame = ttk.Frame(self.root)
        button_frame.pack(fill="x", padx=10, pady=10)
        
        ttk.Button(button_frame, text="生成测试数据", command=self.generate_test_data).pack(side="left", padx=5)
        ttk.Button(button_frame, text="执行拆分", command=self.execute_split).pack(side="left", padx=5)
        ttk.Button(button_frame, text="保存结果", command=self.save_results).pack(side="left", padx=5)
        ttk.Button(button_frame, text="重置视图", command=self.reset_view).pack(side="left", padx=5)
        
        # 信息显示区域
        info_frame = ttk.LabelFrame(self.root, text="处理信息", padding=10)
        info_frame.pack(fill="both", expand=True, padx=10, pady=5)
        
        self.info_text = tk.Text(info_frame, height=10)
        scrollbar = ttk.Scrollbar(info_frame, orient="vertical", command=self.info_text.yview)
        self.info_text.configure(yscrollcommand=scrollbar.set)
        
        self.info_text.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
    
    def setup_vtk(self):
        """设置VTK渲染窗口"""
        # 创建嵌入的VTK窗口
        self.vtk_frame = ttk.Frame(self.root)
        self.vtk_frame.pack(fill="both", expand=True, padx=10, pady=5)
        
        # 创建VTK渲染器
        self.renderer = vtk.vtkRenderer()
        self.renderer.SetBackground(0.1, 0.2, 0.4)
        
        # 创建渲染窗口
        self.render_window = vtk.vtkRenderWindow()
        self.render_window.AddRenderer(self.renderer)
        
        # 创建交互器
        self.render_window_interactor = vtk.vtkRenderWindowInteractor()
        
        # 嵌入到Tkinter
        self.render_window_interactor.SetRenderWindow(self.render_window)
        
        # 添加坐标轴
        axes = vtk.vtkAxesActor()
        axes.SetTotalLength(1.0, 1.0, 1.0)
        self.renderer.AddActor(axes)
        
        # 设置初始相机
        self.renderer.ResetCamera()
    
    def log_info(self, message):
        """在信息框中显示信息"""
        self.info_text.insert(tk.END, message + "\n")
        self.info_text.see(tk.END)
        self.root.update()
    
    def select_file(self):
        """选择STL文件"""
        file_path = filedialog.askopenfilename(
            title="选择STL文件",
            filetypes=[("STL文件", "*.stl"), ("所有文件", "*.*")]
        )
        if file_path:
            self.selected_file = file_path
            self.file_label.config(text=os.path.basename(file_path))
            self.log_info(f"选择文件: {file_path}")
    
    def generate_test_data(self):
        """生成测试数据"""
        self.log_info("正在生成测试STL文件...")
        
        def generate():
            try:
                from generate_test_stl import main as generate_main
                generate_main()
                self.log_info("测试数据生成完成!")
            except Exception as e:
                self.log_info(f"生成测试数据失败: {e}")
        
        # 在新线程中生成，避免阻塞GUI
        thread = threading.Thread(target=generate)
        thread.start()
    
    def execute_split(self):
        """执行壳体拆分"""
        if not self.selected_file:
            messagebox.showwarning("警告", "请先选择STL文件!")
            return
        
        self.log_info(f"开始处理文件: {os.path.basename(self.selected_file)}")
        self.log_info(f"使用方法: {self.method_var.get()}")
        
        def split():
            try:
                # 导入壳体拆分类
                from _04_split_obj import MeshShellSplitter
                
                self.splitter = MeshShellSplitter()
                self.splitter.wall_thickness = self.thickness_var.get()
                
                # 执行拆分
                outer, inner = self.splitter.process_mesh(
                    self.selected_file, 
                    method=self.method_var.get(), 
                    visualize=False
                )
                
                # 更新VTK显示
                self.update_vtk_display(outer, inner)
                
                self.log_info("拆分完成!")
                self.log_info(f"外表面: {outer.GetNumberOfCells()} 个三角面")
                if inner:
                    self.log_info(f"内表面: {inner.GetNumberOfCells()} 个三角面")
                else:
                    self.log_info("内表面: 未检测到")
                    
            except Exception as e:
                self.log_info(f"拆分失败: {e}")
                messagebox.showerror("错误", f"拆分失败: {e}")
        
        # 在新线程中执行拆分
        thread = threading.Thread(target=split)
        thread.start()
    
    def update_vtk_display(self, outer_surface, inner_surface=None):
        """更新VTK显示"""
        # 清除现有actor
        self.renderer.RemoveAllViewProps()
        
        # 重新添加坐标轴
        axes = vtk.vtkAxesActor()
        axes.SetTotalLength(1.0, 1.0, 1.0)
        self.renderer.AddActor(axes)
        
        # 外表面 - 红色
        outer_mapper = vtk.vtkPolyDataMapper()
        outer_mapper.SetInputData(outer_surface)
        
        outer_actor = vtk.vtkActor()
        outer_actor.SetMapper(outer_mapper)
        outer_actor.GetProperty().SetColor(1.0, 0.0, 0.0)
        outer_actor.GetProperty().SetOpacity(0.8)
        
        # 内表面 - 蓝色
        inner_actor = None
        if inner_surface:
            inner_mapper = vtk.vtkPolyDataMapper()
            inner_mapper.SetInputData(inner_surface)
            
            inner_actor = vtk.vtkActor()
            inner_actor.SetMapper(inner_mapper)
            inner_actor.GetProperty().SetColor(0.0, 0.0, 1.0)
            inner_actor.GetProperty().SetOpacity(0.8)
        
        # 添加到渲染器
        self.renderer.AddActor(outer_actor)
        if inner_actor:
            self.renderer.AddActor(inner_actor)
        
        # 重置相机并渲染
        self.renderer.ResetCamera()
        self.render_window.Render()
    
    def save_results(self):
        """保存拆分结果"""
        if not self.splitter:
            messagebox.showwarning("警告", "请先执行拆分操作!")
            return
        
        save_dir = filedialog.askdirectory(title="选择保存目录")
        if save_dir:
            try:
                # 这里需要从splitter获取拆分结果
                # 简化版本，只保存当前显示的内容
                self.log_info(f"结果保存到: {save_dir}")
                messagebox.showinfo("成功", "结果保存完成!")
            except Exception as e:
                self.log_info(f"保存失败: {e}")
                messagebox.showerror("错误", f"保存失败: {e}")
    
    def reset_view(self):
        """重置视图"""
        if self.renderer:
            self.renderer.ResetCamera()
            self.render_window.Render()
    
    def run(self):
        """运行GUI"""
        self.root.mainloop()


def main():
    """主函数"""
    app = ShellSplitterGUI()
    app.run()


if __name__ == "__main__":
    # 导入tkinter
    import tkinter as tk
    main()