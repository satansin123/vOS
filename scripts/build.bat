@echo off
setlocal enabledelayedexpansion

:: Dynamic path detection
set "PROJECT_ROOT=%~dp0.."
set "BUILD_DIR=%PROJECT_ROOT%\build"
set "SCRIPTS_DIR=%PROJECT_ROOT%\scripts"

echo === Dynamic C++ Build System ===
echo Project Root: %PROJECT_ROOT%
echo Build Directory: %BUILD_DIR%
echo Scripts Directory: %SCRIPTS_DIR%
echo.

:: Check if we're in VS environment
if defined VCINSTALLDIR (
    echo Already in Visual Studio environment
    goto :build
)

:: Setup Visual Studio environment
echo Setting up Visual Studio environment...
set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022"

if exist "%VS_PATH%\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)
if exist "%VS_PATH%\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Professional\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)
if exist "%VS_PATH%\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "%VS_PATH%\Community\VC\Auxiliary\Build\vcvars64.bat"
    goto :build
)

echo ERROR: Visual Studio 2022 not found!
echo Please install Visual Studio 2022 or run from Developer Command Prompt.
exit /b 1

:build
echo.
echo Configuring project...
cmake -B "%BUILD_DIR%" -S "%PROJECT_ROOT%" -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Configuration failed - attempting to clean and retry...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
    )
    cmake -B "%BUILD_DIR%" -S "%PROJECT_ROOT%" -G Ninja -DCMAKE_BUILD_TYPE=Release
    if errorlevel 1 (
        echo Configuration still failed!
        exit /b 1
    )
)


echo.
echo Building project...
cmake --build "%BUILD_DIR%"

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable: %BUILD_DIR%\bin\
