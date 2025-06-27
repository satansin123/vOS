#include "DriverInterface.h"
#include "../src/kernel/Logger.h"
#include "../src/kernel/Kernel.h"
#include "DriverTypes.h"
#include<string>

static bool initialized = false;
static DriverState currentState = DRIVER_STATE_UNINITIALIZED;

extern "C"{
    const char* driverName(){
        return "UART0_Driver";
    }
    bool driverInit(){
        if (initialized) {
            return true;
        }
        Kernel::getInstance().getLogger().log(MessageType::UART, "Initialising UART hardware");
        Kernel::getInstance().getLogger().log(MessageType::UART, "Setting baud rate: 115200");
        Kernel::getInstance().getLogger().log(MessageType::UART, "Configuring 8N1 format");

        initialized = true;
        currentState = DRIVER_STATE_INITIALIZED;
        return initialized;
    }

    void driverCleanup(){
        if (initialized) {
            Kernel::getInstance().getLogger().log(MessageType::UART, "Cleaning up UART resources");
            initialized = false;
            currentState = DRIVER_STATE_UNINITIALIZED;
        }
    }

    const char* driverVersion(){
        return "1.0.0";
    }
    int driverGetCapabilities(){
        return DRIVER_CAP_READ | DRIVER_CAP_CONFIGURE | DRIVER_CAP_WRITE;
    }
    DriverType driverGetType(){
        return DRIVER_TYPE_UART;
    }
    DriverStatus driverGetStatus(){
        return initialized ? DRIVER_STATUS_SUCCESS : DRIVER_STATUS_NOT_READY;
    }

    DriverStatus driverRead(void* buffer, size_t size){
        if (!initialized) {
            return DRIVER_STATUS_NOT_READY;
        }
        Kernel::getInstance().getLogger().log(MessageType::UART, "Reading "+ to_string(size) +" bytes");
        return DRIVER_STATUS_SUCCESS;
    }
    DriverStatus driverWrite(const void* buffer, size_t size){
        if (!initialized) {
            return DRIVER_STATUS_NOT_READY;
        }
        Kernel::getInstance().getLogger().log(MessageType::UART, "Writing "+ to_string(size) +" bytes");
        return DRIVER_STATUS_SUCCESS;
    }
    DriverStatus driverConfigure(int parameter, int value){
        if (!initialized) {
            return DRIVER_STATUS_NOT_READY;
        }
        Kernel::getInstance().getLogger().log(MessageType::UART, "configuring parameter " + to_string(parameter) + " to value "+ to_string(value));
        return DRIVER_STATUS_SUCCESS;
    }
}