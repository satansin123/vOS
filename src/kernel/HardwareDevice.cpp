#include "HardwareDevice.h"
#include "Kernel.h"
#include <cstring>
using namespace std;
HardwareDevice::HardwareDevice(LoadedDriver* loadedDriver, string deviceName, string deviceType)
    : driver(loadedDriver), name(deviceName), type(deviceType), ready(false) {
    deviceName = name;
    deviceType = type;
}

string HardwareDevice::read() {
    lock_guard<mutex> lock(deviceMutex);
    
    if (!driver || !ready) {
        return "";
    }
    
    try {
        char buffer[1024] = {0};
        
        typedef int(*DriverReadFunc)(void*, size_t);
        DriverReadFunc readFunc = reinterpret_cast<DriverReadFunc>(
            reinterpret_cast<void*>(driver->functions.driverRead)
        );
        
        int result = readFunc(buffer, sizeof(buffer));
        
        if (result >= 0) {
            return string(buffer, result);
        }
        
        return "";
    } catch (const exception& e) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, 
            "Read error for device " + name + ": " + e.what());
        return "";
    }
}

bool HardwareDevice::write(const string& data) {
    lock_guard<mutex> lock(deviceMutex);
    
    if (!driver || !ready) {
        return false;
    }
    
    try {
        typedef int(*DriverWriteFunc)(const void*, size_t);
        DriverWriteFunc writeFunc = reinterpret_cast<DriverWriteFunc>(
            reinterpret_cast<void*>(driver->functions.driverWrite)
        );
        
        int result = writeFunc(data.c_str(), data.length());
        return result >= 0;
        
    } catch (const exception& e) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, 
            "Write error for device " + name + ": " + e.what());
        return false;
    }
}

string HardwareDevice::getName() const {
    lock_guard<mutex> lock(deviceMutex);
    return name;
}

bool HardwareDevice::isReady() const {
    lock_guard<mutex> lock(deviceMutex);
    return ready && driver && isInitialised;
}

string HardwareDevice::getType() const {
    lock_guard<mutex> lock(deviceMutex);
    return type;
}

string HardwareDevice::getStatus() const {
    lock_guard<mutex> lock(deviceMutex);
    
    if (!driver) {
        return "No driver loaded";
    }
    
    if (!isInitialised) {
        return "Not initialized";
    }
    
    if (!ready) {
        return "Not ready";
    }
    
    return "Ready";
}

bool HardwareDevice::configure(int parameter, int value) {
    lock_guard<mutex> lock(deviceMutex);
    
    if (!driver || !ready) {
        return false;
    }
    
    try {
        typedef int(*DriverConfigureFunc)(int, int);
        DriverConfigureFunc configFunc = reinterpret_cast<DriverConfigureFunc>(
            reinterpret_cast<void*>(driver->functions.driverConfigure)
        );
        
        int result = configFunc(parameter, value);
        return result == 0;
        
    } catch (const exception& e) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, 
            "Configure error for device " + name + ": " + e.what());
        return false;
    }
}

bool HardwareDevice::initialise() {
    lock_guard<mutex> lock(deviceMutex);
    
    if (!driver) {
        return false;
    }
    
    if (isInitialised) {
        return true;
    }
    
    try {
        typedef bool(*DriverInitFunc)();
        DriverInitFunc initFunc = reinterpret_cast<DriverInitFunc>(
            reinterpret_cast<void*>(driver->functions.driverInit)
        );
        
        bool result = initFunc();
        if (result) {
            isInitialised = true;
            ready = true;
            Kernel::getInstance().getLogger().log(MessageType::INFO, 
                "Device " + name + " initialized successfully");
        }
        
        return result;
        
    } catch (const exception& e) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, 
            "Initialize error for device " + name + ": " + e.what());
        return false;
    }
}

void HardwareDevice::cleanup() {
    lock_guard<mutex> lock(deviceMutex);
    
    if (!driver || !isInitialised) {
        return;
    }
    
    try {
        typedef void(*DriverCleanupFunc)();
        DriverCleanupFunc cleanupFunc = reinterpret_cast<DriverCleanupFunc>(
            reinterpret_cast<void*>(driver->functions.driverCleanup)
        );
        
        cleanupFunc();
        isInitialised = false;
        ready = false;
        
        Kernel::getInstance().getLogger().log(MessageType::INFO, 
            "Device " + name + " cleaned up successfully");
            
    } catch (const exception& e) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, 
            "Cleanup error for device " + name + ": " + e.what());
    }
}
