#include "Kernel.h"
#include "DeviceRegistry.h"
#include "Clock.h"
#include "Logger.h"
#include <atomic>
#include <cstdint>
#include<iostream>
#include <memory>
#include "../scheduler/Scheduler.h"

using namespace std;

unique_ptr<Kernel> Kernel::instance = nullptr;
atomic<uint64_t> Kernel::kernelTickCounter {0};

Kernel::Kernel() : initialized(false){
    deviceRegistry = make_unique<DeviceRegistry>();
    systemClock = make_unique<Clock>();
    logger = make_unique<Logger>();
    scheduler = make_unique<Scheduler>();
    dllLoader = std::make_unique<DllLoader>(*logger);
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

bool Kernel::initialize() {
    if (initialized) return true;

    if (!logger->initialize()) {
        cerr << "[BOOT] ERROR: Failed to initialize logger" << endl;
        return false;
    }

    logger->log(MessageType::BOOT, string(getName()) + " v" + getVersion() + " - Initialising...");
    
    if (!deviceRegistry->initialize()) {
        logger->log(MessageType::ERRORS, "Failed to initialize device registry");
        return false;
    }
    logger->log(MessageType::BOOT, "Device registry initialized");
    
    logger->log(MessageType::BOOT, "Scheduler initialized");
    
    if (!systemClock->initialise()) {
        logger->log(MessageType::ERRORS, "Failed to initialize system clock");
        return false;
    }
    logger->log(MessageType::BOOT, "System clock initialized");
    
    initialized = true;
    logger->log(MessageType::BOOT, "vOS ready!");
    return true;
}

void Kernel::shutdown() {
    if (!initialized) return;
    
    logger->log(MessageType::SHUTDOWN, "Stopping system ticks...");
    systemClock->stop();
    
    logger->log(MessageType::SHUTDOWN, "cleaning up devices...");
    deviceRegistry->cleanup();
    
    logger->log(MessageType::SHUTDOWN, "cleaning up logger...");
    initialized = false;
    
    logger->log(MessageType::SHUTDOWN, "vOS shutdown complete");
    logger->stop();
}
