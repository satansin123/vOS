#include "../src/kernel/Kernel.h"
#include "DriverInterface.h"
#include "DriverTypes.h"
#include <string>
#include "../src/kernel/Logger.h"
using namespace std;

static bool initialized = false;
static DriverState currentState = DRIVER_STATE_UNINITIALIZED;

extern "C" {
    const char* driverName() {
        return "SPI0_Driver";
    }
    
    bool driverInit() {
        if (initialized) {
            return true;
        }
        
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Initializing SPI hardware");
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Setting clock frequency: 1MHz");
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Configuring SPI Mode 0 (CPOL=0, CPHA=0)");
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Setting 8-bit data frame");
        
        initialized = true;
        currentState = DRIVER_STATE_INITIALIZED;
        return true;
    }
    
    void driverCleanup() {
        if (initialized) {
            Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Cleaning up SPI resources");
            Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Disabling SPI interface");
            initialized = false;
            currentState = DRIVER_STATE_UNINITIALIZED;
        }
    }
    
    const char* driverVersion() {
        return "1.0.0";
    }
    
    DriverType driverGetType() {
        return DRIVER_TYPE_SPI;
    }
    
    int driverGetCapabilities() {
        return DRIVER_CAP_READ | DRIVER_CAP_WRITE | DRIVER_CAP_CONFIGURE | DRIVER_CAP_DMA;
    }
    
    DriverStatus driverGetStatus() {
        return initialized ? DRIVER_STATUS_SUCCESS : DRIVER_STATUS_NOT_READY;
    }
    
    DriverStatus driverRead(void* buffer, size_t size) {
        if (!initialized) {
            return DRIVER_STATUS_NOT_READY;
        }
        
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Full-duplex read " + to_string(size) + " bytes");
        return DRIVER_STATUS_SUCCESS;
    }
    
    DriverStatus driverWrite(const void* buffer, size_t size) {
        if (!initialized) {
            return DRIVER_STATUS_NOT_READY;
        }
        
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Full-duplex write " + to_string(size) + " bytes");
        return DRIVER_STATUS_SUCCESS;
    }
    
    DriverStatus driverConfigure(int parameter, int value) {
        if (!initialized) {
            return DRIVER_STATUS_NOT_READY;
        }
        
        switch (parameter) {
            case 1: // Clock frequency
                Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Setting clock frequency: " + to_string(value) + " Hz");
                break;
            case 2: // SPI Mode
                Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Setting SPI mode: " + to_string(value));
                break;
            default:
                Kernel::getInstance().getLogger().log(MessageType::INFO, "[SPI] Unknown parameter " + to_string(parameter));
                break;
        }
        
        return DRIVER_STATUS_SUCCESS;
    }
}
