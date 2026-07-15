@echo off
chcp 65001 >nul
echo ========================================
echo   坦克大战 - 打包 JAR
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
if exist TankBattle.jar del TankBattle.jar

echo 编译源代码...
javac -d out -encoding UTF-8 src/com/tankbattle/*.java

if %errorlevel% neq 0 (
    echo [错误] 编译失败！
    pause
    exit /b 1
)

echo 创建清单文件...
echo Manifest-Version: 1.0 > MANIFEST.MF
echo Main-Class: com.tankbattle.Main >> MANIFEST.MF
echo Class-Path: . >> MANIFEST.MF

echo 打包 JAR...
jar cvfm TankBattle.jar MANIFEST.MF -C out .
if %errorlevel% neq 0 (
    echo [错误] 打包失败！
    pause
    exit /b 1
)

del MANIFEST.MF
rmdir /s /q out

echo.
echo ========================================
echo   [成功] 打包完成！
echo   输出: TankBattle.jar
echo ========================================
echo.
echo   文件大小:
dir TankBattle.jar | find "TankBattle.jar"
echo.
echo ========================================
echo  是否现在运行?
echo   1) 是 (运行)
echo   2) 否 (退出)
echo.
set /p run_choice="请输入 [1/2]: "

if "%run_choice%"=="1" (
    echo.
    echo 正在运行...
    java -jar TankBattle.jar
) else (
    echo.
    echo 已退出。
)

pause