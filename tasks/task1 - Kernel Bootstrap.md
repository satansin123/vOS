## 🧩 Phase 1 – Kernel Bootstrap

### **Problem 1.1: System Boot Sequence**

**Difficulty: Easy**

You are tasked with implementing the boot sequence for a virtual embedded operating system. The system must initialize all core subsystems, establish a global tick counter, and announce its successful startup to the world[1].

**Problem Description:**
In embedded systems, the boot process is critical. Your kernel must maintain a global state that tracks system uptime through tick counts, manages a registry of connected devices, and provides a clear indication that the system has successfully started. The kernel should be designed as a singleton to ensure only one instance manages the entire system[1].

**Requirements:**
- Implement a `Kernel` class with static tick counter
- Create a global device registry 
- Display boot message with kernel name and version
- Initialize all core subsystems in proper order

**Input:** None (system initialization)
**Output:** Boot message displaying kernel information

**Example Output:**
```
[BOOT] vOS Kernel v1.0 - Initializing...
[BOOT] Device registry initialized
[BOOT] System clock initialized
[BOOT] vOS ready!
```

**Constraints:**
- Kernel must be singleton pattern
- Tick counter starts at 0
- Boot message must include version info
- All subsystems must initialize successfully

---

### **Problem 1.2: Real-Time Clock Simulation**

**Difficulty: Medium**

Design a real-time clock system that generates periodic interrupts every 100 milliseconds. This forms the heartbeat of your operating system and drives all time-based operations including task scheduling[1].

**Problem Description:**
Every operating system needs a reliable time source. Your task is to create an asynchronous tick generator that increments a global counter every 100ms. This tick system will later be used by the scheduler to manage task execution and system timing. The implementation must not block other system operations[1].

**Requirements:**
- Generate ticks every 100ms precisely
- Increment global tick counter atomically
- Run asynchronously without blocking main thread
- Provide tick() method for manual triggering

**Input:** None (continuous operation)
**Output:** System ticks at 100ms intervals

**Example Execution:**
```
Tick 1 at 100ms
Tick 2 at 200ms  
Tick 3 at 300ms
...
```

**Constraints:**
- Tick interval: exactly 100ms
- Must use std::chrono for timing
- Thread-safe tick counter updates
- Non-blocking operation

---

### **Problem 1.3: System Heartbeat Logger**

**Difficulty: Easy** 

Implement a logging system that outputs the current system tick count with each heartbeat. This provides visual confirmation that your kernel is alive and processing time correctly[1].

**Problem Description:**
Debugging embedded systems often requires visible indicators of system health. Your logging system must output a formatted message showing the current tick count each time the system clock fires. This creates an audit trail of system activity and helps identify timing issues[1].

**Requirements:**
- Print tick count with each system tick
- Use consistent formatting for logs
- Ensure thread-safe logging operations
- Integrate with existing tick system

**Input:** System tick events
**Output:** Formatted log messages

**Example Output:**
```
[Tick 1] System heartbeat
[Tick 2] System heartbeat  
[Tick 3] System heartbeat
[Tick 4] System heartbeat
```

**Constraints:**
- Format: "[Tick N] System heartbeat"
- Must be thread-safe
- No missed ticks in output
- Synchronized with tick counter

---

### **Problem 1.4: Graceful Shutdown Protocol**

**Difficulty: Medium**

Create a shutdown mechanism that monitors for user input while allowing the system to continue normal operation. When "exit" is detected, initiate a clean shutdown sequence that stops all subsystems properly[1].

**Problem Description:**
Embedded systems must handle shutdown gracefully to prevent data corruption and ensure proper cleanup. Your solution must monitor user input in a non-blocking way while the system continues its normal tick operations. Upon receiving the shutdown command, all subsystems must be stopped in reverse initialization order[1].

**Requirements:**
- Monitor for "exit" command non-blocking  
- Continue system ticks during monitoring
- Clean shutdown of all subsystems
- Proper resource cleanup

**Input:** User keyboard input
**Output:** Shutdown sequence messages

**Example Interaction:**
```
[Tick 15] System heartbeat
[Tick 16] System heartbeat
> exit
[SHUTDOWN] Stopping system ticks...
[SHUTDOWN] Cleaning up devices...
[SHUTDOWN] vOS shutdown complete
```

**Constraints:**
- Non-blocking input detection
- Complete cleanup required
- Graceful subsystem shutdown
- No resource leaks

---
