#include "Kernel.h"
#include "DeviceRegistry.h"
#include "Clock.h"
#include "Logger.h"
#include <atomic>
#include <cstdint>
#include<iostream>
#include <memory>

using namespace std;

unique_ptr<Kernel> Kernel::instance = nullptr;
atomic<uint64_t> Kernel::tickCounter {0};

Kernel::Kernel() : initialized(false){
    deviceRegistry = make_unique<DeviceRegistry>();
    systemClock = make_unique<Clock>();
    logger = make_unique<Logger>();
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

    if (!logger->initialize()) {
        cout << "[BOOT] ERROR: Failed to initialize logger" <<endl;
        return false;
    }
    cout << "[BOOT] Logger initialized" << endl;

    if (!systemClock->initialise()) {
        cout<<"[BOOT] ERROR: Failed to initialize system clock" <<endl;
    }
    cout << "[BOOT] System clock initialized" << endl;
    initialized = true;
    cout<< "[BOOT] vOS ready!"<<endl;
    return true;
}

void Kernel::shutdown(){
    if(!initialized){
        return;
    }
    cout << "[SHUTDOWN] Stopping system ticks..." << endl;
    systemClock->stop();

    cout<<"[SHUTDOWN] cleaning up devices..."<<endl;
    deviceRegistry->cleanup();

    cout<<"[SHUTDOWN] cleaning up logger..."<<endl;
    logger->shutdown();

    initialized = false;
    cout<<"[SHUTDOWN] vOS shutdown complete"<<endl;
}