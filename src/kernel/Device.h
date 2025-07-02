#pragma once
#include <string>
#include <mutex>

class Device {
    public:
        virtual ~Device() = default;
        virtual std::string read() = 0;
        virtual bool write(const std::string& data) = 0;
        virtual std::string getName() const = 0;
        virtual bool isReady() const = 0;
        virtual std::string getType() const = 0;
        virtual std::string getStatus() const = 0;
        virtual bool configure(int parameter, int value) = 0;
        virtual bool initialise() = 0;
        virtual void cleanup() = 0;

    protected:
        mutable std::mutex deviceMutex;
        bool isInitialised = false;
        std::string deviceName;
        std::string deviceType;
};
