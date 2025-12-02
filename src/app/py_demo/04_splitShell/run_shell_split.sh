#!/bin/bash

# =============================================================================
# Python VTK 三维模型壳体拆分 - 自动化执行脚本
# =============================================================================
# 作者: AI Assistant
# 创建时间: 2025-12-02
# 用途: 一键执行所有壳体拆分相关的测试和验证命令
# =============================================================================

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 工作目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo -e "${PURPLE}=== $1 ===${NC}"
}

# 检查Python和VTK环境
check_environment() {
    print_header "环境检查"
    
    if ! command -v python3 &> /dev/null && ! command -v python &> /dev/null; then
        print_error "Python 未安装或不在PATH中"
        exit 1
    fi
    
    PYTHON_CMD="python3"
    if ! command -v python3 &> /dev/null; then
        PYTHON_CMD="python"
    fi
    
    print_info "使用Python命令: $PYTHON_CMD"
    
    # 检查VTK
    if ! $PYTHON_CMD -c "import vtk" &> /dev/null; then
        print_error "VTK库未安装，请先安装: pip install vtk"
        exit 1
    fi
    
    VTK_VERSION=$($PYTHON_CMD -c "import vtk; print(vtk.VTK_VERSION)")
    print_success "VTK版本: $VTK_VERSION"
    print_success "环境检查通过！"
}

# 清理旧文件
cleanup_old_files() {
    print_header "清理旧文件"
    
    # 要清理的文件模式
    patterns=(
        "nested_shells.stl"
        "*shell_*.stl"
        "*split_*.stl"
        "*touch_*.stl"
        "*demo_*.stl"
        "*complex_*.stl"
        "touching_geometries.stl"
        "penetrating_geometries.stl"
        "nested_complex_geometries.stl"
        "sparse_distribution_geometries.stl"
        "mixed_complexity_geometries.stl"
        "complex_test_model.stl"
        "*.png"
    )
    
    local count=0
    for pattern in "${patterns[@]}"; do
        if ls $pattern 1> /dev/null 2>&1; then
            rm -f $pattern
            count=$((count + 1))
            print_info "删除: $pattern"
        fi
    done
    
    print_success "清理完成，删除了 $count 个文件"
}

# 基础功能测试
basic_test() {
    print_header "基础功能测试"
    
    print_info "运行主程序生成测试数据..."
    $PYTHON_CMD 04_split_obj.py
    
    if [ -f "nested_shells.stl" ]; then
        print_success "测试数据生成成功: nested_shells.stl"
        
        print_info "执行壳体拆分..."
        $PYTHON_CMD 04_split_obj.py nested_shells.stl demo_shell
        
        print_info "验证拆分结果..."
        $PYTHON_CMD simple_verify.py
        
        print_success "基础功能测试完成！"
    else
        print_error "测试数据生成失败"
        exit 1
    fi
}

# 高级连通类型测试
advanced_test() {
    print_header "高级连通类型测试"
    
    print_info "创建高级测试数据..."
    $PYTHON_CMD create_advanced_tests.py
    
    print_info "执行批量测试验证..."
    $PYTHON_CMD batch_test_advanced.py
    
    print_success "高级连通类型测试完成！"
}

# 交互式测试
interactive_test() {
    print_header "交互式测试"
    
    echo -e "${CYAN}可用的测试文件:${NC}"
    files=(*.stl)
    for i in "${!files[@]}"; do
        echo "  $((i+1)). ${files[$i]}"
    done
    
    echo -e "${CYAN}请选择要拆分的文件 (输入数字 1-${#files[@]}):${NC}"
    read -r choice
    
    if [[ "$choice" =~ ^[0-9]+$ ]] && [ "$choice" -ge 1 ] && [ "$choice" -le "${#files[@]}" ]; then
        selected_file="${files[$((choice-1))]}"
        print_info "选择的文件: $selected_file"
        
        echo -e "${CYAN}输入输出前缀 (默认: interactive_shell):${NC}"
        read -r prefix
        prefix=${prefix:-interactive_shell}
        
        echo -e "${CYAN}是否启动可视化? (y/n, 默认: n):${NC}"
        read -r visualize
        if [[ "$visualize" =~ ^[Yy]$ ]]; then
            $PYTHON_CMD 04_split_obj.py "$selected_file" "$prefix" --visualize
        else
            $PYTHON_CMD 04_split_obj.py "$selected_file" "$prefix"
        fi
        
        print_success "交互式测试完成！"
    else
        print_error "无效的选择"
        exit 1
    fi
}

# 批量处理所有STL文件
batch_process() {
    print_header "批量处理所有STL文件"
    
    for file in *.stl; do
        if [ -f "$file" ]; then
            print_info "处理文件: $file"
            prefix="batch_${file%.*}"
            $PYTHON_CMD 04_split_obj.py "$file" "$prefix"
            print_success "完成: $file → ${prefix}_*.stl"
        fi
    done
    
    print_success "批量处理完成！"
}

# 生成报告
generate_report() {
    print_header "生成测试报告"
    
    report_file="shell_split_report.txt"
    
    cat > "$report_file" << EOF
Python VTK 三维模型壳体拆分 - 执行报告
生成时间: $(date)
环境信息:
- Python: $PYTHON_CMD
- VTK版本: $VTK_VERSION
- 工作目录: $(pwd)

生成的文件列表:
EOF
    
    find . -maxdepth 1 -name "*.stl" -exec ls -lh {} \; >> "$report_file"
    echo "" >> "$report_file"
    
    echo "Python脚本文件:" >> "$report_file"
    find . -maxdepth 1 -name "*.py" -exec ls -lh {} \; >> "$report_file"
    
    print_success "报告已生成: $report_file"
}

# 显示帮助信息
show_help() {
    cat << EOF
Python VTK 三维模型壳体拆分 - 自动化脚本

用法: $0 [选项]

选项:
  all           执行完整的测试流程 (推荐)
  basic         仅执行基础功能测试
  advanced      仅执行高级连通类型测试
  interactive   交互式测试模式
  batch         批量处理所有STL文件
  clean         仅清理旧文件
  report        生成测试报告
  help          显示此帮助信息

示例:
  $0 all        # 执行完整测试
  $0 basic      # 基础测试
  $0 interactive # 交互模式

EOF
}

# 主函数
main() {
    print_header "Python VTK 三维模型壳体拆分 - 自动化执行脚本"
    
    # 检查环境
    check_environment
    
    # 根据参数执行相应功能
    case "${1:-all}" in
        "all")
            cleanup_old_files
            basic_test
            advanced_test
            generate_report
            print_header "所有测试完成！"
            ;;
        "basic")
            cleanup_old_files
            basic_test
            ;;
        "advanced")
            advanced_test
            ;;
        "interactive")
            interactive_test
            ;;
        "batch")
            batch_process
            ;;
        "clean")
            cleanup_old_files
            ;;
        "report")
            generate_report
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
}

# 脚本入口点
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi