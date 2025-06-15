# 🏆 Virtual Embedded OS (vOS) - Competitive Programming Style Task Sheet (Continued)

Continuing from **Task 2.5** with the remaining phases in detailed LeetCode/Codeforces format.

---

## 🔌 Phase 3 – Plugin System (Driver DLLs)

---

### **Problem 3.1: Driver Interface Definition**

**Difficulty: Medium**

You need to create a standardized interface for hardware drivers that can be loaded as dynamic libraries (DLLs). This interface must be C-compatible to ensure cross-compiler compatibility and provide essential driver functions.

**Problem Description:**
Embedded systems often support modular hardware drivers that can be loaded at runtime. Your task is to define a clean C interface that any driver DLL must implement. This interface should provide driver identification, initialization, and cleanup functions while maintaining binary compatibility across different compilers and platforms.

**Requirements:**
- Define C-compatible function signatures using `extern "C"`
- Include driver identification function (`driver_name()`)
- Include driver initialization function (`driver_init()`)
- Include optional cleanup function (`driver_cleanup()`)
- Ensure no C++ symbols leak across DLL boundaries

**Input:** Driver DLL implementation
**Output:** Standardized driver interface

**Example Interface:**
```c
extern "C" {
    const char* driver_name();
    bool driver_init();
    void driver_cleanup();
}
```

**Constraints:**
- Function names must be standardized across all drivers
- Return types must be simple C types only
- No C++ exceptions across DLL boundaries
- Driver name must be null-terminated string

---

### **Problem 3.2: Dynamic Library Loading System**

**Difficulty: Hard**

Implement a runtime DLL loading system that can load driver libraries, extract required function pointers, and handle loading failures gracefully.

**Problem Description:**
Your kernel must dynamically load driver DLLs during runtime without requiring recompilation. The system must locate the DLL file, load it into memory, resolve all required function symbols, and store function pointers for later use. Error handling is critical since DLL loading can fail for various reasons including missing files, symbol resolution failures, or incompatible architectures.

**Requirements:**
- Load DLL using platform-specific APIs (LoadLibraryA on Windows)
- Extract and validate required function pointers
- Handle missing DLL files gracefully
- Handle missing function symbols gracefully
- Store loaded drivers in a registry for management

**Input:** DLL file path and required function names
**Output:** Loaded driver registry entry or error status

**Example Loading Process:**
```
[LOADER] Loading driver: drivers/uart.dll
[LOADER] Resolving symbol: driver_name... OK
[LOADER] Resolving symbol: driver_init... OK
[LOADER] Driver uart.dll loaded successfully
```

**Constraints:**
- Must work on Windows platform (LoadLibraryA/GetProcAddress)
- Handle DLL loading timeouts (5 second maximum)
- Support driver unloading for cleanup
- Maximum 32 drivers can be loaded simultaneously

---

### **Problem 3.3: Driver Initialization and Registration**

**Difficulty: Medium**

After loading a driver DLL, execute its initialization function and register the driver with the kernel's device registry.

**Problem Description:**
Once a driver DLL is successfully loaded, its `driver_init()` function must be called to perform hardware-specific initialization. The driver should then be registered in the kernel's active device list using the name returned by `driver_name()`. This registration process must handle initialization failures and provide clear error reporting.

**Requirements:**
- Call `driver_init()` after successful DLL loading
- Retrieve driver name using `driver_name()` function
- Register driver in kernel device registry
- Handle initialization failures gracefully
- Validate driver name uniqueness

**Input:** Loaded driver function pointers
**Output:** Registered device entry or initialization failure

**Example Initialization:**
```
[INIT] Calling driver_init() for uart.dll...
[INIT] Driver reports name: "UART0"
[INIT] Registering device: UART0
[INIT] Driver UART0 initialization complete
```

**Constraints:**
- Driver names must be unique within the system
- Initialization timeout: 2 seconds maximum
- Failed drivers must be unloaded automatically
- Driver name length: 1-32 characters

---

### **Problem 3.4: Bulk Driver Loading from Directory**

**Difficulty: Medium**

Implement a system that automatically discovers and loads all compatible driver DLLs from a specified directory during system startup.

**Problem Description:**
Production embedded systems typically load multiple drivers automatically during boot. Your implementation must scan the `drivers/` directory, identify valid DLL files, attempt to load each one, and report the loading results. The system should continue loading other drivers even if some fail, providing a complete summary of the loading process.

**Requirements:**
- Scan `drivers/` directory for `.dll` files
- Attempt to load each discovered driver
- Skip non-DLL files without errors
- Report loading success/failure for each driver
- Provide summary statistics after bulk loading

**Input:** Directory path containing driver DLLs
**Output:** Loading summary with success/failure counts

**Example Bulk Loading:**
```
[BULK] Scanning drivers/ directory...
[BULK] Found 4 DLL files
[BULK] Loading uart.dll... SUCCESS
[BULK] Loading i2c.dll... SUCCESS  
[BULK] Loading spi.dll... FAILED (missing symbol)
[BULK] Loading gpio.dll... SUCCESS
[BULK] Bulk loading complete: 3/4 drivers loaded
```

**Constraints:**
- Use C++17 `` for directory scanning
- Loading timeout: 30 seconds total for all drivers
- Skip hidden files and subdirectories
- Generate detailed error log for failed drivers

---

