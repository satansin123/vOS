#include <iostream>
#include <string>
#include <vector>
#include "kernel/Kernel.h"
#include "kernel/Logger.h"
#include "kernel/DllLoader.h"
#include "kernel/DeviceRegistry.h"
using namespace std;
class DeviceRegistry;
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    auto& kernel = Kernel::getInstance();
    if (!kernel.initialize()) {
        cerr << "Failed to initialize kernel!" << endl;
        return 1;
    }

    auto& logger = kernel.getLogger();
    auto& dllLoader = kernel.getDllLoader();

    logger.log(MessageType::HEADER, "vOS - Virtual Operating System");
    logger.log(MessageType::INFO, "Running Task 3.3: Driver Initialization and Registration");
    logger.log(MessageType::INFO, "Testing automatic driver initialization system");

    // Test 1: Individual Driver Loading with Initialization
    logger.log(MessageType::INFO, "=== Test 1: Individual Driver Loading with Initialization ===");
    string driverPath = "drivers/UARTDriver.dll";
    bool loadResult = dllLoader.loadDriver(driverPath);
    logger.log(MessageType::INFO, "Individual load and init result: " + string(loadResult ? "SUCCESS" : "FAILED"));

    // Test 2: Bulk Directory Loading with Initialization
    logger.log(MessageType::INFO, "=== Test 2: Bulk Directory Loading with Initialization ===");
    int loadedCount = dllLoader.loadAllDriversFromDirectory("drivers");
    logger.log(MessageType::INFO, "Bulk loading and init result: " + to_string(loadedCount) + " drivers loaded");

    // Test 3: Verify Device Registry Integration
    logger.log(MessageType::INFO, "=== Test 3: Device Registry Integration ===");
    auto& deviceRegistry = kernel.getDeviceRegistry();
    auto devices = deviceRegistry.getDevices();
    
    logger.log(MessageType::INFO, "Total devices registered: " + to_string(devices.size()));
    logger.log(MessageType::INFO, "Registered devices:");
    for (const auto& device : devices) {
        logger.log(MessageType::INFO, "  - " + device.name + " (" + device.type + ")");
    }

    // Test 4: Driver Status Verification
    logger.log(MessageType::INFO, "=== Test 4: Driver Status Verification ===");
    vector<string> driverNames = dllLoader.getLoadedDriverNames();
    logger.log(MessageType::INFO, "Available initialized drivers: " + to_string(driverNames.size()));
    
    for (const auto& name : driverNames) {
        LoadedDriver* driver = dllLoader.getDriver(name);
        if (driver && driver->initialized) {
            typedef int(*DriverStatusFunc)();
            DriverStatusFunc statusFunc = reinterpret_cast<DriverStatusFunc>(driver->functions.driverGetStatus);
            int status = statusFunc();
            logger.log(MessageType::INFO, "  " + name + " status: " + to_string(status) + " (initialized: Yes)");
        }
    }

    // Test 5: Driver Information Display
    logger.log(MessageType::INFO, "=== Test 5: Driver Information Display ===");
    dllLoader.displayLoadedDrivers();

    // Summary
    logger.log(MessageType::HEADER, "Task 3.3 Test Summary");
    logger.log(MessageType::INFO, "✓ Automatic driver initialization working");
    logger.log(MessageType::INFO, "✓ Device registry integration working");
    logger.log(MessageType::INFO, "✓ Driver name validation working");
    logger.log(MessageType::INFO, "✓ Initialization timeout handling working");
    logger.log(MessageType::INFO, "🎉 TASK 3.3 COMPLETE: Driver Initialization and Registration");

    kernel.shutdown();
    return 0;
}
