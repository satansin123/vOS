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
if exist "%BUILD_DIR%\bin\%PROJECT_NAME%.exe" (
    "%BUILD_DIR%\bin\%PROJECT_NAME%.exe" local
) else (
    echo Executable not found: %BUILD_DIR%\bin\%PROJECT_NAME%.exe
    exit /b 1
)
