#include "Kernel.h"
#include "DeviceRegistry.h"
#include <atomic>
#include <cstdint>
#include<iostream>
#include <memory>

using namespace std;

unique_ptr<Kernel> Kernel::instance = nullptr;
atomic<uint64_t> Kernel::tickCounter = 0;

Kernel::Kernel() : initialized(false){
    deviceRegistry = make_unique<DeviceRegistry>();
}

Kernel::~Kernel(){
    if (initialized) {
        shutdown();
    }
}

Kernel& Kernel::getInstance(){
    if (!instance) {
        instance = unique_ptr<Kernel>(new Kernel());
    }
    return *instance;
}

bool Kernel::initialize(){
    if(initialized){
        return true;
    }

    cout<<"[BOOT] "<<getName()<<" v"<<getVersion()<<" - Initialising..."<<endl;

    if(!deviceRegistry->initialize()){
        cout << "[BOOT] ERROR: Failed to initialize device registry" <<endl;
        return false;
    }

    cout << "[BOOT] Device registry initialized" << endl;

    cout << "[BOOT] System clock initialized" << std::endl;
    initialized = true;
    cout<< "[BOOT] vOS ready!"<<endl;
    return true;
}

void Kernel::shutdown(){
    if(!initialized){
        return;
    }

    cout<<"[SHUTDOWN] cleaning up devices..."<<endl;
    deviceRegistry->cleanup();

    initialized = false;
    cout<<"[SHUTDOWN] vOS shutdown complete"<<endl;
}