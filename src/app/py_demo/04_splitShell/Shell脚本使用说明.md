# Shell脚本使用说明

## 📋 脚本概述

`run_shell_split.sh` 是一个自动化执行Python VTK三维模型壳体拆分的Shell脚本，集成了所有相关的测试、验证和管理功能。

## 🚀 快速开始

### 1. 赋予执行权限
```bash
chmod +x run_shell_split.sh
```

### 2. 执行完整测试（推荐）
```bash
./run_shell_split.sh all
```

## 📖 功能选项

### 完整测试流程
```bash
./run_shell_split.sh all
```
- ✅ 环境检查
- ✅ 清理旧文件
- ✅ 基础功能测试
- ✅ 高级连通类型测试
- ✅ 生成测试报告

### 基础功能测试
```bash
./run_shell_split.sh basic
```
- 清理旧文件
- 运行主程序生成测试数据
- 执行壳体拆分
- 验证拆分结果

### 高级连通类型测试
```bash
./run_shell_split.sh advanced
```
- 创建5种高级测试数据
- 执行批量测试验证
- 分析各种连通类型

### 交互式测试
```bash
./run_shell_split.sh interactive
```
- 列出所有可用STL文件
- 用户选择要拆分的文件
- 自定义输出前缀
- 可选择启动可视化

### 批量处理
```bash
./run_shell_split.sh batch
```
- 批量处理目录中所有STL文件
- 自动生成对应的拆分文件
- 保持原始文件名作为前缀

### 清理旧文件
```bash
./run_shell_split.sh clean
```
- 删除所有生成的STL文件
- 删除测试数据
- 清理临时文件

### 生成报告
```bash
./run_shell_split.sh report
```
- 生成详细的执行报告
- 列出所有生成的文件
- 包含环境信息和时间戳

### 帮助信息
```bash
./run_shell_split.sh help
```

## 🔧 脚本特性

### 环境检查
- 自动检测Python命令（python/python3）
- 验证VTK库安装
- 显示VTK版本信息
- 错误时提供安装提示

### 文件管理
- 智能清理功能
- 文件模式匹配
- 安全删除操作
- 计数统计

### 用户交互
- 彩色输出提示
- 详细的进度信息
- 错误处理和提示
- 交互式选择菜单

### 执行流程
- 严格的错误处理（set -e）
- 模块化函数设计
- 清晰的执行日志
- 完整的状态报告

## 📁 生成的文件

### 测试数据文件
- `nested_shells.stl` - 嵌套球壳测试数据
- `touching_geometries.stl` - 接触型连通测试
- `penetrating_geometries.stl` - 穿透型连通测试
- `nested_complex_geometries.stl` - 嵌套复杂连通测试
- `sparse_distribution_geometries.stl` - 稀疏分布测试
- `mixed_complexity_geometries.stl` - 混合复杂度测试
- `complex_test_model.stl` - 复杂几何体组合

### 拆分结果文件
- `demo_shell_*.stl` - 基础测试拆分结果
- `touch_result_*.stl` - 接触型拆分结果
- `split_*_*.stl` - 各类测试的拆分结果
- `batch_*.stl` - 批量处理结果

### 报告文件
- `shell_split_report.txt` - 自动生成的测试报告

## 🎯 使用场景

### 开发测试
```bash
# 快速验证功能
./run_shell_split.sh basic
```

### 完整验证
```bash
# 全面测试所有功能
./run_shell_split.sh all
```

### 生产使用
```bash
# 处理现有STL文件
./run_shell_split.sh batch
```

### 调试分析
```bash
# 交互式选择测试
./run_shell_split.sh interactive
```

### 清理环境
```bash
# 清理所有生成文件
./run_shell_split.sh clean
```

## ⚠️ 注意事项

1. **权限要求**: 确保脚本有执行权限
2. **Python环境**: 需要安装VTK库
3. **文件路径**: 在包含Python脚本的目录中执行
4. **磁盘空间**: 测试过程会生成较多文件，确保足够空间

## 🔍 故障排除

### VTK未安装
```bash
pip install vtk
```

### 权限问题
```bash
chmod +x run_shell_split.sh
```

### Python命令未找到
脚本会自动检测python3和python命令，确保至少一个可用

### 文件无法生成
检查当前目录是否有写权限

## 📊 输出示例

```
=== Python VTK 三维模型壳体拆分 - 自动化执行脚本 ===
=== 环境检查 ===
[INFO] 使用Python命令: python3
[SUCCESS] VTK版本: 9.5.1
[SUCCESS] 环境检查通过！

=== 基础功能测试 ===
[INFO] 运行主程序生成测试数据...
✅ 已生成嵌套壳体 STL: nested_shells.stl
   包含 3 个独立球壳（半径: [1.0, 2.0, 3.0])
[SUCCESS] 基础功能测试完成！

=== 高级连通类型测试 ===
[INFO] 创建高级测试数据...
🔨 创建接触型连通测试数据...
✅ 已生成接触型连通测试数据: touching_geometries.stl
[SUCCESS] 高级连通类型测试完成！

=== 生成测试报告 ===
[SUCCESS] 报告已生成: shell_split_report.txt

=== 所有测试完成！ ===
```

---

**脚本版本**: 1.0  
**创建时间**: 2025-12-02  
**兼容性**: Linux/macOS/WSL  
**Python要求**: Python 3.6+, VTK 9.0+