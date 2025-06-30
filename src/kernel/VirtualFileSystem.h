#pragma once
#include "DllLoader.h"
#include "Logger.h"
#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class LoadedDriver;
class Logger;

struct DeviceNode{
    string devicePath;
    string deviceName;
    LoadedDriver* driver;
    bool isOpen;
    chrono::steady_clock::time_point lastAccess;
    int openCount;
    DeviceNode(const string& path, const string& name, LoadedDriver* drv):
    devicePath(path), deviceName(name), driver(drv), isOpen(false), openCount(0){
        lastAccess = chrono::steady_clock::now();
    }
};

class VirtualFileSystem{
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

        bool registerDevice(const string& drivereName, LoadedDriver* driver);
        bool unregisterDevice(const string& driverPath);

        int openDevice(const string& driverPath);
        int closeDevice(const string& driverPath);
        int readDevice(const string& driverPath, void* buffer, size_t size);
        int writeDevice(const string& driverPath, const void* buffer, size_t size);

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

        static constexpr int VFS_SUCCESS = 0;
        static constexpr int VFS_ERROR_NOT_FOUND = -1;
        static constexpr int VFS_ERROR_NOT_OPEN = -2;
        static constexpr int VFS_ERROR_DRIVER_FAIL = -3;
        static constexpr int VFS_ERROR_INVALID_PATH = -4;
        static constexpr int VFS_ERROR_ALREADY_OPEN = -5;
};