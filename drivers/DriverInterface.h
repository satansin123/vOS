#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H
#include "DriverTypes.h"

extern "C"{
    const char* driverName();
    bool driverInit();
    void driverCleanup();

    const char* driverVersion();
    int driverGetCapabilities();
    DriverType driverGetType();
    DriverStatus driverGetStatus();

    DriverStatus driverRead(void* buffer, size_t size);
    DriverStatus driverWrite(const void* buffer, size_t size);
    DriverStatus driverConfigure(int parameter, int value);
    
}
#endif