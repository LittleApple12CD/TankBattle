@echo off
setlocal enabledelayedexpansion

:: ========================================
::   坦克大战 - Java 编译工具
:: ========================================

set "PROJECT_DIR=%~dp0"
cd /d "%PROJECT_DIR%"

:show_menu
cls
echo ========================================
echo        坦克大战 - Java 编译工具
echo ========================================
echo.
echo  1. 编译并运行
echo  2. 打包 JAR
echo  3. 仅编译
echo  4. 清理
echo  5. 退出
echo.
echo ========================================
echo.
set /p "choice=请选择 [1-5]: "

if "%choice%"=="1" goto compile_run
if "%choice%"=="2" goto build_jar
if "%choice%"=="3" goto compile_only
if "%choice%"=="4" goto clean_all
if "%choice%"=="5" goto exit

echo [ERROR] 无效选择
ping -n 2 127.0.0.1 >nul
goto show_menu

:check_java
java -version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Java 未安装
    pause
    goto show_menu
)
javac -version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] javac 未安装
    pause
    goto show_menu
)
exit /b 0

:do_compile
echo.
echo [STEP] 清理旧文件...
if exist out rmdir /s /q out

echo [STEP] 编译源代码...
echo [INFO] 依赖库: lib/*.jar
echo.

javac -d out -encoding UTF-8 -cp "lib/*" ^
    src/com/tankbattle/*.java ^
    src/com/tankbattle/mod/*.java ^
    src/com/tankbattle/resource/*.java ^
    src/com/tankbattle/script/*.java

if %errorlevel% neq 0 (
    echo [ERROR] 编译失败！
    pause
    exit /b 1
)

echo [OK] 编译完成！
exit /b 0

:compile_run
echo.
echo ========================================
echo   编译并运行
echo ========================================
echo.
call :check_java
call :do_compile

if %errorlevel% equ 0 (
    echo.
    echo [STEP] 正在运行...
    echo.
    java -cp "out;lib/*" com.tankbattle.Main
)
pause
goto show_menu

:build_jar
echo.
echo ========================================
echo   打包 JAR
echo ========================================
echo.
call :check_java
call :do_compile

if %errorlevel% neq 0 goto show_menu

echo.
echo [STEP] 创建清单文件...
(
    echo Manifest-Version: 1.0
    echo Main-Class: com.tankbattle.Main
    echo Class-Path: . lib/json-20240303.jar lib/luaj-jse-3.0.2.jar
) > MANIFEST.MF

echo [STEP] 打包 JAR...
jar cvfm TankBattle.jar MANIFEST.MF -C out . -C src icon.png

if %errorlevel% neq 0 (
    echo [ERROR] 打包失败！
    pause
    goto show_menu
)

del MANIFEST.MF
rmdir /s /q out

echo.
echo ========================================
echo   [OK] 打包完成！
echo   输出: TankBattle.jar
echo ========================================
echo.
dir TankBattle.jar | find "TankBattle.jar"
echo.

echo ========================================
echo  是否现在运行?
echo   1) 是
echo   2) 否
echo.
set /p "run_choice=请输入 [1/2]: "

if "%run_choice%"=="1" (
    echo.
    java -jar TankBattle.jar
)

pause
goto show_menu

:compile_only
echo.
echo ========================================
echo   仅编译
echo ========================================
echo.
call :check_java
call :do_compile

if %errorlevel% equ 0 (
    echo.
    echo [OK] 编译完成！
    echo [INFO] 输出目录: out/
)
pause
goto show_menu

:clean_all
echo.
echo ========================================
echo   清理
echo ========================================
echo.
if exist out rmdir /s /q out
if exist TankBattle.jar del TankBattle.jar
if exist MANIFEST.MF del MANIFEST.MF
echo [OK] 清理完成！
pause
goto show_menu

:exit
echo.
echo 再见！
exit /b 0