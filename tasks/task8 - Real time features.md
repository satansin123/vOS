# ⚡ Phase 8 – Real-Time Features

Here are the complete Real-Time Features tasks for your Virtual Embedded OS project, formatted in competitive programming style:

---

## **Problem 8.1: Priority-Based Preemptive Scheduler**

**Difficulty: Expert**

Upgrade your cooperative scheduler to support preemptive multitasking with priority levels. Higher priority tasks must interrupt lower priority tasks immediately when they become ready.

**Problem Description:**
Real-time embedded systems require deterministic task scheduling based on priority levels. Your scheduler must support at least 8 priority levels, implement priority inheritance to prevent priority inversion, and provide time-slice management for tasks of equal priority. The scheduler must guarantee that the highest priority ready task always runs within a bounded time.

**Requirements:**
- Support 8 priority levels (0 = highest, 7 = lowest)
- Implement priority inheritance protocol
- Time-slice rotation for equal priority tasks
- Interrupt lower priority tasks immediately
- Track worst-case scheduling latency

**Input:** Tasks with assigned priorities
**Output:** Scheduled task execution with timing guarantees

**Example Priority Behavior:**
```
[Priority 0] Critical_Task running
[Priority 2] Normal_Task preempted by Critical_Task
[Priority 0] Critical_Task completed
[Priority 2] Normal_Task resumed
```

**Constraints:**
- Maximum scheduling latency: 100 microseconds
- Priority inheritance must prevent priority inversion
- Support up to 64 concurrent tasks
- Real-time guarantees for priorities 0-3

---

## **Problem 8.2: Deadline Monitoring System**

**Difficulty: Expert**

Implement a deadline monitoring system that tracks task execution times and triggers alerts when tasks miss their deadlines. This is critical for real-time system reliability.

**Problem Description:**
Real-time systems must meet timing deadlines to function correctly. Your monitoring system must track each task's execution start time, deadline, and completion status. When a task exceeds its deadline, the system must log the violation, optionally terminate the task, and potentially trigger system recovery procedures.

**Requirements:**
- Assign deadlines to individual tasks
- Monitor execution time vs. deadline
- Log deadline violations with detailed timing info
- Support deadline miss recovery strategies
- Provide real-time deadline status reporting

**Input:** Task deadlines and execution timing
**Output:** Deadline compliance reports and violation alerts

**Example Monitoring Output:**
```
[DEADLINE] Task sensor_read: 45ms/50ms - OK
[DEADLINE] Task data_process: 75ms/60ms - VIOLATION!
[DEADLINE] Triggering recovery for data_process
```

**Constraints:**
- Deadline resolution: 1 millisecond
- Support deadline ranges from 1ms to 60 seconds
- Maximum 5% timing overhead for monitoring
- Automatic recovery must complete within deadline period

---

## **Problem 8.3: Resource Locking with Priority Inheritance**

**Difficulty: Expert**

Implement mutex locks with priority inheritance to prevent priority inversion when multiple tasks compete for shared resources.

**Problem Description:**
When high-priority tasks wait for resources held by low-priority tasks, priority inversion can occur, violating real-time guarantees. Your locking mechanism must temporarily boost the priority of resource-holding tasks to match the highest waiting task, ensuring bounded blocking times and maintaining system responsiveness.

**Requirements:**
- Implement priority inheritance protocol
- Support nested lock acquisition
- Track lock ownership and waiting queues
- Automatic priority restoration on unlock
- Deadlock detection and prevention

**Input:** Resource lock requests with task priorities
**Output:** Lock acquisition status and priority adjustments

**Example Priority Inheritance:**
```
[LOCK] Low_Task (P7) acquires Resource_A
[LOCK] High_Task (P1) waits for Resource_A
[INHERIT] Low_Task priority boosted: P7→P1
[UNLOCK] Low_Task releases Resource_A, priority restored: P1→P7
[LOCK] High_Task acquires Resource_A
```

**Constraints:**
- Maximum inheritance chain depth: 8 levels
- Lock acquisition timeout: configurable per task
- Deadlock detection within 100ms
- Support up to 32 simultaneous locks

---

## **Problem 8.4: Rate Monotonic Scheduling**

**Difficulty: Expert**

Implement Rate Monotonic Scheduling (RMS) algorithm that assigns priorities based on task periods, with schedulability analysis to ensure all deadlines can be met.

**Problem Description:**
Rate Monotonic Scheduling is a fundamental real-time scheduling algorithm where tasks with shorter periods receive higher priorities. Your implementation must assign priorities automatically based on task periods, perform schedulability analysis using the RMS theorem, detect scheduling conflicts before they occur, and provide utilization bounds checking to guarantee system feasibility.

**Requirements:**
- Automatically assign priorities based on task periods (shorter period = higher priority)
- Implement Liu & Layland schedulability test
- Calculate system utilization and compare against theoretical bounds
- Detect and report scheduling conflicts
- Support periodic task creation with period and execution time specification

**Input:** Periodic tasks with periods and execution times
**Output:** Priority assignments and schedulability analysis results

**Example RMS Analysis:**
```
[RMS] Analyzing task set:
Task A: period=50ms, execution=15ms, priority=0 (highest)
Task B: period=100ms, execution=20ms, priority=1
Task C: period=200ms, execution=30ms, priority=2

[RMS] System utilization: 62.5%
[RMS] RMS bound for 3 tasks: 77.9%
[RMS] Schedulability: FEASIBLE - All deadlines guaranteed
```

**Constraints:**
- Support minimum 16 periodic tasks
- Period range: 1ms to 10 seconds
- Utilization calculation accuracy: 0.1%
- Schedulability analysis time: <100ms

---

## **Problem 8.5: Earliest Deadline First (EDF) Scheduler**

**Difficulty: Expert**

Develop an Earliest Deadline First scheduler that dynamically assigns priorities based on absolute deadlines, providing optimal scheduling for periodic and aperiodic tasks.

**Problem Description:**
EDF scheduling assigns the highest priority to the task with the earliest absolute deadline. Your implementation must calculate absolute deadlines for periodic tasks, support both periodic and aperiodic (sporadic) tasks, dynamically update priorities as deadlines approach, perform online schedulability testing, and handle deadline misses gracefully with recovery mechanisms.

**Requirements:**
- Calculate absolute deadlines for periodic and aperiodic tasks
- Dynamically assign priorities based on earliest deadline
- Support mixed task sets (periodic + aperiodic)
- Implement online schedulability testing
- Handle deadline misses with configurable recovery policies

**Input:** Mixed task set with periods, deadlines, and execution times
**Output:** Dynamic priority assignments and deadline management

**Example EDF Operation:**
```
[EDF] Current time: 150ms
Task priorities by deadline:
1. Task_A (deadline: 175ms) - RUNNING
2. Task_C (deadline: 200ms) - READY  
3. Task_B (deadline: 250ms) - READY

[EDF] Time: 175ms - Task_A completed successfully
[EDF] Task_C now highest priority, switching context
```

**Constraints:**
- Deadline calculation accuracy: 1ms resolution
- Priority updates must complete within 50 microseconds
- Support up to 32 tasks in ready queue
- Online schedulability test overhead: <2% CPU

---

This completes Phase 8 of the Virtual Embedded OS project, providing comprehensive real-time scheduling capabilities essential for time-critical embedded system applications.