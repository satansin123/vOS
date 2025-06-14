@echo off
setlocal

REM Build first
call build.bat
if errorlevel 1 (
    echo Build failed, cannot run
    exit /b 1
)

REM Run the application
echo Running application...
"build\bin\CppProject.exe" local
