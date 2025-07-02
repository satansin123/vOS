#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include "Device.h"
class Device;
class LoadedDriver;
class Logger;
using namespace std;
struct DeviceNode {
    string devicePath;
    string deviceName;
    unique_ptr<Device> device;
    bool isOpen;
    chrono::steady_clock::time_point lastAccess;
    int openCount;
    DeviceNode(const string& path, const string& name, unique_ptr<Device> dev)
        : devicePath(path), deviceName(name), device(std::move(dev)), isOpen(false), openCount(0) {
        lastAccess = chrono::steady_clock::now();
    }
};

class VirtualFileSystem {
    private:
        Logger& logger;
        unordered_map<string, unique_ptr<DeviceNode>> deviceNodes;
        string devRoot = "/dev";
        mutable mutex vfsMutex;
        bool initialized;
    public:
        explicit VirtualFileSystem(Logger& log);
        ~VirtualFileSystem();
        bool initialize();
        void cleanup();

        // Device registration
        bool registerDevice(const string& devicePath, unique_ptr<Device> device);
        bool registerDevice(const string& driverName, LoadedDriver* driver);

        bool unregisterDevice(const string& devicePath);

        int openDevice(const string& devicePath);
        int closeDevice(const string& devicePath);

        int readDevice(const string& devicePath, void* buffer, size_t size);
        int writeDevice(const string& devicePath, const void* buffer, size_t size);
        int configureDevice(const string& devicePath, int parameter, int value);

        vector<string> listDevice() const;
        bool deviceExists(const string& devicePath) const;
        void displayDeviceTree() const;
        size_t getDeviceCount() const;
        vector<string> getOpenDevices() const;
        void displayVFSStatistics() const;

        string generateDevicePath(const string& driverName);
        bool validateDevicePath(const string& devicePath) const;
        void updateLastAccess(DeviceNode& node);

        bool writeToDevice(const string& devicePath, const string& data);
        pair<string, bool> readFromDevice(const string& devicePath, bool blocking = true);

        Device* findDevice(const string& devicePath) const;

        static constexpr int VFS_SUCCESS = 0;
        static constexpr int VFS_ERROR_NOT_FOUND = -1;
        static constexpr int VFS_ERROR_NOT_OPEN = -2;
        static constexpr int VFS_ERROR_DRIVER_FAIL = -3;
        static constexpr int VFS_ERROR_INVALID_PATH = -4;
        static constexpr int VFS_ERROR_ALREADY_OPEN = -5;
};
