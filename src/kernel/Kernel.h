#pragma once
#include <atomic>
#include <cstdint>
#include <memory>

using namespace std;

class DeviceRegistry;

class Kernel{
    private:
        static unique_ptr<Kernel> instance;
        static atomic<uint64_t> tickCounter;

        unique_ptr<DeviceRegistry> deviceRegistry;
        bool initialized;

        Kernel();

    public:
        ~Kernel();

        static Kernel& getInstance();

        Kernel(const Kernel&) = delete;
        Kernel& operator = (const Kernel& ) = delete;

        bool initialize();
        void shutdown();
        bool isInitialized(){return initialized;}

        static uint64_t getTicks() {return tickCounter;}
        static void incrementTicks(){
            tickCounter.fetch_add(1);
        }

        DeviceRegistry& getDeviceRegistry() const {
            return *deviceRegistry;
        }

        static constexpr const char* getVersion(){
            return "1.0";
        }
        static constexpr const char* getName(){
            return "vOS Kernel";
        }
};