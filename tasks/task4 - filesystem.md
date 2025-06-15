# 📁 Phase 4 – Virtual Device Filesystem

Here are the complete Virtual Device Filesystem tasks for your Virtual Embedded OS project, formatted in competitive programming style:

---

## **Problem 4.1: Abstract Device Base Class**

**Difficulty: Medium**

Design an abstract base class that defines the interface all hardware devices must implement, enabling polymorphic device access through a unified API.

**Problem Description:**
To create a unified device access layer, all hardware devices must implement a common interface. Your abstract `Device` class should define pure virtual methods for basic I/O operations while allowing device-specific implementations. This design enables the virtual filesystem to treat all devices uniformly regardless of their underlying hardware type.

**Requirements:**
- Define pure virtual `read()` method
- Define pure virtual `write(data)` method  
- Include device identification methods
- Support device status queries
- Enable polymorphic device access

**Input:** Device-specific implementation
**Output:** Unified device interface

**Example Interface:**
```cpp
class Device {
public:
    virtual ~Device() = default;
    virtual std::string read() = 0;
    virtual bool write(const std::string& data) = 0;
    virtual std::string getName() const = 0;
    virtual bool isReady() const = 0;
};
```

**Constraints:**
- All methods must be pure virtual
- Device names must be unique
- Thread-safe implementation required
- Support Unicode data for international text

---

## **Problem 4.2: Virtual Filesystem Device Registration**

**Difficulty: Medium**

Create a registration system that maps device objects to filesystem paths like `/dev/uart0`, enabling path-based device access.

**Problem Description:**
Unix-like systems access hardware devices through filesystem paths in the `/dev/` directory. Your VFS must maintain a mapping between device paths and device objects, validate path formats, prevent duplicate registrations, and provide efficient path-to-device resolution for I/O operations.

**Requirements:**
- Map device paths to Device* pointers
- Validate `/dev/` path format
- Prevent duplicate path registration
- Support device unregistration
- Provide path-to-device lookup functionality

**Input:** Device object and desired filesystem path
**Output:** Registration success status

**Example Registration:**
```cpp
bool success = vfs.register_device("/dev/uart0", uart_device);
if (success) {
    std::cout << "Device registered at /dev/uart0\n";
}
```

**Constraints:**
- Path format: `/dev/[a-zA-Z0-9_]+`
- Maximum path length: 64 characters
- Support up to 256 registered devices
- Case-sensitive path matching

---

## **Problem 4.3: Device Discovery and Listing**

**Difficulty: Easy**

Implement a function that lists all registered devices with their paths, current status, and basic information for system administration and debugging.

**Problem Description:**
System administrators need visibility into available hardware devices. Your listing function should enumerate all registered devices, query their current status, and present the information in a clear, formatted manner. This is essential for system diagnostics and device management.

**Requirements:**
- List all registered device paths
- Display device status (Ready/Busy/Error)
- Show device type or name
- Format output in readable columns
- Sort devices by path name

**Input:** VFS device registry
**Output:** Formatted device listing

**Example Device Listing:**
```
Registered Devices:
Path            Status    Type
/dev/gpio0      Ready     GPIO Controller
/dev/i2c1       Busy      I2C Bus Master  
/dev/uart0      Ready     UART Interface
/dev/uart1      Error     UART Interface
```

**Constraints:**
- Tabular output format required
- Status must reflect real-time device state
- Maximum display width: 80 characters
- Handle empty device registry gracefully

---

## **Problem 4.4: Virtual Filesystem Write Operations**

**Difficulty: Medium**

Implement the VFS write function that accepts a device path and data, locates the corresponding device, and executes the write operation with proper error handling.

**Problem Description:**
The VFS write operation is the primary interface for sending data to hardware devices. Your implementation must resolve the device path, validate the target device exists and is ready, perform the write operation, and handle various error conditions including device failures and invalid paths.

**Requirements:**
- Resolve device path to Device* pointer
- Validate device exists and is accessible
- Execute device write() method
- Handle write failures gracefully
- Return operation status and error details

**Input:** Device path and data to write
**Output:** Write operation success status

**Example Write Operation:**
```cpp
bool result = vfs.write("/dev/uart0", "Hello World!");
if (!result) {
    std::cout << "Write failed: Device not ready\n";
}
```

**Constraints:**
- Path resolution must be case-sensitive
- Data size limit: 4KB per write operation
- Write timeout: 5 seconds maximum
- Support binary and text data

---

## **Problem 4.5: Virtual Filesystem Read Operations**

**Difficulty: Medium**

Create the VFS read function that retrieves data from specified device paths, with support for blocking and non-blocking read modes.

**Problem Description:**
Device read operations must handle various scenarios including immediate data availability, blocking waits for data, and read timeouts. Your VFS read implementation should provide both blocking and non-blocking modes, handle empty reads gracefully, and return appropriate status information along with the data.

**Requirements:**
- Support blocking and non-blocking read modes
- Handle immediate data availability
- Implement read timeouts for blocking mode
- Return both data and operation status
- Handle device-specific read behaviors

**Input:** Device path and read mode configuration
**Output:** Read data and operation status

**Example Read Operation:**
```cpp
auto [data, success] = vfs.read("/dev/uart0", ReadMode::BLOCKING);
if (success) {
    std::cout << "Received: " << data << std::endl;
}
```

**Constraints:**
- Blocking timeout: configurable 1-30 seconds
- Non-blocking returns immediately
- Maximum read size: 4KB per operation
- Support empty read results

---

This completes Phase 4 of the Virtual Embedded OS project, providing a comprehensive virtual filesystem interface that enables unified access to all hardware devices through standard Unix-style device paths.