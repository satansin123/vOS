#include <chrono>
#include <iostream>
#include <thread>
#include "kernel/Kernel.h"
#include "kernel/DeviceRegistry.h"
#include "kernel/Clock.h"
#include "kernel/Logger.h"
#include "kernel/InputMonitor.h"
#include "scheduler/TCB.h"
#include "scheduler/Scheduler.h"

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
    #define isatty _isatty
    #define fileno _fileno
#else
    #include <unistd.h>
#endif

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize the kernel (Task 2 foundation)
    auto& kernel = Kernel::getInstance();
    
    if (!kernel.initialize()) {
        cerr << "Failed to initialise kernel!" << endl;
        return 1;
    }

    auto& logger = kernel.getLogger();
    auto& scheduler = kernel.getScheduler();

    // System startup
    logger.log(MessageType::HEADER, "vOS - Virtual Operating System");
    logger.log(MessageType::INFO, "Running in: " + string(argc > 1 ? argv[1] : "local") + " environment");
    logger.log(MessageType::HEADER, "Task 2: Cooperative Scheduler - Complete Implementation");

    // ================================================================
    // Task 2.1: TCB (Task Control Block) Implementation Demo
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2.1: TCB Implementation");
    
    TCB testTask("TestLogger", Priority::MEDIUM, [&logger]() {
        logger.log(MessageType::INFO, "Hello from test task!");
    }, 50);

    logger.log(MessageType::INFO, "TCB Created: " + testTask.getName() + " (ID: " + to_string(testTask.getId()) + ")");
    logger.log(MessageType::INFO, "Priority: " + testTask.getPriorityString());
    logger.log(MessageType::INFO, "Initial State: " + testTask.getStateString());

    // Test state transitions
    testTask.setState(TaskState::RUNNING);
    logger.log(MessageType::INFO, "State transition to RUNNING: " + testTask.getStateString());
    testTask.executeTask();
    testTask.setState(TaskState::WAITING);
    logger.log(MessageType::INFO, "State transition to WAITING: " + testTask.getStateString());
    logger.log(MessageType::INFO, "Task 2.1 Complete: TCB with state management and callback execution");

    // ================================================================
    // Task 2.2: Dynamic Task Registry Implementation
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2.2: Dynamic Task Registry");
    
    // Create tasks for comprehensive testing
    auto backgroundLogger = make_unique<TCB>("BackgroundLogger", Priority::LOW, [&logger]() {
        uint64_t currentTick = Kernel::getTicks();
        logger.log(MessageType::INFO, "[Logger] System running smoothly");
        logger.log(MessageType::INFO, "[Logger] Scheduler operational at tick " + to_string(currentTick));
        
        // Demonstrate scheduler integration
        auto& scheduler = Kernel::getInstance().getScheduler();
        int readyTasks = scheduler.getTaskCountByState(TaskState::READY);
        int waitingTasks = scheduler.getTaskCountByState(TaskState::WAITING);
        logger.log(MessageType::INFO, "[Logger] Tasks: " + to_string(readyTasks) + " ready, " + 
                   to_string(waitingTasks) + " waiting");
    }, 5); // Execute every 5 ticks (Task 2.5 requirement)

    auto systemMonitor = make_unique<TCB>("SystemMonitor", Priority::HIGH, [&logger]() {
        logger.log(MessageType::INFO, "System monitoring task executed");
        logger.log(MessageType::INFO, "System health: All subsystems operational");
    }, 3); // Execute every 3 ticks

    auto dataProcessor = make_unique<TCB>("DataProcessor", Priority::MEDIUM, [&logger]() {
        logger.log(MessageType::INFO, "Data processing task executed");
        static int processedItems = 0;
        processedItems += 10;
        logger.log(MessageType::INFO, "Processed " + to_string(processedItems) + " data items");
    }, 4); // Execute every 4 ticks

    // Register tasks (Task 2.2: Dynamic registration)
    bool result1 = scheduler.registerTask(std::move(backgroundLogger));
    bool result2 = scheduler.registerTask(std::move(systemMonitor));
    bool result3 = scheduler.registerTask(std::move(dataProcessor));

    logger.log(MessageType::INFO, "BackgroundLogger registration: " + string(result1 ? "SUCCESS" : "FAILED"));
    logger.log(MessageType::INFO, "SystemMonitor registration: " + string(result2 ? "SUCCESS" : "FAILED"));
    logger.log(MessageType::INFO, "DataProcessor registration: " + string(result3 ? "SUCCESS" : "FAILED"));

    // Test duplicate prevention
    auto duplicate = make_unique<TCB>("BackgroundLogger", Priority::MEDIUM, []() {});
    if (!scheduler.registerTask(std::move(duplicate))) {
        logger.log(MessageType::INFO, "Duplicate prevention working correctly");
    }

    // Display registry status
    scheduler.getAllRegisteredTasks();
    scheduler.getRegistrationStats();
    scheduler.displayTaskSummary();
    logger.log(MessageType::INFO, "Task 2.2 Complete: Dynamic registry with " + 
               to_string(scheduler.getNumberOfRegisteredTasks()) + " registered tasks");

    // ================================================================
    // Task 2.3: Round-Robin Scheduler Demonstration
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2.3: Round-Robin Scheduler");
    logger.log(MessageType::INFO, "Scheduler integrated with kernel and clock system");
    logger.log(MessageType::INFO, "Round-robin execution: tasks execute in fair rotation");
    logger.log(MessageType::INFO, "State transitions: READY → RUNNING → WAITING");
    logger.log(MessageType::INFO, "Single-task execution: one task per tick");

    // ================================================================
    // Task 2.4: Timer Wake-up System Demonstration  
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2.4: Timer Wake-up System");
    logger.log(MessageType::INFO, "Per-task timer tracking implemented");
    logger.log(MessageType::INFO, "Automatic WAITING → READY transitions");
    logger.log(MessageType::INFO, "BackgroundLogger: 5-tick timer period");
    logger.log(MessageType::INFO, "SystemMonitor: 3-tick timer period");
    logger.log(MessageType::INFO, "DataProcessor: 4-tick timer period");

    // ================================================================
    // Task 2.5: Background Logger Demonstration
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2.5: Background Logger Task");
    logger.log(MessageType::INFO, "Background logger logs every 5 system ticks");
    logger.log(MessageType::INFO, "Demonstrates scheduler functionality");
    logger.log(MessageType::INFO, "Integration with timer system");
    logger.log(MessageType::INFO, "Automatic scheduling cycle");

    // ================================================================
    // System Status and Integration Verification
    // ================================================================
    logger.log(MessageType::HEADER, "System Integration Status");
    logger.log(MessageType::STATUS, "Kernel Initialized: " + string(kernel.isInitialized() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Logger Active: " + string(logger.isInitialized() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Device Count: " + to_string(kernel.getDeviceRegistry().getDeviceCount()));
    logger.log(MessageType::STATUS, "Clock Running: " + string(kernel.getClock().isRunning() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Current Tick: " + to_string(Kernel::getTicks()));
    logger.log(MessageType::STATUS, "Registered Tasks: " + to_string(scheduler.getNumberOfRegisteredTasks()));

    // ================================================================
    // Task 2 Complete Demonstration
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2 - Complete Implementation Demo");
    logger.log(MessageType::INFO, "Real-time clock: 100ms tick intervals");
    logger.log(MessageType::INFO, "Round-robin scheduler: Fair task execution");
    logger.log(MessageType::INFO, "Timer system: Automatic task reactivation");
    logger.log(MessageType::INFO, "Background logger: Periodic system status");

    // Determine execution mode
    bool isInteractive = isatty(fileno(stdin));
    
    if (!isInteractive || (argc > 1 && string(argv[1]) == "demo")) {
        // Demo mode for comprehensive Task 2 demonstration
        logger.log(MessageType::HEADER, "Running Task 2 Complete Demo (60 seconds)");
        logger.log(MessageType::INFO, "Watch all Task 2 components working together:");
        logger.log(MessageType::INFO, "• Round-robin task execution every tick");
        logger.log(MessageType::INFO, "• Timer-based task reactivation");
        logger.log(MessageType::INFO, "• Background logger every 5 ticks");
        logger.log(MessageType::INFO, "• Complete system integration");

        // Enable detailed timer logging for demonstration
        if (auto task = scheduler.findTaskByName("BackgroundLogger")) {
            task->enableTimerCountdown(true);
        }

        // Run comprehensive demo
        auto startTime = chrono::steady_clock::now();
        auto demoTime = chrono::seconds(60);
        int statusInterval = 0;

        while (chrono::steady_clock::now() - startTime < demoTime) {
            this_thread::sleep_for(chrono::milliseconds(5000)); // Status every 5 seconds
            statusInterval++;

            uint64_t currentTick = Kernel::getTicks();
            logger.log(MessageType::STATUS, "=== Demo Progress - " + to_string(statusInterval * 5) + 
                       "s elapsed, Tick " + to_string(currentTick) + " ===");

            // Show detailed system status
            scheduler.displayDetailedTimerStatus();
            scheduler.displayTimerEfficiency();

            // Demonstrate dynamic timer adjustment at 30 seconds
            if (statusInterval == 6) {
                logger.log(MessageType::INFO, "🔧 Demonstrating dynamic timer adjustment...");
                scheduler.adjustTaskTimer("SystemMonitor", 6);
            }

            // Show timer statistics at 45 seconds
            if (statusInterval == 9) {
                logger.log(MessageType::INFO, "📊 Displaying comprehensive timer statistics...");
                scheduler.displayTimerStatistics();
            }

            logger.log(MessageType::STATUS, "=== Status Complete ===");
        }

        logger.log(MessageType::HEADER, "Task 2 Demo Complete");
        logger.log(MessageType::INFO, "🎉 All Task 2 components demonstrated successfully!");
        logger.log(MessageType::INFO, "Final statistics:");
        scheduler.displayTimerStatistics();

    } else {
        // Interactive mode
        logger.log(MessageType::INFO, "Task 2 is running in interactive mode");
        logger.log(MessageType::INFO, "All components are active and executing automatically");
        logger.log(MessageType::INFO, "Type 'exit' to shutdown the system");

        InputMonitor inputMonitor;
        inputMonitor.startMonitoring();

        while (!inputMonitor.isShutdownRequested()) {
            this_thread::sleep_for(chrono::milliseconds(1000));
        }

        inputMonitor.stopMonitoring();
    }

    // ================================================================
    // Task 2 Completion Summary
    // ================================================================
    logger.log(MessageType::HEADER, "Task 2 - Cooperative Scheduler Summary");
    logger.log(MessageType::INFO, "Task 2.1: TCB with state management, priorities, and callbacks");
    logger.log(MessageType::INFO, "Task 2.2: Dynamic task registry with validation and lookup");
    logger.log(MessageType::INFO, "Task 2.3: Round-robin scheduler with fair execution");
    logger.log(MessageType::INFO, "Task 2.4: Timer system with automatic task reactivation");
    logger.log(MessageType::INFO, "Task 2.5: Background logger demonstrating system integration");
    logger.log(MessageType::INFO, "");
    logger.log(MessageType::INFO, "🏆 TASK 2 COMPLETE: Cooperative Scheduler fully implemented!");
    logger.log(MessageType::INFO, "");

    // System shutdown
    logger.log(MessageType::INFO, "Final system statistics:");
    logger.log(MessageType::INFO, "• Total ticks executed: " + to_string(Kernel::getTicks()));
    logger.log(MessageType::INFO, "• System uptime: " + to_string(Kernel::getTicks() * 100) + "ms");
    logger.log(MessageType::INFO, "• Tasks managed: " + to_string(scheduler.getNumberOfRegisteredTasks()));

    logger.log(MessageType::INFO, "Shutting down vOS...");
    kernel.shutdown();
    logger.log(MessageType::INFO, "System halted successfully. Task 2 demonstration complete!");

    return 0;
}
