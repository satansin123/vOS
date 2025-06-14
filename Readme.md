# 🛠️ Professional C++ Project Template

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat&ake-3.15+-green.svg?style-Ready-blue.svg?style=flat&logo=dockerio/badge/VS%20Code-Configured-blue.svg?style=flat&logo=visual, containerized C++ development environment with CMake, Ninja, vcpkg, and full VS Code integration\*

[Getting Started](#-getting-started) -
[Commands](#-the-3-essential-commands) -
[Features](#-features) -
[Dependencies](#-managing-dependencies) -
[Contributing](#-contributing)

---

## 🚀 Getting Started

### Prerequisites

- [Docker](https://www.docker.com/get-started)
- [Visual Studio Code](https://code.visualstudio.com/)
- VS Code Extensions:
  - **Dev Containers** (`ms-vscode-remote.remote-containers`)
  - **C/C++** (`ms-vscode.cpptools`)
  - **CMake Tools** (`ms-vscode.cmake-tools`)

### Quick Setup

1. **Clone this template:**

   ```bash
   git clone https://github.com/yourusername/cpp-project-template.git
   cd cpp-project-template
   ```

2. **Open in VS Code:**

   ```bash
   code .
   ```

3. **Reopen in Container:**

   - Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
   - Type: `Dev Containers: Reopen in Container`
   - Wait for the container to build (first time only)

4. **You're ready to code!** 🎉

---

## ⚡ The 3 Essential Commands

| Action       | VS Code Shortcut                           | Description                            |
| ------------ | ------------------------------------------ | -------------------------------------- |
| **🔨 Build** | `Ctrl+Shift+B` → `Build`                   | Compiles your entire project           |
| **▶️ Run**   | `Ctrl+Shift+P` → `Tasks: Run Task` → `Run` | Executes your application              |
| **🐛 Debug** | `F5`                                       | Launches GDB debugger with breakpoints |

### Alternative: Command Line

```bash
# Inside the dev container
cmake --build build          # Build
./build/main                 # Run
gdb ./build/main            # Debug
```

---

## without using dev containers

```bash

# Configure the project
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build the project
cmake --build build

# Run the project
./build/main.exe

```

# Run the project

## 🏗️ Project Structure

```
cpp-project-template/
├── 📁 .devcontainer/
│   ├── 🐳 Dockerfile              # Development environment
│   └── ⚙️ devcontainer.json       # VS Code container config
├── 📁 .vscode/
│   ├── 🎯 launch.json             # Debug configuration
│   └── 📋 tasks.json              # Build & run tasks
├── 📁 src/                        # Source files (create as needed)
├── 📁 include/                    # Header files (create as needed)
├── 🛠️ CMakeLists.txt              # Build configuration
├── 📦 vcpkg.json                  # Package dependencies
├── 💻 main.cpp                    # Entry point
└── 📖 README.md                   # This file
```

---

## ✨ Features

### 🔧 Development Tools

- **CMake 4.0+** - Modern build system
- **Ninja** - Fast parallel builds
- **vcpkg** - Cross-platform package manager
- **GDB** - Full debugging support
- **Clang-Tidy** - Static analysis (optional)

### 🐳 Docker Integration

- **Isolated Environment** - No local setup required
- **Consistent Builds** - Same environment everywhere
- **VS Code Integration** - Seamless development experience

### 📝 VS Code Features

- **IntelliSense** - Auto-completion and syntax highlighting
- **Debugging** - Breakpoints, variable inspection, call stack
- **Tasks** - One-click build and run
- **Extensions** - Pre-configured C++ development tools

---

## 📦 Managing Dependencies

### Adding a New Library

1. **Add to `vcpkg.json`:**

   ```json
   {
     "name": "my-cpp-project",
     "version-string": "1.0.0",
     "dependencies": ["fmt", "spdlog", "catch2"]
   }
   ```

2. **Use in `CMakeLists.txt`:**

   ```cmake
   find_package(fmt CONFIG REQUIRED)
   find_package(spdlog CONFIG REQUIRED)

   target_link_libraries(main PRIVATE fmt::fmt spdlog::spdlog)
   ```

3. **Rebuild the container** or run:
   ```bash
   vcpkg install
   cmake --build build
   ```

---

## 🛠️ Manual Docker Usage

If you prefer command line over VS Code:

```bash
# Build the development image
docker build -t cpp-dev-env .devcontainer

# Run interactive development container
docker run -it --rm \
  -v $(pwd):/workspace \
  -w /workspace \
  cpp-dev-env bash

# Inside container: configure and build
cmake -S . -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

cmake --build build
./build/main
```

---

## 🚀 Customization

### Project Setup

1. **Rename** your project in `CMakeLists.txt`:

   ```cmake
   project(YourProjectName VERSION 1.0.0)
   ```

2. **Update** `vcpkg.json` with your project details:

   ```json
   {
     "name": "your-project-name",
     "version-string": "1.0.0"
   }
   ```

3. **Replace** `main.cpp` with your actual source code

### Build Configuration

- **Debug builds**: `cmake -DCMAKE_BUILD_TYPE=Debug`
- **Release builds**: `cmake -DCMAKE_BUILD_TYPE=Release`
- **Custom flags**: Modify `CMakeLists.txt`

---
