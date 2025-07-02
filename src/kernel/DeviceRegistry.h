#pragma once
#include "kernel.h"
#include <cstddef>
#include<vector>
#include<mutex>
#include<string>

using namespace std;

struct DeviceInfo{
    string name;
    string type;
    bool active;
    DeviceInfo(const string& n, const string& t) :name(n) , type(t), active(true){}
};

class DeviceRegistry{
    private:
        vector<DeviceInfo> devices;
        mutable mutex registryMutex;
        bool initialized;

    public:
        DeviceRegistry();
        ~DeviceRegistry();

        bool initialize();
        void cleanup();

        void registerDevice(const string& name, const string& type);
        void unregisterDevice(const string& name);

        size_t getDeviceCount() const;
        vector<DeviceInfo> getDevices() const;
};