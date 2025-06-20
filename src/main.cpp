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

using namespace std;

int main(int argc, char* argv[]) {
    auto& kernel = Kernel::getInstance();
    
    if (!kernel.initialize()) {
        cerr << "Failed to initialise kernel!" << endl;  
        return 1;
    }
    
    auto& logger = kernel.getLogger();
    
    logger.log(MessageType::HEADER, "vOS - Virtual Operating system");
    logger.log(MessageType::INFO, "Running in: " + string(argc > 1 ? argv[1] : "local") + " environment");
    
    logger.log(MessageType::HEADER, "Testing TCB Implementation");
    TCB testTask("TestLogger", Priority::MEDIUM, [&logger](){
        logger.log(MessageType::INFO, "Hello from test task!");
    });
    
    logger.log(MessageType::INFO, "Task: " + testTask.getName() + " (ID: " + to_string(testTask.getId()) + ")");
    logger.log(MessageType::INFO, "Priority: " + testTask.getPriorityString());
    logger.log(MessageType::INFO, "State: " + testTask.getStateString());
    
    testTask.setState(TaskState::RUNNING);
    logger.log(MessageType::INFO, "After RUNNING: " + testTask.getStateString());
    testTask.executeTask();
    testTask.setState(TaskState::WAITING);
    logger.log(MessageType::INFO, "After WAITING: " + testTask.getStateString());

    // Test Scheduler Registration System
    logger.log(MessageType::HEADER, "Testing Task Registration System");

    Scheduler scheduler;

    // Create and register test tasks
    auto task1 = make_unique<TCB>("BackgroundLogger", Priority::LOW, 
        [&logger](){ logger.log(MessageType::INFO, "Background task executed"); });

    auto task2 = make_unique<TCB>("SystemMonitor", Priority::HIGH, 
        [&logger](){ logger.log(MessageType::INFO, "System monitoring task executed"); });

    auto task3 = make_unique<TCB>("DataProcessor", Priority::MEDIUM, 
        [&logger](){ logger.log(MessageType::INFO, "Data processing task executed"); });

    // Register tasks
    bool result1 = scheduler.registerTask(std::move(task1));
    bool result2 = scheduler.registerTask(std::move(task2));
    bool result3 = scheduler.registerTask(std::move(task3));

    logger.log(MessageType::INFO, "Registration results: " + to_string(result1) + ", " + to_string(result2) + ", " + to_string(result3));

    // Test duplicate prevention
    auto duplicate = make_unique<TCB>("BackgroundLogger", Priority::MEDIUM, 
        [](){ /* duplicate task */ });

    if (!scheduler.registerTask(std::move(duplicate))) {
        logger.log(MessageType::INFO, "Duplicate prevention working correctly");
    }

    // Display registered tasks and statistics
    scheduler.getAllRegisteredTasks();
    scheduler.getRegistrationStats();
    scheduler.displayTaskSummary();

    logger.log(MessageType::STATUS, "Total registered tasks: " + to_string(scheduler.getNumberOfRegisteredTasks()));

    
    logger.log(MessageType::HEADER, "System Status");
    logger.log(MessageType::STATUS, "Kernel Initialised: " + string(kernel.isInitialized() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Logger initialized: " + string(logger.isInitialized() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Device count: " + to_string(kernel.getDeviceRegistry().getDeviceCount()));
    logger.log(MessageType::STATUS, "Clock running: " + string(kernel.getClock().isRunning() ? "YES" : "NO"));
    logger.log(MessageType::STATUS, "Current tick count: " + to_string(Kernel::getTicks()));
    
    logger.log(MessageType::INFO, "Real-time clock running... System will auto-tick every 100ms");
    logger.log(MessageType::INFO, "Type 'exit' to shutdown the system");
    
    InputMonitor inputMonitor;
    inputMonitor.startMonitoring();
    
    while (!inputMonitor.isShutdownRequested()) {
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    
    logger.log(MessageType::INFO, "Final tick count: " + to_string(Kernel::getTicks()));
    logger.log(MessageType::INFO, "Shutting down system...");
    
    inputMonitor.stopMonitoring();
    kernel.shutdown();
    
    logger.log(MessageType::INFO, "System halted.");
    return 0;
}
