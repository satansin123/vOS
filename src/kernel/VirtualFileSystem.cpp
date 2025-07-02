#include "VirtualFileSystem.h"
#include "Device.h"
#include "Logger.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "HardwareDevice.h"
using namespace std;

VirtualFileSystem::VirtualFileSystem(Logger& log) : logger(log), initialized(false) {
    logger.log(MessageType::VFS, "virtual filesystem init..");
}

VirtualFileSystem::~VirtualFileSystem(){
    cleanup();
}

bool VirtualFileSystem::initialize(){
    lock_guard<mutex> lock(vfsMutex);
    if (initialized) {
        return true;
    }
    logger.log(MessageType::VFS, "Creating virtual device tree structure");
    logger.log(MessageType::VFS, "Device root: " + devRoot);
    initialized = true;
    logger.log(MessageType::VFS, "Vfs init success" );
    return true;
}

void VirtualFileSystem::cleanup() {
    lock_guard<mutex> lock(vfsMutex);
    
    if (!initialized) {
        return;
    }
    
    logger.log(MessageType::VFS, "Cleaning up virtual filesystem...");
    for (auto& pair : deviceNodes) {
        if (pair.second->isOpen) {
            logger.log(MessageType::VFS, "Force closing device: " + pair.first);
            pair.second->isOpen = false;
            pair.second->openCount = 0;
        }
    }
    
    deviceNodes.clear();
    initialized = false;
    
    logger.log(MessageType::VFS, "Virtual filesystem cleanup complete");
}
bool VirtualFileSystem::registerDevice(const std::string& driverName, LoadedDriver* driver) {
    if (!driver) {
        logger.log(MessageType::VFS, "Cannot register null driver: " + driverName);
        return false;
    }

    std::string devicePath = generateDevicePath(driverName);

    auto hardwareDevice = std::make_unique<HardwareDevice>(driver, driverName, "Hardware");

    return registerDevice(devicePath, std::move(hardwareDevice));
}

bool VirtualFileSystem::registerDevice(const std::string& devicePath, std::unique_ptr<Device> device) {
    lock_guard<mutex> lock(vfsMutex);
    
    if (!device) {
        logger.log(MessageType::VFS, "Cannot register null device: " + devicePath);
        return false;
    }
    
    if (!validateDevicePath(devicePath)) {
        logger.log(MessageType::VFS, "Invalid device path: " + devicePath);
        return false;
    }
    
    if (deviceExists(devicePath)) {
        logger.log(MessageType::VFS, "Device path already exists: " + devicePath);
        return false;
    }
    
    if (!device->initialise()) {
        logger.log(MessageType::VFS, "Failed to initialize device: " + devicePath);
        return false;
    }
    
    string deviceName = device->getName();
    auto deviceNode = make_unique<DeviceNode>(devicePath, deviceName, std::move(device));
    deviceNodes[devicePath] = std::move(deviceNode);
    
    logger.log(MessageType::VFS, "Device registered: " + devicePath + " (" + deviceName + ")");
    return true;
}
bool VirtualFileSystem::unregisterDevice(const string& devicePath){
    lock_guard<mutex> lock(vfsMutex);

    auto it = deviceNodes.find(devicePath);
    if (it==deviceNodes.end()) {
        logger.log(MessageType::VFS, "Device not found for unregister: " + devicePath);
        return false;
    }
    if (it->second->isOpen) {
        logger.log(MessageType::VFS, "closing device: " + devicePath);
        it->second->isOpen = false;
    }

    string driverName = it->second->deviceName;
    deviceNodes.erase(it);
    logger.log(MessageType::VFS, "Device unregistered: " + devicePath + " (" + driverName + ")");
    return true;
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
        transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        path+=lower.empty() ? "generic0" : lower;
    }
    return path;
}

bool VirtualFileSystem::validateDevicePath(const string& devicePath) const {
    if (devicePath.substr(0, devRoot.length()) != devRoot) {
        return false;
    }
    if (devicePath.length() <= devRoot.length()+1) {
        return false;
    }
    return true;
}

void VirtualFileSystem::updateLastAccess(DeviceNode& node){
    node.lastAccess = chrono::steady_clock::now();
}

int VirtualFileSystem::openDevice(const string& devicePath){
    lock_guard<mutex> lock(vfsMutex);

    if (!validateDevicePath(devicePath)) {
        logger.log(MessageType::VFS, "Invalid path: " + devicePath);
        return VFS_ERROR_INVALID_PATH;
    }
    auto it = deviceNodes.find(devicePath);
    if (it==deviceNodes.end()) {
        logger.log(MessageType::VFS, "device not found: " + devicePath);
        return VFS_ERROR_NOT_FOUND;
    }

    auto& node = it->second;

    if (node->isOpen) {
        logger.log(MessageType::VFS, "device already open: " + devicePath);
        return VFS_ERROR_ALREADY_OPEN;
    }

    node->isOpen = true;
    node->openCount++;
    updateLastAccess(*node);
    logger.log(MessageType::VFS, "Device opened: "+devicePath);
    return VFS_SUCCESS;
}

int VirtualFileSystem::closeDevice(const string& devicePath){
    lock_guard<mutex> lock(vfsMutex);

    auto it = deviceNodes.find(devicePath);

    if (it == deviceNodes.end()) {
        logger.log(MessageType::VFS, "Device not found: " + devicePath);
        return VFS_ERROR_NOT_FOUND;
    }

    auto& node = it->second;
    if (!node->isOpen) {
        logger.log(MessageType::VFS, "Device not open: " + devicePath);
        return VFS_ERROR_NOT_OPEN;
    }
    node->isOpen = false;
    updateLastAccess(*node);
    logger.log(MessageType::VFS, "Device close: " + devicePath);
    return VFS_SUCCESS;
}

int VirtualFileSystem::readDevice(const string& devicePath, void* buffer, size_t size){
    lock_guard<mutex> lock(vfsMutex);
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
    string data = node->device->read();
    if (data.empty()) {
        return -3;
    }
    size_t toCopy = min(size-1, data.size());
    memcpy(buffer, data.data(), toCopy);
    static_cast<char*>(buffer)[toCopy] = '\0';
    updateLastAccess(*node);
    return static_cast<int>(toCopy);
}

bool VirtualFileSystem::writeToDevice(const string& devicepath, const string& data){
    lock_guard<mutex> lock(vfsMutex);

    if (!validateDevicePath(devicepath)) {
        logger.log(MessageType::VFS, "Invalid device path: " + devicepath);
        return false;
    }
    Device* device = findDevice(devicepath);
    if (!device) {
        logger.log(MessageType::VFS, "Device not found: " + devicepath);
        return false;
    }
    if (!device->isReady()) {
        logger.log(MessageType::VFS, "Device not ready: " + devicepath);
        return false;
    }
    logger.log(MessageType::VFS, "Writing to " + devicepath + " (" + to_string(data.length()) + " bytes)");

    bool result = device->write(data);
    if (result) {
        auto it = deviceNodes.find(devicepath);
        if (it!= deviceNodes.find(devicepath)) {
            updateLastAccess(*it->second);
        }
        logger.log(MessageType::VFS, "Write successful to " + devicepath);
    } else {
        logger.log(MessageType::VFS, "Write failed to " + devicepath);
    
    }
    return result;
}
pair<string, bool> VirtualFileSystem::readFromDevice(const std::string& devicePath, bool blocking) {
    lock_guard<mutex> lock(vfsMutex);
    
    if (!validateDevicePath(devicePath)) {
        logger.log(MessageType::VFS, "Invalid device path: " + devicePath);
        return make_pair("", false);
    }
    
    Device* device = findDevice(devicePath);
    if (!device) {
        logger.log(MessageType::VFS, "Device not found: " + devicePath);
        return make_pair("", false);
    }
    
    if (!device->isReady()) {
        logger.log(MessageType::VFS, "Device not ready: " + devicePath);
        return make_pair("", false);
    }
    
    logger.log(MessageType::VFS, "Reading from " + devicePath + 
               " (mode: " + (blocking ? "blocking" : "non-blocking") + ")");
    
    string data = device->read();
    bool success = !data.empty();
    
    if (success) {
        // Update access time
        auto it = deviceNodes.find(devicePath);
        if (it != deviceNodes.end()) {
            updateLastAccess(*it->second);
        }
        logger.log(MessageType::VFS, "Read successful from " + devicePath + 
                   " (" + to_string(data.length()) + " bytes)");
    } else {
        logger.log(MessageType::VFS, "Read failed from " + devicePath);
    }
    
    return make_pair(data, success);
}
int VirtualFileSystem::writeDevice(const string& driverPath, const void* buffer, size_t size){
    lock_guard<mutex> lock(vfsMutex);
    auto it = deviceNodes.find(driverPath);
    if (it==deviceNodes.end()) {
        return -1;  
    } 
    auto& node = it->second;
    if (!node->isOpen) {
        return -2;
    }
    string data(static_cast<const char*>(buffer), size);
    bool result = node->device->write(data);
    updateLastAccess(*node);
    return result ? static_cast<int>(size) : -3;
}

int VirtualFileSystem::configureDevice(const string& devicePath, int parameter, int value){
    lock_guard<mutex> lock(vfsMutex);

    auto it = deviceNodes.find(devicePath);
    if (it==deviceNodes.end()) {
        logger.log(MessageType::VFS, "Device not found: " + devicePath);
        return VFS_ERROR_NOT_FOUND;
    }

    auto& node = it->second;
    bool result = node->device->configure(parameter, value);
    updateLastAccess(*node);

    return result ? VFS_SUCCESS : VFS_ERROR_DRIVER_FAIL;
}

Device* VirtualFileSystem::findDevice(const std::string& devicePath) const {
    auto it = deviceNodes.find(devicePath);
    return (it != deviceNodes.end()) ? it->second->device.get() : nullptr;
}

vector<string> VirtualFileSystem::listDevice() const{
    lock_guard<mutex> lock(vfsMutex);

    vector<string> deviceList;
    for (const auto& pair  : deviceNodes) {
        deviceList.push_back(pair.first);
    }
    return deviceList;
}

bool VirtualFileSystem::deviceExists(const string& devicePath) const {
    // lock_guard<mutex> lock(vfsMutex);
    return deviceNodes.find(devicePath) != deviceNodes.end();
}

void VirtualFileSystem::displayDeviceTree() const{
    lock_guard<mutex> lock(vfsMutex);

    logger.log(MessageType::HEADER, "Virtual device tree");
    logger.log(MessageType::STATUS, devRoot+"/");

    if (deviceNodes.empty()) {
        logger.log(MessageType::VFS, "(no devices registerd)");
        return;
    }

    vector<string> sortedPaths;
    for(const auto& pair: deviceNodes){
        sortedPaths.push_back(pair.first);
    }
    sort(sortedPaths.begin(), sortedPaths.end());

    for (const auto& path : sortedPaths) {
        const auto& node = deviceNodes.at(path);
        string deviceName = path.substr(devRoot.length()+1);

        string status = node->isOpen ? "[OPEN]" : "[CLOSED]";
        string accessInfo = "accessed " + to_string(node->openCount) + " times";

        logger.log(MessageType::STATUS, "  " + deviceName + " -> " + node->deviceName + " " + status + " (" + accessInfo + ")");
    }
}

size_t VirtualFileSystem::getDeviceCount() const{
    lock_guard<mutex> lock(vfsMutex);
    return deviceNodes.size();
}

vector<string> VirtualFileSystem::getOpenDevices() const {
    lock_guard<mutex> lock(vfsMutex);
    
    vector<string> openDevices;
    for (const auto& pair : deviceNodes) {
        if (pair.second->isOpen) {
            openDevices.push_back(pair.first);
        }
    }
    
    return openDevices;
}

void VirtualFileSystem::displayVFSStatistics() const {
    lock_guard<mutex> lock(vfsMutex);
    
    logger.log(MessageType::HEADER, "VFS Statistics");
    
    int totalDevices = static_cast<int>(deviceNodes.size());
    int openDevices = 0;
    int totalAccesses = 0;
    
    for (const auto& pair : deviceNodes) {
        if (pair.second->isOpen) {
            openDevices++;
        }
        totalAccesses += pair.second->openCount;
    }
    
    logger.log(MessageType::STATUS, "Total devices: " + to_string(totalDevices));
    logger.log(MessageType::STATUS, "Open devices: " + to_string(openDevices));
    logger.log(MessageType::STATUS, "Closed devices: " + to_string(totalDevices - openDevices));
    logger.log(MessageType::STATUS, "Total accesses: " + to_string(totalAccesses));
    
    if (totalDevices > 0) {
        float avgAccesses = static_cast<float>(totalAccesses) / totalDevices;
        logger.log(MessageType::STATUS, "Average accesses per device: " + to_string(avgAccesses));
    }
}