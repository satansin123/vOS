@echo off
setlocal enabledelayedexpansion

:: Dynamic path detection
set "PROJECT_ROOT=%~dp0.."
set "BUILD_DIR=%PROJECT_ROOT%\build_debug"

echo === Debug Build System ===
echo Project Root: %PROJECT_ROOT%
echo Debug Build Directory: %BUILD_DIR%
echo.

:: Check VS environment
if defined VCINSTALLDIR (
    echo Already in Visual Studio environment
    goto :debug_build
)

:: Setup Visual Studio environment
echo Setting up Visual Studio environment...
set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022"

if exist "%VS_PATH%\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    goto :debug_build
)
if exist "%VS_PATH%\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Professional\VC\Auxiliary\Build\vcvars64.bat"
    goto :debug_build
)
if exist "%VS_PATH%\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Community\VC\Auxiliary\Build\vcvars64.bat"
    goto :debug_build
)

echo ERROR: Visual Studio 2022 not found!
exit /b 1

:debug_build
echo.
echo Configuring for DEBUG build...
cmake -B "%BUILD_DIR%" -S "%PROJECT_ROOT%" -G Ninja -DCMAKE_BUILD_TYPE=Debug

if errorlevel 1 (
    echo Debug configuration failed!
    exit /b 1
)

echo.
echo Building DEBUG version...
cmake --build "%BUILD_DIR%"

if errorlevel 1 (
    echo Debug build failed!
    exit /b 1
)

echo.
echo Debug build completed successfully!
echo.
echo Starting debugger...

:: Get project name dynamically
for %%I in ("%PROJECT_ROOT%") do set "PROJECT_NAME=%%~nxI"

:: Launch with Visual Studio debugger
if exist "%BUILD_DIR%\bin\%PROJECT_NAME%.exe" (
    echo Launching: %BUILD_DIR%\bin\%PROJECT_NAME%.exe
    devenv /debugexe "%BUILD_DIR%\bin\%PROJECT_NAME%.exe" debug
) else (
    echo Executable not found! Check build output.
    exit /b 1
)
