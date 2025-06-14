# PowerShell script to setup and build the project
param(
    [string]$Action = "build"
)

# Function to find Visual Studio installation
function Find-VisualStudio {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($vsPath) {
            return "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
        }
    }
    
    # Fallback paths
    $fallbackPaths = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    )
    
    foreach ($path in $fallbackPaths) {
        if (Test-Path $path) {
            return $path
        }
    }
    
    return $null
}

# Check if we're in a VS environment
$inVSEnv = $env:VCINSTALLDIR -ne $null

if (-not $inVSEnv) {
    $vsSetupScript = Find-VisualStudio
    if (-not $vsSetupScript) {
        Write-Error "Visual Studio 2022 not found. Please install Visual Studio 2022 or run from Developer PowerShell."
        exit 1
    }
    
    Write-Host "Setting up Visual Studio environment..." -ForegroundColor Green
    cmd /c "`"$vsSetupScript`" && set" | ForEach-Object {
        if ($_ -match "^([^=]+)=(.*)") {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        }
    }
}

switch ($Action.ToLower()) {
    "configure" {
        Write-Host "Configuring project..." -ForegroundColor Green
        cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
    }
    "build" {
        Write-Host "Building project..." -ForegroundColor Green
        cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
        cmake --build build
    }
    "start" {
        Write-Host "Building and running project..." -ForegroundColor Green
        cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
        cmake --build build
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Running application..." -ForegroundColor Green
            & ".\build\bin\CppProject.exe" local
        }
    }
    "clean" {
        Write-Host "Cleaning build artifacts..." -ForegroundColor Green
        if (Test-Path "build") {
            Remove-Item -Recurse -Force "build"
        }
    }
    default {
        Write-Host "Usage: .\setup.ps1 [configure|build|start|clean]" -ForegroundColor Yellow
    }
}
