@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo   坦克大战 - Python 高级打包工具
echo ========================================
echo.

cd /d "%~dp0"

REM 检查 Python
echo 检查 Python 环境...
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未找到 Python，请先安装 Python 3.6+
    echo 下载地址: https://www.python.org/downloads/
    pause
    exit /b 1
)

REM 检查依赖
echo 检查依赖...
python -c "import pygame" >nul 2>&1
if %errorlevel% neq 0 (
    echo [信息] 安装依赖...
    pip install -r requirements.txt -q
)

python -c "import PyInstaller" >nul 2>&1
if %errorlevel% neq 0 (
    echo [信息] 安装 PyInstaller...
    pip install pyinstaller -q
)

echo.
echo ========================================
echo  请选择打包方式:
echo ========================================
echo   1) 单文件版 (一个 EXE，启动稍慢)
echo   2) 文件夹版 (多个文件，启动更快)
echo   3) 单文件 + 控制台 (显示调试信息)
echo   4) 优化版 (UPX压缩，文件更小)
echo   5) 高兼容版 (兼容 Windows 7+)
echo   6) 取消
echo.
set /p choice="请输入 [1-6]: "

REM 通用参数
set PYINSTALLER_ARGS=--name="TankBattle" --clean --noconfirm

REM 添加图标（如果有）
if exist icon.ico (
    set PYINSTALLER_ARGS=!PYINSTALLER_ARGS! --icon=icon.ico
)

if "%choice%"=="1" goto single
if "%choice%"=="2" goto folder
if "%choice%"=="3" goto console
if "%choice%"=="4" goto upx
if "%choice%"=="5" goto compatible
if "%choice%"=="6" goto cancel

echo [错误] 无效选择！
pause
exit /b 1

:single
echo.
echo [信息] 开始打包为单文件 EXE...
echo [信息] 这可能需要 1-3 分钟...
if exist icon.ico (
    pyinstaller --onefile --noconsole !PYINSTALLER_ARGS! --icon=icon.ico main.py
) else (
    pyinstaller --onefile --noconsole !PYINSTALLER_ARGS! main.py
)
goto done

:folder
echo.
echo [信息] 开始打包为文件夹版 EXE...
echo [信息] 这可能需要 1-3 分钟...
if exist icon.ico (
    pyinstaller --onedir --noconsole !PYINSTALLER_ARGS! --icon=icon.ico main.py
) else (
    pyinstaller --onedir --noconsole !PYINSTALLER_ARGS! main.py
)
goto done

:console
echo.
echo [信息] 开始打包为单文件 + 控制台版 EXE...
echo [信息] 这可能需要 1-3 分钟...
if exist icon.ico (
    pyinstaller --onefile --console !PYINSTALLER_ARGS! --icon=icon.ico main.py
) else (
    pyinstaller --onefile --console !PYINSTALLER_ARGS! main.py
)
goto done

:upx
echo.
echo [信息] 优化打包 (需要 UPX)...
echo [信息] 如果提示 UPX 错误，请忽略，文件仍然可用
if exist icon.ico (
    pyinstaller --onefile --noconsole --upx-dir="C:\upx" !PYINSTALLER_ARGS! --icon=icon.ico main.py 2>nul
) else (
    pyinstaller --onefile --noconsole --upx-dir="C:\upx" !PYINSTALLER_ARGS! main.py 2>nul
)
if %errorlevel% neq 0 (
    echo [警告] UPX 压缩失败，使用普通模式...
    if exist icon.ico (
        pyinstaller --onefile --noconsole !PYINSTALLER_ARGS! --icon=icon.ico main.py
    ) else (
        pyinstaller --onefile --noconsole !PYINSTALLER_ARGS! main.py
    )
)
goto done

:compatible
echo.
echo [信息] 高兼容打包 (支持 Windows 7+)...
if exist icon.ico (
    pyinstaller --onefile --noconsole --target-arch=x86_64 !PYINSTALLER_ARGS! --icon=icon.ico main.py
) else (
    pyinstaller --onefile --noconsole --target-arch=x86_64 !PYINSTALLER_ARGS! main.py
)
goto done

:cancel
echo.
echo 已取消打包
pause
exit /b 0

:done
if %errorlevel%==0 (
    echo.
    echo ========================================
    echo   [成功] 打包完成！
    echo ========================================
    echo.
    echo   输出位置: dist\TankBattle.exe
    
    if "%choice%"=="2" (
        echo   文件夹位置: dist\TankBattle\
        echo   运行文件: dist\TankBattle\TankBattle.exe
    )
    
    echo.
    echo   文件大小:
    dir dist\TankBattle.exe 2>nul | find "TankBattle.exe"
    
    echo.
    echo ========================================
    echo  是否现在运行?
    echo   1) 是 (运行)
    echo   2) 否 (退出)
    echo.
    set /p run_choice="请输入 [1/2]: "
    if "!run_choice!"=="1" (
        echo.
        echo 正在运行...
        cd dist
        TankBattle.exe
        cd ..
    )
) else (
    echo.
    echo ========================================
    echo   [失败] 打包失败！
    echo ========================================
    echo.
    echo 常见问题:
    echo   1. 检查是否缺少依赖: pip install -r requirements.txt
    echo   2. 检查 main.py 是否存在
    echo   3. 检查所有 Python 文件是否有语法错误
    echo   4. 尝试以管理员身份运行
    echo   5. 查看详细错误: dist\*.log
    echo.
    echo 查看错误日志:
    type dist\*.log 2>nul
)

echo.
pause