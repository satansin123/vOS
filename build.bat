@echo off
setlocal

REM Check if we're already in a VS environment
if defined VCINSTALLDIR (
    echo Already in Visual Studio environment
    goto :build
)

REM Try to find and setup Visual Studio 2022 environment
set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022"
if exist "%VS_PATH%\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Community\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)
if exist "%VS_PATH%\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Professional\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)
if exist "%VS_PATH%\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

echo Error: Visual Studio 2022 not found. Please install Visual Studio 2022 or run from Developer Command Prompt.
exit /b 1

:build
echo Configuring project...
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Configuration failed
    exit /b 1
)

echo Building project...
cmake --build build
if errorlevel 1 (
    echo Build failed
    exit /b 1
)

echo Build completed successfully!
