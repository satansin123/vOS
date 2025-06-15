# 💻 Phase 5 – Shell (CLI)

Here are the complete Shell (Command Line Interface) tasks for your Virtual Embedded OS project, formatted in competitive programming style:

---

## **Problem 5.1: Interactive Command Shell Implementation**

**Difficulty: Medium**

Build a Read-Eval-Print Loop (REPL) command shell that accepts user input, parses commands, executes them, and displays results in an interactive terminal interface.

**Problem Description:**
Every operating system needs a command-line interface for system administration and debugging. Your shell must provide a prompt, accept user input, parse command-line arguments, dispatch commands to appropriate handlers, and display results or error messages. The shell should handle common input scenarios including empty commands, invalid syntax, and command history.

**Requirements:**
- Display command prompt and accept user input
- Parse command line into command and arguments
- Dispatch commands to registered handlers
- Display command results and error messages
- Handle empty input and invalid commands gracefully

**Input:** User keyboard input
**Output:** Command execution results

**Example Shell Session:**
```
vOS> help
Available commands:
  help - Show this help message
  list - List registered devices
  exit - Shutdown system

vOS> list
/dev/uart0 - Ready
/dev/i2c1  - Idle

vOS> invalid_command
Error: Unknown command 'invalid_command'
```

**Constraints:**
- Prompt format: "vOS> "
- Maximum command length: 256 characters
- Support minimum 10 built-in commands
- Command parsing must handle quoted arguments

---

## **Problem 5.2: Comprehensive Help System**

**Difficulty: Easy**

Create a help command that displays all available commands with their syntax, parameters, and descriptions in a user-friendly format.

**Problem Description:**
Users need comprehensive documentation for available shell commands. Your help system should maintain a registry of all commands with their descriptions, display them in an organized format, and optionally provide detailed help for individual commands when requested.

**Requirements:**
- Display all available commands with descriptions
- Show command syntax and required parameters
- Support detailed help for individual commands
- Format output in readable columns
- Include usage examples for complex commands

**Input:** Optional specific command name
**Output:** Formatted help documentation

**Example Help Output:**
```
vOS Commands:
help [command]     - Show help for all commands or specific command
list               - List all registered devices  
write   - Write data to device
read         - Read data from device
exit               - Shutdown the operating system

Use 'help ' for detailed information.
```

**Constraints:**
- Help text must fit in 80-character terminal width
- Support both general help and command-specific help
- Commands must be sorted alphabetically
- Include usage examples where appropriate

---

## **Problem 5.3: Device I/O Commands**

**Difficulty: Medium**

Implement shell commands for reading from and writing to virtual devices through their filesystem paths, with proper argument validation and error handling.

**Problem Description:**
System administrators need command-line access to hardware devices for testing and debugging. Your implementation must provide `read` and `write` commands that interface with the VFS, validate device paths and data formats, handle I/O errors gracefully, and provide clear feedback about operation results.

**Requirements:**
- Implement `write  ` command
- Implement `read ` command  
- Validate device paths and accessibility
- Handle I/O errors with descriptive messages
- Support both text and hexadecimal data formats

**Input:** Device path and optional data
**Output:** I/O operation results or error messages

**Example I/O Commands:**
```
vOS> write /dev/uart0 "Hello Device!"
Write successful: 13 bytes written to /dev/uart0

vOS> read /dev/uart0
Read successful: "ACK"

vOS> write /dev/invalid "test"
Error: Device /dev/invalid not found
```

**Constraints:**
- Validate device paths before I/O operations
- Maximum write data size: 1KB per command
- Support escape sequences in quoted strings
- Display byte counts for successful operations

---

## **Problem 5.4: Error Handling and Command Suggestions**

**Difficulty: Medium**

Enhance the shell with intelligent error handling that provides helpful suggestions when users enter invalid commands or incorrect syntax.

**Problem Description:**
User experience improves significantly with helpful error messages and command suggestions. Your error handling system should detect common mistakes like typos in command names, missing arguments, and invalid syntax. When possible, suggest corrections or similar commands to help users accomplish their intended tasks.

**Requirements:**
- Detect and handle invalid command names
- Suggest similar commands for typos (edit distance)
- Validate command argument counts and types
- Provide helpful error messages with correction hints
- Handle special cases like empty input gracefully

**Input:** Invalid user commands or syntax
**Output:** Error messages with helpful suggestions

**Example Error Handling:**
```
vOS> hep
Error: Unknown command 'hep'. Did you mean 'help'?

vOS> write /dev/uart0
Error: 'write' command requires 2 arguments:  
Usage: write  

vOS> read
Error: 'read' command requires 1 argument: 
Usage: read 
```

**Constraints:**
- Use Levenshtein distance for command suggestions
- Suggest commands with edit distance ≤ 2
- Provide usage examples in error messages
- Handle partial commands and incomplete input

---

## **Problem 5.5: Dynamic Task Scheduling via CLI**

**Difficulty: Hard**

Add shell commands that allow users to create, schedule, and manage kernel tasks dynamically during runtime.

**Problem Description:**
Advanced system administration requires the ability to create and manage tasks without recompiling the kernel. Your CLI task management system must allow users to schedule new tasks with custom callbacks, set task priorities and timing parameters, monitor task execution status, and provide task lifecycle management including pausing and termination.

**Requirements:**
- Implement `schedule  ` command
- Support task parameter configuration
- Provide task status monitoring commands
- Allow task pause/resume/terminate operations
- Integrate with existing cooperative scheduler

**Input:** Task creation parameters and management commands
**Output:** Task scheduling status and execution feedback

**Example Task Management:**
```
vOS> schedule HeartbeatTask 5
Task 'HeartbeatTask' scheduled with priority 5

vOS> tasks
Active Tasks:
Name            Status    Priority  Last Run
HeartbeatTask   READY     5         12 ticks ago
LoggerTask      WAITING   3         2 ticks ago

vOS> pause HeartbeatTask
Task 'HeartbeatTask' paused

vOS> resume HeartbeatTask  
Task 'HeartbeatTask' resumed
```

**Constraints:**
- Support task priorities 1-10 (1 = highest)
- Task names must be unique and 1-32 characters
- Maximum 16 user-scheduled tasks simultaneously
- Provide real-time task status updates

---

This completes Phase 5 of the Virtual Embedded OS project, providing a comprehensive command-line interface that enables system administration, device interaction, and dynamic task management through an intuitive shell environment.