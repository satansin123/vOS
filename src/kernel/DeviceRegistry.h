#pragma once
#include "kernel.h"
#include <cstddef>
#include<vector>
#include<mutex>
#include<string>

using namespace std;

struct Device{
    string name;
    string type;
    bool active;
    Device(const string& n, const string& t) :name(n) , type(t), active(true){}
};

class DeviceRegistry{
    private:
        vector<Device> devices;
        mutex registryMutex;
        bool initialized;

    public:
        DeviceRegistry();
        ~DeviceRegistry();

        bool initialize();
        void cleanup();

        void registerDevice(const string& name, const string& type);
        void unregisterDevice(const string& name);

        size_t getDeviceCount() ;
        vector<Device> getDevices() ;
};