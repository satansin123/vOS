#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include "../scheduler/Scheduler.h"
#include "DllLoader.h"
using namespace std;

class DeviceRegistry;
class Clock;
class Logger;
class Scheduler;

class Kernel{
    private:
        static unique_ptr<Kernel> instance;
        static atomic<uint64_t> kernelTickCounter;

        unique_ptr<DeviceRegistry> deviceRegistry;
        unique_ptr<Clock> systemClock;
        unique_ptr<Logger> logger;
        unique_ptr<Scheduler> scheduler;
        unique_ptr<DllLoader> dllLoader;

        bool initialized;

        Kernel();

    public:
        ~Kernel();
        
        static constexpr const char* getVersion(){
            return "1.0";
        }
        static constexpr const char* getName(){
            return "vOS Kernel";
        }
        
        static Kernel& getInstance();

        Kernel(const Kernel&) = delete;
        Kernel& operator = (const Kernel& ) = delete;

        bool initialize();
        void shutdown();
        bool isInitialized(){return initialized;}

        static uint64_t getTicks() {return kernelTickCounter;}
        static void incrementTicks(){
            kernelTickCounter.fetch_add(1);
        }

        DeviceRegistry& getDeviceRegistry() const {
            return *deviceRegistry;
        }
        Clock& getClock() const {
            return *systemClock;
        }
        Logger& getLogger() const{
            return *logger;
        }
        Scheduler& getScheduler() const{
            return *scheduler;
        }
        DllLoader& getDllLoader() const{
            return *dllLoader;
        }

};