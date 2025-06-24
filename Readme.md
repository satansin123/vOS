# vOS - Virtual Embedded Operating System

**vOS** is a comprehensive simulation-based approach to learning and implementing embedded operating system concepts without requiring any physical hardware. This innovative project demonstrates how complex embedded systems can be fully simulated using modern C++ and standard desktop development tools.

## Simulation-Based Architecture

I am building vOS as a **pure software simulation** that replicates the behavior and functionality of real embedded systems. Instead of programming actual microcontrollers or embedded hardware, i am creating virtual representations of all hardware components:


**Hardware Abstraction Through Software(some have already been implemented while some are in progress):**
- **Memory-mapped I/O** simulation will be using `std::map` structures
- **Hardware interrupts** implentation will be as software callbacks and threading
- **Real-time clock** emulated with `std::chrono` for precise timing
- **Device registers** representation will be as virtual memory addresses
- **Peripheral devices** (UART, GPIO, Timers) simulation as C++ classes

## Progressive Development Approach

I am constructing vOS through **11 structured tasks** that build upon each other, creating a complete embedded operating system simulation[1]:

### Foundation Phase (Tasks 1-2) - **Currently Complete**
- **Kernel Bootstrap**: System initialization with singleton pattern and tick management
- **Cooperative Scheduler**: Task Control Blocks (TCBs) with round-robin execution and timer-based reactivation

### System Integration Phase (Tasks 3-5) - **In Progress**
- **Plugin System**: Dynamic driver loading through DLL architecture
- **Virtual Filesystem**: Unix-style `/dev/` device access
- **Command Line Interface**: System administration and debugging tools

### Advanced Features Phase (Tasks 6-11) - **Planned**
- **Interrupt Simulation**: Software-based IRQ handling with ISR management
- **Real-time Features**: Priority scheduling, deadline monitoring, and schedulability analysis
- **Communication Protocols**: UART, SPI, and wireless stack implementations
- **Security Systems**: Cryptography, secure boot, and audit logging

## Current Implementation Status

Your vOS kernel currently demonstrates a **fully functional cooperative multitasking system**[1]:

```cpp
// Real embedded-style task management
TCB backgroundLogger("BackgroundLogger", Priority::LOW, [](){
    uint64_t currentTick = Kernel::getTicks();
    logger.log(MessageType::INFO, "[Logger] System running smoothly");
}, 5); // 5-tick timer period

// Automatic scheduler integration
scheduler.registerTask(std::move(backgroundLogger));
```

**Key Simulation Features Working:**
- **100ms system tick** driving all operations
- **Task state management** (READY/RUNNING/WAITING)
- **Priority-based scheduling** with three levels
- **Timer-based task reactivation** for periodic execution
- **Thread-safe operations** throughout the system

## Educational Simulation Benefits

This simulation-based approach provides several advantages atleast for me:

**Accessibility**: No expensive development boards or hardware debuggers required
**Safety**: No risk of hardware damage during experimentation
**Rapid Prototyping**: Immediate testing and debugging without hardware setup delays
**Cost Effective**: Runs on any desktop computer with standard development tools
**Comprehensive Learning**: Covers all embedded concepts from kernel to security

