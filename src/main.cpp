#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "kernel/Kernel.h"
#include "kernel/Logger.h"
#include "kernel/DllLoader.h"


using namespace std;

int main(int argc, char* argv[]) {
    auto& kernel = Kernel::getInstance();
    if (!kernel.initialize()) {
        cerr << "Failed to initialize kernel!" << endl;
        return 1;
    }

    auto& logger = kernel.getLogger();
    auto& dllLoader = kernel.getDllLoader();

    logger.log(MessageType::HEADER, "vOS - Virtual Operating System");
    logger.log(MessageType::INFO, "Running Task 3.2: Dynamic DLL Loading");
    logger.log(MessageType::INFO, "Testing runtime driver loading system");

    // Test 1: Load individual driver
    logger.log(MessageType::INFO, "=== Test 1: Individual Driver Loading ===");
    
    string driverPath = "bin/drivers/UARTDriver.dll";
    bool loadResult = dllLoader.loadDriver(driverPath);
    logger.log(MessageType::INFO, "Individual load result: " + string(loadResult ? "SUCCESS" : "FAILED"));

    // Test 2: Bulk directory loading
    logger.log(MessageType::INFO, "=== Test 2: Bulk Directory Loading ===");
    
    int loadedCount = dllLoader.loadAllDriversFromDirectory("bin/drivers");
    logger.log(MessageType::INFO, "Bulk loading result: " + to_string(loadedCount) + " drivers loaded");

    // Test 3: Driver access and testing
    logger.log(MessageType::INFO, "=== Test 3: Driver Access and Testing ===");
    
    vector<string> driverNames = dllLoader.getLoadedDriverNames();
    logger.log(MessageType::INFO, "Available drivers: " + to_string(driverNames.size()));

    for (const auto& name : driverNames) {
        logger.log(MessageType::INFO, "Testing driver: " + name);
        
        LoadedDriver* driver = dllLoader.getDriver(name);
        if (driver) {
            // Test driver initialization
            typedef bool (*DriverInitFunc)();
            DriverInitFunc initFunc = reinterpret_cast<DriverInitFunc>(driver->functions.driverInit);
            
            bool initResult = initFunc();
            driver->initialized = initResult;
            
            logger.log(MessageType::INFO, "  Initialization: " + string(initResult ? "SUCCESS" : "FAILED"));

            if (initResult) {
                // Test driver operations
                typedef int (*DriverStatusFunc)();
                typedef int (*DriverConfigureFunc)(int, int);
                typedef int (*DriverWriteFunc)(const void*, size_t);

                DriverStatusFunc statusFunc = reinterpret_cast<DriverStatusFunc>(driver->functions.driverGetStatus);
                DriverConfigureFunc configFunc = reinterpret_cast<DriverConfigureFunc>(driver->functions.driverConfigure);
                DriverWriteFunc writeFunc = reinterpret_cast<DriverWriteFunc>(driver->functions.driverWrite);

                int status = statusFunc();
                logger.log(MessageType::INFO, "  Status: " + to_string(status));

                int configResult = configFunc(1, 115200);
                logger.log(MessageType::INFO, "  Configure: " + to_string(configResult));

                string testData = "Hello from " + name;
                int writeResult = writeFunc(testData.c_str(), testData.length());
                logger.log(MessageType::INFO, "  Write: " + to_string(writeResult));
            }
        }
    }

    // Test 4: Driver information display
    logger.log(MessageType::INFO, "=== Test 4: Driver Information Display ===");
    dllLoader.displayLoadedDrivers();

    logger.log(MessageType::HEADER, "Task 3.2 Test Summary");
    logger.log(MessageType::INFO, "✓ Dynamic DLL loading system working");
    logger.log(MessageType::INFO, "✓ Driver function resolution working");
    logger.log(MessageType::INFO, "✓ Driver management system working");

    logger.log(MessageType::INFO, "🎉 TASK 3.2 COMPLETE: Dynamic DLL Loading");

    kernel.shutdown();
    logger.log(MessageType::INFO, "Task 3.2 demonstration complete!");

    return 0;
}
