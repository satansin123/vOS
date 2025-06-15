## 🧠 Phase 2 – Cooperative Scheduler

### **Problem 2.1: Task Control Block Design**

**Difficulty: Medium**

Design the fundamental data structure that represents a schedulable task in your operating system. This structure must support state management, priority handling, and callback execution[1].

**Problem Description:**
The Task Control Block (TCB) is the core data structure in any operating system scheduler. Your implementation must track task identity, current execution state, scheduling priority, and the actual work to be performed. The design should support common task states like READY, RUNNING, and WAITING, while providing type-safe callback execution[1].

**Requirements:**
- Task identification (name/ID)
- State management (READY, RUNNING, WAITING)
- Priority levels for scheduling
- Executable callback storage
- State transition validation

**Input:** Task parameters (name, priority, callback)
**Output:** Initialized task structure

**Example Task Creation:**
```cpp
Task logger_task("Logger", Priority::MEDIUM, [](){
    std::cout  for callbacks
- Enum-based state management
- Thread-safe state transitions
- Unique task names required

---

### **Problem 2.2: Dynamic Task Registry**

**Difficulty: Medium**

Implement a registration system that allows tasks to be added to the scheduler dynamically during runtime. The system must prevent duplicate registrations and validate task parameters[1].

**Problem Description:**
Modern operating systems support dynamic task creation. Your registry must maintain a collection of all registered tasks, ensure unique naming, and provide efficient lookup capabilities. The system should validate task parameters before registration and provide meaningful error messages for invalid requests[1].

**Requirements:**  
- Dynamic task registration API
- Duplicate name prevention
- Task parameter validation
- Efficient task lookup
- Registration status reporting

**Input:** Task objects for registration
**Output:** Registration success/failure status

**Example Usage:**
```cpp
bool success = scheduler.registerTask(myTask);
if (!success) {
    std::cout << "Task registration failed\n";
}
```

**Constraints:**
- No duplicate task names
- Validate all task parameters
- Return clear error codes
- Efficient storage structure

---

### **Problem 2.3: Round-Robin Task Execution**

**Difficulty: Hard**

Create a cooperative scheduler that executes all READY tasks in round-robin fashion. Each task runs to completion before the next task begins, with proper state management throughout the execution cycle[1].

**Problem Description:**
Cooperative scheduling relies on tasks voluntarily yielding control. Your scheduler must iterate through all READY tasks, execute them one at a time, and manage state transitions properly. Tasks move from READY → RUNNING → WAITING states, with the scheduler coordinating these transitions and ensuring fair execution[1].

**Requirements:**
- Round-robin task selection
- Proper state transitions
- Single-task execution guarantee  
- Integration with system ticks
- Fair scheduling algorithm

**Input:** Collection of registered tasks
**Output:** Executed tasks with state updates

**Example Execution Flow:**
```
[Scheduler] Running Task A (READY → RUNNING)
[Task A] Executing work...
[Scheduler] Task A complete (RUNNING → WAITING)
[Scheduler] Running Task B (READY → RUNNING)  
[Task B] Executing work...
[Scheduler] Task B complete (RUNNING → WAITING)
```

**Constraints:**
- One task executes at a time
- Complete state transition tracking
- Fair round-robin selection
- Integration with tick system

---

### **Problem 2.4: Task Wake-up Timer System**

**Difficulty: Hard**

Implement a timing mechanism that automatically transitions WAITING tasks back to READY state after a specified number of system ticks[1].

**Problem Description:**
Cooperative systems need a way to reactivate sleeping tasks. Your timer system must track individual wait periods for each task, decrement counters with each system tick, and automatically transition tasks from WAITING to READY when their timer expires. This creates a periodic execution pattern essential for background tasks[1].

**Requirements:**
- Per-task wait timer tracking
- Automatic WAITING → READY transitions
- Tick-based countdown system
- Configurable wait periods
- Timer reset on task completion

**Input:** Task wait duration configurations
**Output:** Automatic task state transitions

**Example Timer Behavior:**
```
[Timer] Task A waiting for 5 ticks
[Tick 1] Task A: 4 ticks remaining  
[Tick 2] Task A: 3 ticks remaining
[Tick 3] Task A: 2 ticks remaining
[Tick 4] Task A: 1 tick remaining
[Tick 5] Task A: WAITING → READY
```

**Constraints:**
- Accurate tick counting
- Per-task timer storage
- Automatic state transitions
- Timer configuration support

---

### **Problem 2.5: Background Logger Task Implementation**

**Difficulty: Easy**

Create a background task that demonstrates the scheduler functionality by logging periodic heartbeat messages every 5 system ticks[1].

**Problem Description:**
To verify your scheduler works correctly, implement a logger task that outputs status messages at regular intervals. This task should count system ticks internally and produce output every 5 ticks, then wait before becoming ready again. This provides visible proof that task scheduling and wake-up timers function properly[1].

**Requirements:**
- Log message every 5 ticks
- Internal tick counting
- Automatic scheduling cycle
- Clear output formatting
- Integration with timer system

**Input:** System tick events  
**Output:** Periodic log messages

**Example Output:**
```
[Tick 5] [Logger] System running smoothly
[Tick 10] [Logger] System running smoothly  
[Tick 15] [Logger] System running smoothly
[Tick 20] [Logger] System running smoothly
```

**Constraints:**
- Exactly every 5 ticks
- Consistent message format
- Self-managing tick counter
- Proper scheduler integration

---