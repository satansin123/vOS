#include <iostream>
#include <string>
#include <vector>
#include "kernel/Kernel.h"
#include "kernel/Logger.h"
#include "kernel/DllLoader.h"
using namespace std;
class DeviceRegistry;


using namespace std;

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    auto& kernel = Kernel::getInstance();

    if (!kernel.initialize()) {
        cerr << "Failed to initialize kernel!" << endl;
        return 1;
    }

    auto& logger = kernel.getLogger();
    auto& dllLoader = kernel.getDllLoader();
    auto& vfs = kernel.getVfs();

    logger.log(MessageType::HEADER, "vOS - Virtual Operating System");
    logger.log(MessageType::INFO, "System boot complete. Loading drivers...");

    // Load all drivers from the drivers/ directory
    int loadedCount = dllLoader.loadAllDriversFromDirectory("drivers");
    logger.log(MessageType::INFO, "Loaded drivers: " + to_string(loadedCount));

    // Show registered devices in the VFS
    vfs.displayDeviceTree();

    // List all device paths
    auto deviceList = vfs.listDevice();
    logger.log(MessageType::INFO, "Available devices in /dev:");
    for (const auto& device : deviceList) {
        logger.log(MessageType::INFO, "  " + device);
    }

    // Try opening, configuring, writing, reading, and closing the first device
    if (!deviceList.empty()) {
        string testDevice = deviceList[0];
        logger.log(MessageType::INFO, "Testing device: " + testDevice);

        int openResult = vfs.openDevice(testDevice);
        logger.log(MessageType::INFO, "Open " + testDevice + ": " + (openResult == 0 ? "SUCCESS" : "FAILED(" + to_string(openResult) + ")"));

        if (openResult == 0) {
            int configResult = vfs.configureDevice(testDevice, 1, 115200);
            logger.log(MessageType::INFO, "Configure " + testDevice + ": " + (configResult == 0 ? "SUCCESS" : "FAILED(" + to_string(configResult) + ")"));

            string testData = "Hello from vOS!";
            int writeResult = vfs.writeDevice(testDevice, testData.c_str(), testData.length());
            logger.log(MessageType::INFO, "Write to " + testDevice + ": " + (writeResult > 0 ? to_string(writeResult) + " bytes" : "FAILED(" + to_string(writeResult) + ")"));

            char buffer[64] = {0};
            int readResult = vfs.readDevice(testDevice, buffer, sizeof(buffer));
            logger.log(MessageType::INFO, "Read from " + testDevice + ": " + (readResult > 0 ? to_string(readResult) + " bytes" : "FAILED(" + to_string(readResult) + ")"));

            int closeResult = vfs.closeDevice(testDevice);
            logger.log(MessageType::INFO, "Close " + testDevice + ": " + (closeResult == 0 ? "SUCCESS" : "FAILED(" + to_string(closeResult) + ")"));
        }
    }

    // Open and close a few devices in batch
    vector<string> testDevices;
    for (int i = 0; i < min(3, static_cast<int>(deviceList.size())); i++) {
        testDevices.push_back(deviceList[i]);
    }
    for (const auto& device : testDevices) {
        int result = vfs.openDevice(device);
        logger.log(MessageType::INFO, "Batch open " + device + ": " + (result == 0 ? "SUCCESS" : "FAILED"));
    }
    auto openDevices = vfs.getOpenDevices();
    logger.log(MessageType::INFO, "Currently open devices: " + to_string(openDevices.size()));
    for (const auto& device : openDevices) {
        logger.log(MessageType::INFO, "  " + device);
    }
    for (const auto& device : testDevices) {
        vfs.closeDevice(device);
    }

    // Error handling test
    int invalidResult = vfs.openDevice("/invalid/path");
    logger.log(MessageType::INFO, "Invalid path test: " + (invalidResult < 0 ? "CORRECTLY FAILED(" + to_string(invalidResult) + ")" : "ERROR - SHOULD HAVE FAILED"));
    int nonExistentResult = vfs.openDevice("/dev/nonexistent");
    logger.log(MessageType::INFO, "Non-existent device test: " + (nonExistentResult < 0 ? "CORRECTLY FAILED(" + to_string(nonExistentResult) + ")" : "ERROR - SHOULD HAVE FAILED"));

    vfs.displayDeviceTree();
    vfs.displayVFSStatistics();
    logger.log(MessageType::INFO, "Final device count: " + to_string(vfs.getDeviceCount()));

    logger.log(MessageType::HEADER, "vOS system test complete.");
    kernel.shutdown();
    return 0;
}
