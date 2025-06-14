@echo off
setlocal

:: Dynamic path detection
set "PROJECT_ROOT=%~dp0.."
set "BUILD_DIR=%PROJECT_ROOT%\build"
set "DEBUG_BUILD_DIR=%PROJECT_ROOT%\build_debug"

echo === Cleaning Build Artifacts ===
echo Project Root: %PROJECT_ROOT%

if exist "%BUILD_DIR%" (
    echo Removing: %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%"
)

if exist "%DEBUG_BUILD_DIR%" (
    echo Removing: %DEBUG_BUILD_DIR%
    rmdir /s /q "%DEBUG_BUILD_DIR%"
)

echo Clean completed!
