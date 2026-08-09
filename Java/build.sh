#!/bin/bash

# ========================================
#   坦克大战 - Java 打包 JAR
#   功能与 build.bat 完全一致
# ========================================

# 颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m'

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_DIR"

print_header() { echo ""; echo "${CYAN}========================================${NC}"; echo "${CYAN}  $1${NC}"; echo "${CYAN}========================================${NC}"; echo ""; }
print_success() { echo "${GREEN}✅ $1${NC}"; }
print_error() { echo "${RED}❌ $1${NC}"; }
print_info() { echo "${YELLOW}ℹ️  $1${NC}"; }
print_step() { echo "${BLUE}➜ $1${NC}"; }

check_java() {
    if ! command -v java &> /dev/null; then
        print_error "Java 未安装或不在 PATH 中"
        return 1
    fi
    print_success "Java: $(java -version 2>&1 | head -1)"
    return 0
}

check_javac() {
    if ! command -v javac &> /dev/null; then
        print_error "javac 未安装或不在 PATH 中"
        return 1
    fi
    print_success "javac: $(javac -version 2>&1)"
    return 0
}

# ===== 主菜单 =====
build_jar() {
    print_header "打包 JAR"

    check_java || return 1
    check_javac || return 1

    print_step "清理旧文件..."
    [ -d "out" ] && rm -rf out
    [ -f "TankBattle.jar" ] && rm -f TankBattle.jar

    print_step "编译源代码..."
    print_info "依赖库: lib/*.jar"
    echo ""

    javac -d out -encoding UTF-8 -cp "lib/*" \
        src/com/tankbattle/*.java \
        src/com/tankbattle/mod/*.java \
        src/com/tankbattle/resource/*.java \
        src/com/tankbattle/script/*.java

    if [ $? -ne 0 ]; then
        print_error "编译失败！"
        return 1
    fi

    print_step "创建清单文件..."
    cat > MANIFEST.MF << 'EOF'
Manifest-Version: 1.0
Main-Class: com.tankbattle.Main
Class-Path: . lib/json-20240303.jar lib/luaj-jse-3.0.2.jar
EOF

    print_step "打包 JAR..."
    jar cvfm TankBattle.jar MANIFEST.MF -C out . -C src icon.png

    if [ $? -ne 0 ]; then
        print_error "打包失败！"
        return 1
    fi

    rm -f MANIFEST.MF
    rm -rf out

    print_success "打包完成！"
    print_info "输出: TankBattle.jar"
    echo ""
    print_info "文件大小:"
    ls -lh TankBattle.jar | awk '{print "  " $5 "  " $9}'

    echo ""
    print_step "是否现在运行?"
    echo "  1) 是 (运行)"
    echo "  2) 否 (退出)"
    echo ""
    read -p "请输入 [1/2]: " run_choice

    if [ "$run_choice" == "1" ]; then
        echo ""
        print_step "正在运行..."
        java -cp "TankBattle.jar:lib/*" com.tankbattle.Main
    else
        echo ""
        print_info "已退出。"
    fi
}

# ===== 菜单 =====
show_menu() {
    clear
    echo "${CYAN}========================================${NC}"
    echo "${CYAN}      坦克大战 - Java 打包工具          ${NC}"
    echo "${CYAN}========================================${NC}"
    echo ""
    echo "  ${GREEN}1${NC}) 打包 JAR (build)"
    echo "  ${GREEN}2${NC}) 编译运行 (compile)"
    echo "  ${GREEN}3${NC}) 仅编译"
    echo "  ${GREEN}4${NC}) 清理"
    echo "  ${GREEN}5${NC}) 退出"
    echo ""
    echo "${CYAN}========================================${NC}"
    echo ""
}

compile_run() {
    print_header "编译运行"

    check_java || return 1
    check_javac || return 1

    print_step "清理旧文件..."
    [ -d "out" ] && rm -rf out

    print_step "编译源代码..."
    print_info "依赖库: lib/*.jar"
    echo ""

    javac -d out -encoding UTF-8 -cp "lib/*" \
        src/com/tankbattle/*.java \
        src/com/tankbattle/mod/*.java \
        src/com/tankbattle/resource/*.java \
        src/com/tankbattle/script/*.java

    if [ $? -eq 0 ]; then
        print_success "编译完成！"
        echo ""
        print_step "正在运行..."
        echo ""
        java -cp "out:lib/*" com.tankbattle.Main
    else
        print_error "编译失败！"
        return 1
    fi
}

compile_only() {
    print_header "仅编译"

    check_javac || return 1

    print_step "清理旧文件..."
    [ -d "out" ] && rm -rf out

    print_step "编译源代码..."
    print_info "依赖库: lib/*.jar"

    javac -d out -encoding UTF-8 -cp "lib/*" \
        src/com/tankbattle/*.java \
        src/com/tankbattle/mod/*.java \
        src/com/tankbattle/resource/*.java \
        src/com/tankbattle/script/*.java

    if [ $? -eq 0 ]; then
        print_success "编译完成！"
        print_info "输出目录: out/"
    else
        print_error "编译失败！"
        return 1
    fi
}

clean_all() {
    print_header "清理"
    print_step "删除 out/..."
    [ -d "out" ] && rm -rf out
    print_step "删除 TankBattle.jar..."
    [ -f "TankBattle.jar" ] && rm -f TankBattle.jar
    print_step "删除 MANIFEST.MF..."
    [ -f "MANIFEST.MF" ] && rm -f MANIFEST.MF
    print_success "清理完成！"
}

# ===== 主循环 =====
while true; do
    show_menu
    read -p "请选择 [1-5]: " choice
    case $choice in
        1) build_jar; read -p "按回车键继续..." ;;
        2) compile_run; read -p "按回车键继续..." ;;
        3) compile_only; read -p "按回车键继续..." ;;
        4) clean_all; read -p "按回车键继续..." ;;
        5) echo ""; echo "${GREEN}再见！${NC}"; exit 0 ;;
        *) print_error "无效选择"; sleep 1 ;;
    esac
done