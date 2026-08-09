#!/bin/bash

# ========================================
#   坦克大战 - C++ SFML 3.0 编译脚本
#   支持 MinGW 动态编译 + CMake 编译
#   自动复制所有 DLL
#   包含模组、资源包、Lua 脚本支持
# ========================================

export PATH="/ucrt64/bin:/mingw64/bin:/usr/bin:$PATH"

# 颜色
RED=''
GREEN=''
YELLOW=''
BLUE=''
CYAN=''
MAGENTA=''
NC=''

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

# ============================================
# 检查依赖
# ============================================
check_dependencies() {
    print_header "检查依赖"

    # GCC
    if check_command g++; then
        print_success "g++: $(g++ --version | head -1)"
    else
        print_error "g++ 未安装"
        print_info "安装: pacman -S mingw-w64-ucrt-x86_64-gcc"
        return 1
    fi

    # CMake
    if check_command cmake; then
        print_success "cmake: $(cmake --version | head -1)"
    else
        print_warning "cmake 可选"
    fi

    # SFML 3.0
    print_step "检查 SFML 3.0..."
    if [ -f "/ucrt64/include/SFML/Graphics.hpp" ] || [ -f "/mingw64/include/SFML/Graphics.hpp" ]; then
        print_success "SFML 已安装"
    else
        print_error "SFML 未找到！"
        print_info "安装: pacman -S mingw-w64-ucrt-x86_64-sfml"
        return 1
    fi

    # Lua
    print_step "检查 Lua..."
    if [ -f "/ucrt64/include/lua.h" ] || [ -f "/mingw64/include/lua.h" ]; then
        print_success "Lua 已安装"
    else
        print_warning "Lua 未安装（脚本功能将不可用）"
        print_info "安装: pacman -S mingw-w64-ucrt-x86_64-lua"
    fi

    # nlohmann/json
    print_step "检查 nlohmann/json..."
    if [ -f "/ucrt64/include/nlohmann/json.hpp" ] || [ -f "/mingw64/include/nlohmann/json.hpp" ]; then
        print_success "nlohmann/json 已安装"
    else
        print_warning "nlohmann/json 未安装（JSON 解析需要）"
        print_info "安装: pacman -S mingw-w64-ucrt-x86_64-nlohmann-json"
    fi

    # libzip (可选)
    print_step "检查 libzip..."
    if [ -f "/ucrt64/include/zip.h" ] || [ -f "/mingw64/include/zip.h" ]; then
        print_success "libzip 已安装 (ZIP 资源包支持)"
    else
        print_info "libzip 可选 (ZIP 资源包需要)"
        print_info "安装: pacman -S mingw-w64-ucrt-x86_64-libzip"
    fi

    echo ""
    read -p "按回车键继续..."
    return 0
}

# ============================================
# 安装依赖
# ============================================
install_dependencies() {
    print_header "安装依赖"

    if ! check_command pacman; then
        print_error "pacman 未找到"
        return 1
    fi

    print_step "安装 SFML 3.0..."
    pacman -S mingw-w64-ucrt-x86_64-sfml

    print_step "安装 Lua..."
    pacman -S mingw-w64-ucrt-x86_64-lua

    print_step "安装 nlohmann/json..."
    pacman -S mingw-w64-ucrt-x86_64-nlohmann-json

    print_step "安装 libzip (可选)..."
    pacman -S mingw-w64-ucrt-x86_64-libzip

    print_step "安装 CMake..."
    pacman -S mingw-w64-ucrt-x86_64-cmake

    print_success "依赖安装完成！"
    read -p "按回车键继续..."
}

# ============================================
# 复制 DLL
# ============================================
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

    # ===== 音频编解码器 DLL =====
    print_step "复制音频编解码器 DLL..."

    AUDIO_DLLS=(
        "libFLAC.dll"
        "libvorbis-0.dll"
        "libvorbisenc-2.dll"
        "libogg-0.dll"
        "libmpg123-0.dll"
        "libsndfile-1.dll"
        "libvorbisfile-3.dll"
    )

    COPIED_AUDIO=0
    for dll in "${AUDIO_DLLS[@]}"; do
        FOUND=0
        for src_path in "$DLL_SRC" "/mingw64/bin" "/usr/bin" "/bin"; do
            if [ -f "$src_path/$dll" ]; then
                if [ ! -f "$PROJECT_DIR/$dll" ]; then
                    cp "$src_path/$dll" "$PROJECT_DIR/"
                    print_success "已复制 $dll (from $src_path)"
                    COPIED_AUDIO=$((COPIED_AUDIO + 1))
                else
                    print_info "$dll 已存在，跳过"
                fi
                FOUND=1
                break
            fi
        done
        if [ $FOUND -eq 0 ]; then
            print_warning "$dll 不存在"
        fi
    done

    print_success "共复制 $COPIED_AUDIO 个音频 DLL"

    # ===== 依赖库 DLL =====
    print_step "复制依赖库 DLL..."

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

    # ===== Lua DLL =====
    print_step "复制 Lua DLL..."
    LUA_DLLS=(
        "lua54.dll"
        "lua53.dll"
        "liblua54.dll"
        "liblua53.dll"
        "lua55.dll"
        "liblua55.dll"
    )

    for dll in "${LUA_DLLS[@]}"; do
        for src_path in "$DLL_SRC" "/mingw64/bin" "/usr/bin" "/bin"; do
            if [ -f "$src_path/$dll" ]; then
                if [ ! -f "$PROJECT_DIR/$dll" ]; then
                    cp "$src_path/$dll" "$PROJECT_DIR/"
                    print_success "已复制 $dll"
                else
                    print_info "$dll 已存在，跳过"
                fi
                break
            fi
        done
    done

    # ===== libzip DLL =====
    print_step "复制 libzip DLL..."
    LIBZIP_DLLS=(
        "libzip.dll"
        "libzip-5.dll"
        "libzip-4.dll"
    )

    for dll in "${LIBZIP_DLLS[@]}"; do
        for src_path in "$DLL_SRC" "/mingw64/bin" "/usr/bin" "/bin"; do
            if [ -f "$src_path/$dll" ]; then
                if [ ! -f "$PROJECT_DIR/$dll" ]; then
                    cp "$src_path/$dll" "$PROJECT_DIR/"
                    print_success "已复制 $dll"
                else
                    print_info "$dll 已存在，跳过"
                fi
                break
            fi
        done
    done

    # ===== ✅ 压缩库 DLL (libzip 依赖) =====
    print_step "复制压缩库 DLL..."

    COMPRESS_DLLS=(
        "liblzma-5.dll"
        "libzstd.dll"
    )

    COPIED_COMPRESS=0
    for dll in "${COMPRESS_DLLS[@]}"; do
        for src_path in "$DLL_SRC" "/mingw64/bin" "/usr/bin" "/bin"; do
            if [ -f "$src_path/$dll" ]; then
                if [ ! -f "$PROJECT_DIR/$dll" ]; then
                    cp "$src_path/$dll" "$PROJECT_DIR/"
                    print_success "已复制 $dll (from $src_path)"
                    COPIED_COMPRESS=$((COPIED_COMPRESS + 1))
                else
                    print_info "$dll 已存在，跳过"
                fi
                break
            fi
        done
    done

    if [ $COPIED_COMPRESS -gt 0 ]; then
        print_success "共复制 $COPIED_COMPRESS 个压缩库 DLL"
    fi

    # ===== MSYS2 运行时 DLL =====
    print_step "复制 MSYS2 运行时 DLL..."
    MSYS_DLLS=(
        "msys-2.0.dll"
        "msys-stdc++-6.dll"
        "msys-gcc_s_seh-1.dll"
    )

    for dll in "${MSYS_DLLS[@]}"; do
        for path in "/usr/bin" "/bin" "/mingw64/bin" "/ucrt64/bin"; do
            if [ -f "$path/$dll" ] && [ ! -f "$PROJECT_DIR/$dll" ]; then
                cp "$path/$dll" "$PROJECT_DIR/"
                print_success "已复制 $dll"
                break
            fi
        done
    done

    print_success "DLL 复制完成！"
    return 0
}

# ============================================
# MinGW 直接编译
# ============================================
compile_mingw() {
    print_header "MinGW 动态编译"

    if ! check_command g++; then
        print_error "g++ 未找到！"
        return 1
    fi

    if ! check_dependencies_quick; then
        print_error "依赖检查失败！"
        return 1
    fi

    print_step "开始编译..."
    print_info "GCC: $(g++ --version | head -1)"

    # 编译资源文件
    if [ -f "src/icon.rc" ]; then
        print_step "编译资源文件..."
        windres src/icon.rc -O coff -o src/icon.res
    fi

    # 源文件列表
    SOURCES="src/main.cpp \
        src/Game.cpp \
        src/Tank.cpp \
        src/Bullet.cpp \
        src/Wall.cpp \
        src/Explosion.cpp \
        src/EnemyAI.cpp \
        src/MapGenerator.cpp \
        src/Menu.cpp \
        src/PowerUp.cpp \
        src/Boss.cpp \
        src/LevelData.cpp \
        src/LevelState.cpp \
        src/SaveManager.cpp \
        src/SoundManager.cpp \
        src/SettingsMenu.cpp \
        src/ConfigManager.cpp \
        src/mod/ModLoader.cpp \
        src/resource/TextureManager.cpp \
        src/resource/ResourcePackLoader.cpp \
        src/resource/MapLoader.cpp \
        src/script/ScriptEngine.cpp"

    # 检查是否有 icon.res
    RESOURCES=""
    [ -f "src/icon.res" ] && RESOURCES="src/icon.res"

    # 编译命令
    g++ -std=c++17 -O2 -mwindows \
        $SOURCES \
        $RESOURCES \
        -I/src \
        -I/src/mod \
        -I/src/resource \
        -I/src/script \
        -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio \
        -llua -lzip \
        -o TankBattle.exe

    if [ $? -eq 0 ]; then
        print_success "编译成功！"
        copy_all_dlls
        return 0
    else
        print_error "编译失败！"
        return 1
    fi
}

# ============================================
# CMake 编译
# ============================================
compile_cmake() {
    print_header "CMake 编译"

    CMAKE_EXE=""
    command -v cmake &> /dev/null && CMAKE_EXE="cmake"
    [ -f "/ucrt64/bin/cmake" ] && CMAKE_EXE="/ucrt64/bin/cmake"
    [ -f "/mingw64/bin/cmake" ] && CMAKE_EXE="/mingw64/bin/cmake"

    if [ -z "$CMAKE_EXE" ]; then
        print_error "cmake 未找到！"
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
        return 1
    fi

    # 清理并创建 build 目录
    [ -d "build" ] && { print_step "清理 build..."; rm -rf build; }
    print_step "创建 build..."
    mkdir build && cd build

    print_step "运行 CMake..."
    $CMAKE_EXE .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

    if [ $? -ne 0 ]; then
        print_error "CMake 配置失败！"
        cd "$PROJECT_DIR"
        return 1
    fi

    print_step "开始编译..."
    $MAKE_EXE -j$(nproc 2>/dev/null || echo 4)

    if [ $? -eq 0 ]; then
        cd "$PROJECT_DIR"
        print_success "编译成功！"
        [ -f "build/TankBattle.exe" ] && cp build/TankBattle.exe .
        copy_all_dlls
        return 0
    else
        cd "$PROJECT_DIR"
        print_error "编译失败！"
        return 1
    fi
}

# ============================================
# 快速依赖检查
# ============================================
check_dependencies_quick() {
    if [ ! -f "/ucrt64/include/SFML/Graphics.hpp" ] && [ ! -f "/mingw64/include/SFML/Graphics.hpp" ]; then
        print_error "SFML 未安装！"
        return 1
    fi
    return 0
}

# ============================================
# 运行程序
# ============================================
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

# ============================================
# 清理
# ============================================
clean_all() {
    print_header "清理"

    print_step "删除可执行文件..."
    rm -f TankBattle.exe

    print_step "删除 DLL..."
    rm -f sfml-*.dll libsfml-*.dll libfreetype-*.dll libharfbuzz-*.dll
    rm -f libpng*.dll libbz2-*.dll libbrotli*.dll zlib*.dll
    rm -f libglib-*.dll libgraphite*.dll libintl-*.dll libiconv-*.dll
    rm -f libpcre2-*.dll libwinpthread-*.dll libgcc_s_*.dll libstdc++-*.dll
    rm -f msys-*.dll libFLAC*.dll libvorbis*.dll libogg*.dll
    rm -f libmpg123*.dll libsndfile*.dll lua*.dll liblua*.dll

    print_step "删除 build..."
    rm -rf build

    print_step "删除 release..."
    rm -rf TankBattle_Release
    rm -f TankBattle_Release.zip

    print_step "删除临时文件..."
    rm -f src/*.res
    rm -f src/*.bak

    print_success "清理完成！"
}

# ============================================
# 打包发布
# ============================================
package_release() {
    print_header "打包发布包"

    if [ ! -f "TankBattle.exe" ]; then
        print_error "TankBattle.exe 不存在，请先编译！"
        return 1
    fi

    RELEASE_DIR="TankBattle_Release"
    rm -rf "$RELEASE_DIR"
    mkdir -p "$RELEASE_DIR"

    print_step "复制 EXE 和 DLL 到 $RELEASE_DIR"
    cp TankBattle.exe "$RELEASE_DIR/"
    cp *.dll "$RELEASE_DIR/" 2>/dev/null || print_info "没有 DLL 需要复制"

    # 复制 assets 目录
    if [ -d "assets" ]; then
        cp -r assets "$RELEASE_DIR/"
        print_success "已复制 assets 目录"
    fi

    # 创建目录结构（模组、资源包、脚本）
    mkdir -p "$RELEASE_DIR/mods"
    mkdir -p "$RELEASE_DIR/resourcepacks"
    mkdir -p "$RELEASE_DIR/Scripts"
    mkdir -p "$RELEASE_DIR/PlayerData"

    # 创建配置文件
    cat > "$RELEASE_DIR/config.ini" << 'EOF'
; ============================================
;  坦克大战 - 游戏配置文件
;  修改后重启游戏生效
; ============================================

[Game]
FPS = 60

[Tank]
Size = 36
Speed = 7
PlayerLives = 3
ShotCooldown = 0.5

[Bullet]
Size = 8
Speed = 520
MaxBullets = 3

[Enemy]
Count = 4
SpawnInterval = 4.0
AIDirectionChange = 2.0
AIShootChance = 0.4

[PowerUp]
SpawnInterval = 10.0
MaxPowerups = 3
EOF

    # 创建 settings.json
    cat > "$RELEASE_DIR/settings.json" << 'EOF'
{
    "window": {
        "width": 1600,
        "height": 900,
        "fullscreen": false
    },
    "audio": {
        "volume": 0.8,
        "muted": false
    },
    "keys": {
        "player1": {
            "up": "Up",
            "down": "Down",
            "left": "Left",
            "right": "Right",
            "shoot": "Space"
        },
        "player2": {
            "up": "W",
            "down": "S",
            "left": "A",
            "right": "D",
            "shoot": "J"
        }
    }
}
EOF

    # 创建模组列表
    cat > "$RELEASE_DIR/mods/mod_list.txt" << 'EOF'
# Mod list - one mod DLL/SO file per line
# Add # before filename to disable
# Example:
# MyMod.dll
# #DisabledMod.dll
EOF

    # 创建示例 Lua 脚本
    cat > "$RELEASE_DIR/Scripts/example.lua" << 'EOF'
-- 示例 Lua 脚本
function onGameStart()
    print('Hello from Lua!')
end

function getEnemySpeed()
    return 1.0
end

function onEnemySpawn(enemy)
    -- 可以修改敌人属性
    return enemy
end
EOF

    # 创建运行脚本
    cat > "$RELEASE_DIR/run.bat" << 'EOF'
@echo off
chcp 65001 >nul
TankBattle.exe
pause
EOF

    # 创建 README
    cat > "$RELEASE_DIR/README.txt" << 'EOF'
坦克大战 - 游戏说明 (C++ SFML 3.0)

控制方式：
P1: 方向键移动 + SPACE射击
P2: WASD移动 + J射击
P: 暂停
R: 重新开始
ESC: 退出游戏/返回主菜单

游戏模式：
- 单机模式：玩家1独自挑战
- PVP模式：玩家1 vs 玩家2
- PVE模式：合作对抗电脑
- 关卡模式：挑战10个关卡
- 无尽模式：无限生存

模组系统：
- 将 DLL/SO 文件放入 mods/ 目录
- 编辑 mods/mod_list.txt 启用/禁用
- 模组需实现 IMod 接口

资源包系统：
- 将资源包放入 resourcepacks/ 目录
- 支持文件夹或 ZIP 格式
- 包含 pack.json 配置文件

Lua 脚本：
- 将 .lua 文件放入 Scripts/ 目录
- 支持 onGameStart(), getEnemySpeed() 等

配置文件：
- config.ini: 游戏参数
- settings.json: 窗口、音频、按键设置
EOF

    print_step "打包成 ZIP..."
    cd "$RELEASE_DIR"
    zip -r ../TankBattle_Release.zip * > /dev/null
    cd ..

    if [ $? -eq 0 ]; then
        print_success "ZIP 打包成功！"
        print_info "输出: $PROJECT_DIR/TankBattle_Release.zip"
        rm -rf "$RELEASE_DIR"

        # ===== 打包完成后清理 EXE 和 DLL =====
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

# ============================================
# 显示菜单
# ============================================
show_menu() {
    clear
    echo "${CYAN}========================================${NC}"
    echo "${CYAN}      坦克大战 - C++ 编译工具          ${NC}"
    echo "${CYAN}========================================${NC}"
    echo ""
    echo " (1) MinGW 直接编译 (推荐)"
    echo " (2) CMake 编译"
    echo " (3) 运行程序"
    echo " (4) 打包发布包 (ZIP)"
    echo " (5) 复制 DLL（手动）"
    echo " (6) 检查依赖"
    echo " (7) 安装依赖"
    echo " (8) 清理"
    echo " (9) 退出"
    echo ""
    echo "${CYAN}========================================${NC}"
    echo ""
}

# ============================================
# 主循环
# ============================================
while true; do
    show_menu
    read -p "请选择 [1-9]: " choice
    case $choice in
        1) compile_mingw; read -p "按回车键继续..." ;;
        2) compile_cmake; read -p "按回车键继续..." ;;
        3) run_program; read -p "按回车键继续..." ;;
        4) package_release; read -p "按回车键继续..." ;;
        5) copy_all_dlls; read -p "按回车键继续..." ;;
        6) check_dependencies; ;;
        7) install_dependencies; ;;
        8) clean_all; read -p "按回车键继续..." ;;
        9) echo ""; echo "${GREEN}再见！${NC}"; exit 0 ;;
        *) print_error "无效选择"; sleep 1 ;;
    esac
done