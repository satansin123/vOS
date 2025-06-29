@echo off
setlocal enabledelayedexpansion

:: Dynamic path detection
set "PROJECT_ROOT=%~dp0.."
set "BUILD_DIR=%PROJECT_ROOT%\build"

echo === Running Application ===

:: Build first
call "%~dp0build.bat"
if errorlevel 1 (
    echo Build failed, cannot run application
    exit /b 1
)

:: Get project name dynamically
for %%I in ("%PROJECT_ROOT%") do set "PROJECT_NAME=%%~nxI"

echo.
echo Running application...
cd "%BUILD_DIR%\bin"
if exist "vOS.exe" (
    "vOS.exe" local
) else (
    echo Executable not found: %BUILD_DIR%\bin\vOS.exe
    exit /b 1
)
