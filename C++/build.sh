#!/bin/bash

# ========================================
#   坦克大战 - C++ 编译脚本
#   支持 MinGW 动态编译 + CMake 编译
#   自动复制所有 DLL
#   打包成 ZIP（用于 GitHub Release）
#   打包后自动清理 EXE 和 DLL
# ========================================

export PATH="/ucrt64/bin:/mingw64/bin:/usr/bin:$PATH"

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
print_warning() { echo "${MAGENTA}⚠️  $1${NC}"; }

check_command() {
    command -v $1 &> /dev/null && return 0
    [ -f "/ucrt64/bin/$1" ] && return 0
    [ -f "/mingw64/bin/$1" ] && return 0
    return 1
}

fix_sfml30_code() {
    print_step "检测 SFML 版本..."
    SFML_VERSION=""
    [ -f "/ucrt64/include/SFML/Config.hpp" ] && SFML_VERSION=$(grep -E "SFML_VERSION_MAJOR" /ucrt64/include/SFML/Config.hpp 2>/dev/null | head -1 | grep -oE "[0-9]+")
    if [ "$SFML_VERSION" == "3" ]; then
        print_info "检测到 SFML 3.x，自动修复..."
        cp src/Tank.cpp src/Tank.cpp.bak 2>/dev/null
        cp src/Wall.cpp src/Wall.cpp.bak 2>/dev/null
        cp src/Game.cpp src/Game.cpp.bak 2>/dev/null
        sed -i 's/.*setRadius.*/\/\/ &/g' src/Tank.cpp 2>/dev/null
        sed -i 's/.*setRadius.*/\/\/ &/g' src/Wall.cpp 2>/dev/null
        sed -i 's/.*setRadius.*/\/\/ &/g' src/Game.cpp 2>/dev/null
        print_success "已修复"
    else
        print_info "SFML 2.x，无需修复"
    fi
}

restore_backup() {
    [ -f "src/Tank.cpp.bak" ] && mv src/Tank.cpp.bak src/Tank.cpp
    [ -f "src/Wall.cpp.bak" ] && mv src/Wall.cpp.bak src/Wall.cpp
    [ -f "src/Game.cpp.bak" ] && mv src/Game.cpp.bak src/Game.cpp
}

check_sfml() {
    print_step "检查 SFML..."
    if [ -f "/ucrt64/include/SFML/Graphics.hpp" ] || [ -f "/mingw64/include/SFML/Graphics.hpp" ]; then
        print_success "SFML 已安装"
        return 0
    fi
    print_error "SFML 未找到！"
    print_info "请安装: pacman -S mingw-w64-ucrt-x86_64-sfml"
    return 1
}

copy_all_dlls() {
    print_step "复制 SFML 3.0 DLL..."
    
    DLL_SRC=""
    if [ -d "/ucrt64/bin" ]; then
        DLL_SRC="/ucrt64/bin"
    elif [ -d "/mingw64/bin" ]; then
        DLL_SRC="/mingw64/bin"
    else
        print_error "找不到 DLL 目录！"
        return 1
    fi
    
    print_info "DLL 源目录: $DLL_SRC"
    
    # SFML 3.0 核心 DLL
    SFML_CORE_DLLS=(
        "libsfml-graphics-3.dll"
        "libsfml-window-3.dll"
        "libsfml-system-3.dll"
        "libsfml-audio-3.dll"
        "libsfml-network-3.dll"
    )
    
    COPIED_CORE=0
    for dll in "${SFML_CORE_DLLS[@]}"; do
        if [ -f "$DLL_SRC/$dll" ]; then
            if [ ! -f "$PROJECT_DIR/$dll" ]; then
                cp "$DLL_SRC/$dll" "$PROJECT_DIR/"
                print_success "已复制 $dll"
                COPIED_CORE=$((COPIED_CORE + 1))
            else
                print_info "$dll 已存在，跳过"
            fi
        else
            print_warning "$dll 不存在"
        fi
    done
    
    if [ $COPIED_CORE -gt 0 ]; then
        print_success "共复制 $COPIED_CORE 个 SFML 核心 DLL"
    else
        print_warning "未复制任何 SFML 核心 DLL！"
    fi
    
    # 依赖库（SFML 依赖 + MinGW 运行时）
    DEP_DLLS=(
        "libfreetype-6.dll"
        "libharfbuzz-0.dll"
        "libpng16-16.dll"
        "libbz2-1.dll"
        "libbrotlidec.dll"
        "libbrotlicommon.dll"
        "zlib1.dll"
        "libglib-2.0-0.dll"
        "libgraphite2.dll"
        "libintl-8.dll"
        "libiconv-2.dll"
        "libpcre2-8-0.dll"
        "libgcc_s_seh-1.dll"
        "libstdc++-6.dll"
        "libwinpthread-1.dll"
    )
    
    COPIED=0
    for dll in "${DEP_DLLS[@]}"; do
        if [ -f "$DLL_SRC/$dll" ] && [ ! -f "$PROJECT_DIR/$dll" ]; then
            cp "$DLL_SRC/$dll" "$PROJECT_DIR/"
            print_success "已复制 $dll"
            COPIED=$((COPIED + 1))
        fi
    done
    
    print_success "共复制 $COPIED 个依赖 DLL"
    
    # ===== 复制 MSYS2 运行时 DLL =====
    print_step "复制 MSYS2 运行时 DLL..."
    
    MSYS_DLLS=(
        "msys-2.0.dll"
        "msys-stdc++-6.dll"
        "msys-gcc_s-seh-1.dll"
    )
    
    for dll in "${MSYS_DLLS[@]}"; do
        for path in "/usr/bin" "/bin" "/mingw64/bin" "/ucrt64/bin" "/d/msys64/usr/bin" "/d/msys64/bin"; do
            if [ -f "$path/$dll" ]; then
                cp "$path/$dll" "$PROJECT_DIR/"
                print_success "已复制 $dll"
                break
            fi
        done
    done
    
    return 0
}

compile_mingw() {
    windres src/icon.rc -O coff -o src/icon.res
    print_header "MinGW 动态编译"
    if ! check_command g++; then
        print_error "g++ 未找到！"
        read -p "按回车键继续..."
        return 1
    fi
    if ! check_sfml; then
        read -p "按回车键继续..."
        return 1
    fi
    fix_sfml30_code
    
    print_step "开始编译..."
    print_info "GCC: $(g++ --version | head -1)"
    
    SOURCES="src/main.cpp src/Game.cpp src/Tank.cpp src/Bullet.cpp src/Wall.cpp src/Explosion.cpp src/EnemyAI.cpp src/MapGenerator.cpp"
    
    g++ -std=c++17 -O2 -mwindows \
            src/main.cpp src/Game.cpp src/Tank.cpp src/Bullet.cpp \
            src/Wall.cpp src/Explosion.cpp src/EnemyAI.cpp src/MapGenerator.cpp \
            src/icon.res \
            -lsfml-graphics -lsfml-window -lsfml-system \
            -o TankBattle.exe
    
    if [ $? -eq 0 ]; then
        print_success "编译成功！"
        copy_all_dlls
        restore_backup
        return 0
    else
        print_error "编译失败！"
        restore_backup
        return 1
    fi
}

compile_cmake() {
    print_header "CMake 编译"
    CMAKE_EXE=""
    command -v cmake &> /dev/null && CMAKE_EXE="cmake"
    [ -f "/ucrt64/bin/cmake" ] && CMAKE_EXE="/ucrt64/bin/cmake"
    [ -f "/mingw64/bin/cmake" ] && CMAKE_EXE="/mingw64/bin/cmake"
    
    if [ -z "$CMAKE_EXE" ]; then
        print_error "cmake 未找到！"
        read -p "按回车键继续..."
        return 1
    fi
    
    print_info "CMake: $CMAKE_EXE"
    $CMAKE_EXE --version | head -1
    
    MAKE_EXE=""
    command -v mingw32-make &> /dev/null && MAKE_EXE="mingw32-make"
    [ -f "/ucrt64/bin/mingw32-make" ] && MAKE_EXE="/ucrt64/bin/mingw32-make"
    [ -f "/mingw64/bin/mingw32-make" ] && MAKE_EXE="/mingw64/bin/mingw32-make"
    
    if [ -z "$MAKE_EXE" ]; then
        print_error "mingw32-make 未找到！"
        read -p "按回车键继续..."
        return 1
    fi
    
    if ! check_sfml; then
        read -p "按回车键继续..."
        return 1
    fi
    
    fix_sfml30_code
    
    [ -d "build" ] && { print_step "清理 build..."; rm -rf build; }
    print_step "创建 build..."
    mkdir build && cd build
    
    print_step "运行 CMake..."
    $CMAKE_EXE .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    
    if [ $? -ne 0 ]; then
        print_error "CMake 配置失败！"
        cd "$PROJECT_DIR"
        restore_backup
        return 1
    fi
    
    print_step "开始编译..."
    $MAKE_EXE -j$(nproc 2>/dev/null || echo 4)
    
    if [ $? -eq 0 ]; then
        cd "$PROJECT_DIR"
        print_success "编译成功！"
        [ -f "build/TankBattle.exe" ] && cp build/TankBattle.exe .
        copy_all_dlls
        restore_backup
        return 0
    else
        cd "$PROJECT_DIR"
        print_error "编译失败！"
        restore_backup
        return 1
    fi
}

run_program() {
    print_header "运行程序"
    EXE_PATH=""
    [ -f "TankBattle.exe" ] && EXE_PATH="./TankBattle.exe"
    [ -f "build/TankBattle.exe" ] && EXE_PATH="./build/TankBattle.exe"
    
    if [ -z "$EXE_PATH" ]; then
        print_error "找不到可执行文件！"
        return 1
    fi
    
    print_step "运行: $EXE_PATH"
    echo ""
    "$EXE_PATH"
}

clean_all() {
    print_header "清理"
    print_step "删除可执行文件..."; rm -f TankBattle.exe
    print_step "删除 DLL..."; rm -f sfml-*.dll libsfml-*.dll libfreetype-*.dll libharfbuzz-*.dll libpng*.dll libbz2-*.dll libbrotli*.dll zlib*.dll libglib-*.dll libgraphite*.dll libintl-*.dll libiconv-*.dll libpcre2-*.dll libwinpthread-*.dll libgcc_s_*.dll libstdc++-*.dll msys-*.dll
    print_step "删除 build..."; rm -rf build
    print_step "删除备份..."; rm -f src/*.bak
    print_step "删除发布包..."; rm -f TankBattle_Release.zip
    print_success "清理完成！"
}

do_zip_package() {
    RELEASE_DIR="TankBattle_Release"
    rm -rf "$RELEASE_DIR"
    mkdir -p "$RELEASE_DIR"
    
    print_step "复制 EXE 和 DLL 到 $RELEASE_DIR"
    cp TankBattle.exe "$RELEASE_DIR/"
    cp *.dll "$RELEASE_DIR/" 2>/dev/null || print_info "没有 DLL 需要复制"
    
    cat > "$RELEASE_DIR/run.bat" << 'EOF'
@echo off
TankBattle.exe
pause
EOF
    
    print_step "打包成 ZIP..."
    cd "$RELEASE_DIR"
    zip -r ../TankBattle_Release.zip * > /dev/null
    cd ..
    
    if [ $? -eq 0 ]; then
        print_success "ZIP 打包成功！"
        print_info "输出: $PROJECT_DIR/TankBattle_Release.zip"
        rm -rf "$RELEASE_DIR"
        
        # ===== 打包完成后删除当前目录的 EXE 和 DLL =====
        print_step "清理当前目录的 EXE 和 DLL..."
        rm -f TankBattle.exe
        rm -f *.dll 2>/dev/null
        print_success "已清理，保留 ZIP 发布包"
        
        return 0
    else
        print_error "打包失败！"
        rm -rf "$RELEASE_DIR"
        return 1
    fi
}

package_release() {
    print_header "打包发布包"
    
    if [ ! -f "TankBattle.exe" ]; then
        print_error "TankBattle.exe 不存在，请先编译！"
        return 1
    fi
    
    if ! check_command zip; then
        print_warning "zip 未安装，正在安装..."
        pacman -S zip
        if [ $? -ne 0 ]; then
            print_error "zip 安装失败，请手动安装: pacman -S zip"
            return 1
        fi
    fi
    
    do_zip_package
}

check_dependencies() {
    print_header "检查依赖"
    check_command g++ && print_success "g++: $(g++ --version | head -1)" || print_error "g++ 未安装"
    check_command cmake && print_success "cmake: $(cmake --version | head -1)" || print_info "cmake 可选"
    check_sfml
    read -p "按回车键继续..."
}

install_sfml() {
    print_header "安装 SFML"
    if check_command pacman; then
        pacman -S mingw-w64-ucrt-x86_64-sfml
        [ $? -eq 0 ] && print_success "安装成功！" || print_error "安装失败！"
    else
        print_error "pacman 未找到"
    fi
    read -p "按回车键继续..."
}

show_menu() {
    clear
    echo "${CYAN}========================================${NC}"
    echo "${CYAN}      坦克大战 - C++ 编译工具          ${NC}"
    echo "${CYAN}========================================${NC}"
    echo ""
    echo "  ${GREEN}1${NC}) MinGW 动态编译 (推荐)"
    echo "  ${GREEN}2${NC}) CMake 编译"
    echo "  ${GREEN}3${NC}) 运行程序"
    echo "  ${GREEN}4${NC}) 打包发布包 (ZIP)"
    echo "  ${GREEN}5${NC}) 清理"
    echo "  ${GREEN}6${NC}) 复制 DLL（手动）"
    echo "  ${GREEN}7${NC}) 检查依赖"
    echo "  ${GREEN}8${NC}) 安装 SFML"
    echo "  ${GREEN}9${NC}) 退出"
    echo ""
    echo "${CYAN}========================================${NC}"
    echo ""
}

while true; do
    show_menu
    read -p "请选择 [1-9]: " choice
    case $choice in
        1) compile_mingw; read -p "按回车键继续..." ;;
        2) compile_cmake; read -p "按回车键继续..." ;;
        3) run_program; read -p "按回车键继续..." ;;
        4) package_release; read -p "按回车键继续..." ;;
        5) clean_all; read -p "按回车键继续..." ;;
        6) copy_all_dlls; read -p "按回车键继续..." ;;
        7) check_dependencies ;;
        8) install_sfml ;;
        9) echo ""; echo "${GREEN}再见！${NC}"; exit 0 ;;
        *) print_error "无效选择"; sleep 1 ;;
    esac
done