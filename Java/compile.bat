@echo off
chcp 65001 >nul
echo ========================================
echo   坦克大战 - Java 编译运行
echo ========================================
echo.

cd /d "%~dp0"

echo 检查 Java 环境...
java -version >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未找到 Java！
    pause
    exit /b 1
)

echo 清理旧文件...
if exist out rmdir /s /q out

echo 编译源代码...
javac -d out -encoding UTF-8 src/com/tankbattle/*.java

if %errorlevel% == 0 (
    echo.
    echo ========================================
    echo   [成功] 编译完成！
    echo ========================================
    echo.
    echo 正在运行...
    echo.
    java -cp out com.tankbattle.Main
) else (
    echo.
    echo ========================================
    echo   [失败] 编译失败！
    echo ========================================
    pause
)