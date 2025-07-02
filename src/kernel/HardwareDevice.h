#pragma once
#include "Device.h"
#include "DllLoader.h"
#include <string>

class HardwareDevice : public Device {
    LoadedDriver* driver;
    std::string name;
    std::string type;
    bool ready;
public:
    HardwareDevice(LoadedDriver* loadedDriver, std::string deviceName, std::string deviceType);
    std::string read() override;
    bool write(const std::string& data) override;
    std::string getName() const override;
    bool isReady() const override;
    std::string getType() const override;
    std::string getStatus() const override;
    bool configure(int parameter, int value) override;
    bool initialise() override;
    void cleanup() override;
    LoadedDriver* getDriver() const { return driver; }
    void setReady(bool state) { ready = state; }
};
