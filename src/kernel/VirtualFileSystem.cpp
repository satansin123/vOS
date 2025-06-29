#include "VirtualFileSystem.h"
#include "DllLoader.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>

using namespace std;

bool VirtualFileSystem::registerDevice(const string& driverName, LoadedDriver* driver){
    if (!driver) {
        logger.log(MessageType::VFS, "Cannot register null driver: "+driverName);
        return false;
    }
    string devicePath = generateDevicePath(driverName);

    if (deviceExists(devicePath)) {
        logger.log(MessageType::VFS, "display path already exists: " + devicePath);
        return false;
    }
    auto deviceNode = make_unique<DeviceNode>(devicePath, driverName, driver);
    deviceNodes[devicePath] = std::move(deviceNode);
    logger.log(MessageType::VFS, "Device registered: "+driverName);
    return false;
}
string VirtualFileSystem::generateDevicePath(const string& driverName){
    string path = devRoot + "/";
    if (driverName.find("UART") != string::npos) {
        path+="uart0";
    }
    else if (driverName.find("ADC") != string::npos) {
        path += "adc0";
    }
    else if (driverName.find("GPIO") != string::npos) {
        path += "gpio0";
    }
    else if (driverName.find("I2C") != string::npos) {
        path += "i2c0";
    }
    else if (driverName.find("SPI") != string::npos) {
        path += "spi0";
    }
    else if (driverName.find("Timer") != string::npos) {
        path += "timer0";
    }
    else if (driverName.find("PWM") != string::npos) {
        path += "pwm0";
    }
    else{
        string lower = driverName;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        path+=lower;
    }
    return path;
}

int VirtualFileSystem::readDevice(const string& devicePath, void* buffer, size_t size){
    auto it = deviceNodes.find(devicePath);
    if (it == deviceNodes.end()) {
        logger.log(MessageType::VFS, "Device not found: " +devicePath );
        return -1;
    }
    auto& node = it->second;
    if (!node->isOpen) {
        logger.log(MessageType::VFS, "Device not open: "+devicePath);
        return -2;
    }
    typedef int (*DriverReadFunc)(void*, size_t);
    DriverReadFunc readFunc = reinterpret_cast<DriverReadFunc>(reinterpret_cast<void*>(node->driver->functions.driverRead));

    logger.log(MessageType::VFS, "reading from " + devicePath + " (" + to_string(size) + " bytes)");
    int result = readFunc(buffer, size);
    node->lastAccess = chrono::steady_clock::now();
    return result == 0? static_cast<int>(size) : -3;
}

int VirtualFileSystem::writeDevice(const string& driverPath, const void* buffer, size_t size){
    auto it = deviceNodes.find(driverPath);
    if (it==deviceNodes.end()) {
        return -1;  
    } 
    auto& node = it->second;
    if (!node->isOpen) {
        return -2;
    }
    typedef int (*DriverWriteFunc)(const void*, size_t);
    DriverWriteFunc writeFunc = reinterpret_cast<DriverWriteFunc>(reinterpret_cast<void*>(node->driver->functions.driverWrite));
    logger.log(MessageType::VFS, "Writing to " + driverPath + " (" + std::to_string(size) + " bytes)");
    int result = writeFunc(buffer, size);
    node->lastAccess = chrono::steady_clock::now();
    return result == 0? static_cast<int>(size):-3;
}