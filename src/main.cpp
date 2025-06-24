#include <chrono>
#include<iostream>
#include <thread>
#include "kernel/Kernel.h"
#include "kernel/DeviceRegistry.h"
#include "kernel/Clock.h"
#include "kernel/Logger.h"
#include "kernel/InputMonitor.h"
#include "scheduler/TCB.h"
#include "scheduler/Scheduler.h"
#include <io.h>      // for isatty on Windows
#include <fcntl.h>


using namespace std;

int main(int argc, char* argv[]) {
    auto& kernel = Kernel::getInstance();
    
    if (!kernel.initialize()) {
        cerr << "Failed to initialise kernel!" << endl;
        return 1;
    }

    auto& logger = kernel.getLogger();
    auto& scheduler = kernel.getScheduler();

    logger.log(MessageType::HEADER, "vOS - Virtual Operating system");
    logger.log(MessageType::INFO, "Running in: " + string(argc > 1 ? argv[1] : "local") + " environment");


    // Create Logger Task (Task 2.4) - logs every 5 ticks
    auto loggerTask = make_unique<TCB>("BackgroundLogger", Priority::LOW, [&logger]() {
        static int tickCount = 0;
        tickCount++;
        if (tickCount % 5 == 0) {
            logger.log(MessageType::INFO, "[Logger] System running smoothly");
        }
    }, 5); // Wait 5 ticks between executions

    auto systemTask = make_unique<TCB>("SystemMonitor", Priority::HIGH, [&logger]() {
        logger.log(MessageType::INFO, "System monitoring task executed");
    }, 3); // Wait 3 ticks

    auto dataTask = make_unique<TCB>("DataProcessor", Priority::MEDIUM, [&logger]() {
        logger.log(MessageType::INFO, "Data processing task executed");
    }, 4); // Wait 4 ticks

    bool result1 = scheduler.registerTask(std::move(loggerTask));
    bool result2 = scheduler.registerTask(std::move(systemTask));
    bool result3 = scheduler.registerTask(std::move(dataTask));

    logger.log(MessageType::INFO, "Registration results: " + to_string(result1) + ", " + to_string(result2) + ", " + to_string(result3));

    // Test duplicate prevention
    auto duplicate = make_unique<TCB>("BackgroundLogger", Priority::MEDIUM, []() {});
    if (!scheduler.registerTask(std::move(duplicate))) {
        logger.log(MessageType::INFO, "Duplicate prevention working correctly");
    }

    scheduler.getAllRegisteredTasks();
    scheduler.getRegistrationStats();
    scheduler.displayTaskSummary();

    logger.log(MessageType::STATUS, "Total registered tasks: " + to_string(scheduler.getNumberOfRegisteredTasks()));

    // System Status
    logger.log(MessageType::HEADER, "System Status");
    logger.log(MessageType::STATUS, "Kernel Initialised: " + string(kernel.isInitialized() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Logger initialized: " + string(logger.isInitialized() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Device count: " + to_string(kernel.getDeviceRegistry().getDeviceCount()));
    logger.log(MessageType::STATUS, "Clock running: " + string(kernel.getClock().isRunning() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Current tick count: " + to_string(Kernel::getTicks()));

    logger.log(MessageType::INFO, "Real-time clock running... System will auto-tick every 100ms");
    logger.log(MessageType::INFO, "Scheduler will execute one task per tick in round-robin order");
    logger.log(MessageType::INFO, "Tasks will automatically transition from WAITING to READY based on their timers");

    bool runDemo = (argc > 1 && string(argv[1]) == "demo") || !isatty(fileno(stdin));

    if (runDemo) {
        // Enhanced Timer System Demo
logger.log(MessageType::HEADER, "Enhanced Timer System Demo");

// Enable countdown logging for demonstration
if (auto task = scheduler.findTaskByName("BackgroundLogger")) {
    task->enableTimerCountdown(true);
}

// Show initial timer status
scheduler.displayDetailedTimerStatus();

// Run demo with timer monitoring
for (int i = 0; i < 10; i++) {
    this_thread::sleep_for(chrono::milliseconds(2000));
    
    logger.log(MessageType::STATUS, "=== Timer Demo - Iteration " + to_string(i+1) + " ===");
    scheduler.displayDetailedTimerStatus();
    scheduler.displayTimerEfficiency();
    
    // Demonstrate dynamic timer adjustment
    if (i == 5) {
        scheduler.adjustTaskTimer("SystemMonitor", 6); // Change from 3 to 6 ticks
    }
}

// Final timer statistics
scheduler.displayTimerStatistics();

        
        logger.log(MessageType::INFO, "Demo completed! Tasks executed with automatic timer management.");
    } else {
        logger.log(MessageType::INFO, "Type 'exit' to shutdown the system");
        InputMonitor inputMonitor;
        inputMonitor.startMonitoring();
        
        while (!inputMonitor.isShutdownRequested()) {
            this_thread::sleep_for(chrono::milliseconds(1000)); // Just wait, let the system run automatically
        }
        
        inputMonitor.stopMonitoring();
    }

    logger.log(MessageType::INFO, "Final tick count: " + to_string(Kernel::getTicks()));
    logger.log(MessageType::INFO, "Shutting down system...");
    kernel.shutdown();
    logger.log(MessageType::INFO, "System halted.");

    return 0;
}
