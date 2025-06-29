#include "DllLoader.h"
#include "Kernel.h"
#include "Logger.h"
#include "DeviceRegistry.h"
#include <cstring>
#include <chrono>
#include <exception>
#include <future>
#include<memory>
#include <libloaderapi.h>
#include <memory>
#include <minwindef.h>
#include <string>
#include <vector>
#include <filesystem>  
class DeviceRegistry;
using namespace std;

DllLoader::DllLoader(Logger& log) : logger(log) {
    logger.log(MessageType::DLL_LOADER, "Dynamic loader initialised");
}

DllLoader::~DllLoader() {
    unloadAllDrivers();
}

DllHandle DllLoader::loadLibrary(const string& path) {
#ifdef _WIN32
    return LoadLibraryA(path.c_str());
#else
    return dlopen(path.c_str(), RTLD_LAZY);
#endif
}

FunctionPtr DllLoader::getFunctionAddress(DllHandle handle, const string& functionName) {
#ifdef _WIN32
    return GetProcAddress(handle, functionName.c_str());
#else
    return dlsym(handle, functionName.c_str());
#endif
}

void DllLoader::unloadLibrary(DllHandle handle) {
#ifdef _WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}

bool DllLoader::loadDriver(const string& dllPath) {
    auto startTime = chrono::steady_clock::now();
    logger.log(MessageType::DLL_LOADER, "Loading driver: " + dllPath);

    DllHandle handle = loadLibrary(dllPath);
    if (!handle) {
        logger.log(MessageType::DLL_LOADER, "Failed to load DLL: " + dllPath);
        return false;
    }

    string fileName = filesystem::path(dllPath).stem().string();
    auto driver = make_unique<LoadedDriver>(fileName, dllPath, handle);

    if (!resolveFunctions(*driver)) {
        logger.log(MessageType::DLL_LOADER, "Failed to resolve functions for: " + fileName);
        unloadLibrary(handle);
        return false;
    }

    if (!validateDriver(*driver)) {
        logger.log(MessageType::DLL_LOADER, "Driver Validation failed");
        unloadLibrary(handle);
        return false;
    }

    typedef const char*(*DriverNameFunc)();
    DriverNameFunc nameFunc = reinterpret_cast<DriverNameFunc>(
        reinterpret_cast<void*>(driver->functions.driverName)
    );
    string actualName = nameFunc();
    driver->name = actualName;

    if (loadedDrivers.find(actualName) != loadedDrivers.end()) {
        logger.log(MessageType::DLL_LOADER, "Driver name conflict: " + actualName);
        unloadLibrary(handle);
        return false;
    }
    try{
        if (!initializeAndRegisterDriver(*driver)) {
            logger.log(MessageType::DLL_LOADER, "Driver Initialisation failed: "+ actualName);
            cleanupFailedDriver(std::move(driver));
            return false;
        }
    }
    catch(const exception& ex){
        logger.log(MessageType::DLL_LOADER, "Exception during initialization: " + string(ex.what()));
        cleanupFailedDriver(std::move(driver));
        return false;
    }

    loadedDrivers[actualName] = std::move(driver);

    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    logger.log(MessageType::DLL_LOADER, "Driver loaded successfully: " + actualName + " (" + to_string(duration.count()) + "ms");

    return true;
}

bool DllLoader::initializeAndRegisterDriver(LoadedDriver& driver){
    typedef const char*(*DriverNameFunc)();
    DriverNameFunc nameFunc = reinterpret_cast<DriverNameFunc>(reinterpret_cast<void*>(driver.functions.driverName));
    string driverName = nameFunc();
    logger.log(MessageType::INIT, "Calling driverInit() for: "+ driverName );
    
    typedef bool(*DriverInitFunc)();
    DriverInitFunc initFunc = reinterpret_cast<DriverInitFunc>(reinterpret_cast<void*>(driver.functions.driverInit));
    bool initResult = callDriverInitWithTimeout(initFunc, INIT_TIMEOUT_MS);

    if (!initResult) {
        logger.log(MessageType::INIT, "Driver initialization failed: " + driverName);
        return false;
    }
    driver.initTime = chrono::steady_clock::now();
    driver.initialized = true;
    logger.log(MessageType::INIT, "Driver reports name: \"" + driverName + "\"");

    auto& vfs = Kernel::getInstance().getVfs();
    if (!vfs.registerDevice(driverName, driver.())) {
        logger.log(MessageType::DLL_LOADER, "Failed to register device in VFS: " + driverName);
    }

    logger.log(MessageType::INIT, "Registering device with kernel: " + driverName);
    registerDriverWithKernel(driverName);

    logger.log(MessageType::INIT, "Driver " + driverName + " initialization complete");

    return true;
}

bool DllLoader::callDriverInitWithTimeout(function<bool()> initFunc, int timeoutMs){
    future<bool> future = async(launch::async, [initFunc](){
        try{
            return initFunc();
        }
        catch(...){
            return false;
        }
    });

    if (future.wait_for(chrono::milliseconds(timeoutMs)) == future_status::timeout) {
        logger.log(MessageType::INIT, "Driver init timeout: "+ to_string(timeoutMs));
        return false;
    }
    try {
        return future.get();
    } catch (...) {
        logger.log(MessageType::INIT, "Exception during driver initialization");
        return false;
    }
}

void DllLoader::registerDriverWithKernel(const string& name){
    auto& deviceRegistry = Kernel::getInstance().getDeviceRegistry();
    deviceRegistry.registerDevice(name, "hardware device"); 
}

void DllLoader::cleanupFailedDriver(unique_ptr<LoadedDriver> driver){
    if (driver) {
        if (driver->handle) {
            unloadLibrary(driver->handle);
        }
    }
}

bool DllLoader::resolveFunctions(LoadedDriver& driver) {
    vector<pair<string, FunctionPtr*>> requiredFunctions = {
        {"driverName", &driver.functions.driverName},
        {"driverInit", &driver.functions.driverInit},
        {"driverCleanup", &driver.functions.driverCleanup},
        {"driverVersion", &driver.functions.driverVersion},
        {"driverGetCapabilities", &driver.functions.driverGetCapabilities},
        {"driverGetType", &driver.functions.driverGetType},
        {"driverGetStatus", &driver.functions.driverGetStatus},
        {"driverRead", &driver.functions.driverRead},
        {"driverWrite", &driver.functions.driverWrite},
        {"driverConfigure", &driver.functions.driverConfigure},
    };

    for (const auto& funcPair : requiredFunctions) {
        const string& funcName = funcPair.first;
        FunctionPtr* funcPtr = funcPair.second;
        
        *funcPtr = getFunctionAddress(driver.handle, funcName);
        if (!*funcPtr) {
            logger.log(MessageType::DLL_LOADER, "Missing function: " + funcName);
            return false;
        }
        logger.log(MessageType::DLL_LOADER, "resolved function:" + funcName);
    }
    return true;
}

bool DllLoader::validateDriver(const LoadedDriver& driver) {
    // Fixed: Cast to void* first, then to target function type
    typedef const char*(*DriverNameFunc)();
    DriverNameFunc nameFunc = reinterpret_cast<DriverNameFunc>(
        reinterpret_cast<void*>(driver.functions.driverName)
    );
    const char* name = nameFunc();

    if (!name || strlen(name) == 0) {
        logger.log(MessageType::DLL_LOADER, "Invalid driver name");
        return false;
    }

    typedef const char*(*DriverVersionFunc)();
    DriverVersionFunc versionFunc = reinterpret_cast<DriverVersionFunc>(
        reinterpret_cast<void*>(driver.functions.driverVersion)
    );
    const char* version = versionFunc();

    if (!version || strlen(version) == 0) {
        logger.log(MessageType::DLL_LOADER, "Invalid driver version");
        return false;
    }

    logger.log(MessageType::DLL_LOADER, "Driver validation passed: " + string(name) + " v" + string(version));
    return true;
}

LoadedDriver* DllLoader::getDriver(const string& name) {
    auto it = loadedDrivers.find(name);
    return (it != loadedDrivers.end()) ? it->second.get() : nullptr;
}

int DllLoader::loadAllDriversFromDirectory(const string& directory) {
    logger.log(MessageType::HEADER, "Bulk Driver Loading");
    logger.log(MessageType::DLL_LOADER, "[DLL_LOADER] Scanning directory: " + directory);

    if (!filesystem::exists(directory)) {
        logger.log(MessageType::DLL_LOADER, "Directory not found: " + directory);
        return 0;
    }

    int successCount = 0;
    int totalCount = 0;
    auto startTime = chrono::steady_clock::now();

    try {
        for (const auto& entry : filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                string extension = entry.path().extension().string();
                
#ifdef _WIN32
                if (extension == ".dll") {
#else
                if (extension == ".so") {
#endif
                    totalCount++;
                    string filepath = entry.path().string();
                    logger.log(MessageType::DLL_LOADER, "Attempting to load: " + entry.path().filename().string());
                    
                    if (loadDriver(filepath)) {
                        successCount++;
                        logger.log(MessageType::DLL_LOADER, "SUCCESS");
                    } else {
                        logger.log(MessageType::DLL_LOADER, "FAILED");
                    }
                }
            }
        }
    } catch (const exception& e) {
        logger.log(MessageType::DLL_LOADER, "Directory scan error: " + string(e.what()));
    }

    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    logger.log(MessageType::HEADER, "Bulk Loading Summary");
    logger.log(MessageType::STATUS, "Total DLLs found: " + to_string(totalCount));
    logger.log(MessageType::STATUS, "Successfully loaded: " + to_string(successCount));
    logger.log(MessageType::STATUS, "Failed to load: " + to_string(totalCount - successCount));
    logger.log(MessageType::STATUS, "Loading time: " + to_string(duration.count()) + "ms");

    return successCount;
}

void DllLoader::displayLoadedDrivers() const {
    logger.log(MessageType::HEADER, "Loaded Drivers");
    
    if (loadedDrivers.empty()) {
        logger.log(MessageType::INFO, "No drivers currently loaded");
        return;
    }

    for (const auto& driverPair : loadedDrivers) {
        const string& name = driverPair.first;
        const unique_ptr<LoadedDriver>& driver = driverPair.second;
        
        typedef const char*(*DriverVersionFunc)();
        typedef int(*DriverTypeFunc)();
        typedef int(*DriverCapabilitiesFunc)();
        
        DriverVersionFunc versionFunc = reinterpret_cast<DriverVersionFunc>(
            reinterpret_cast<void*>(driver->functions.driverVersion)
        );
        DriverTypeFunc typeFunc = reinterpret_cast<DriverTypeFunc>(
            reinterpret_cast<void*>(driver->functions.driverGetType)
        );
        DriverCapabilitiesFunc capFunc = reinterpret_cast<DriverCapabilitiesFunc>(
            reinterpret_cast<void*>(driver->functions.driverGetCapabilities)
        );

        string version = versionFunc();
        int type = typeFunc();
        int capabilities = capFunc();

        logger.log(MessageType::STATUS, "Driver: " + name);
        logger.log(MessageType::STATUS, "  Version: " + version);
        logger.log(MessageType::STATUS, "  Type: " + to_string(type));
        logger.log(MessageType::STATUS, "  File: " + driver->filePath);
        logger.log(MessageType::STATUS, "  Capabilities: 0x" + to_string(capabilities));
        logger.log(MessageType::STATUS, "  Initialized: " + string(driver->initialized ? "Yes" : "No"));
    }
}

vector<string> DllLoader::getLoadedDriverNames() const {
    vector<string> names;
    for (const auto& driverPair : loadedDrivers) {
        names.push_back(driverPair.first);
    }
    return names;
}

int DllLoader::getLoadedDriverCount() const {
    return static_cast<int>(loadedDrivers.size());
}

void DllLoader::unloadAllDrivers() {
    logger.log(MessageType::INFO, "[DLL_LOADER] Unloading all drivers...");
    
    for (auto& driverPair : loadedDrivers) {
        const string& name = driverPair.first;
        unique_ptr<LoadedDriver>& driver = driverPair.second;
        
        if (driver->initialized) {
            typedef void(*DriverCleanupFunc)();
            DriverCleanupFunc cleanupFunc = reinterpret_cast<DriverCleanupFunc>(
                reinterpret_cast<void*>(driver->functions.driverCleanup)
            );
            cleanupFunc();
        }
        unloadLibrary(driver->handle);
        logger.log(MessageType::INFO, "[DLL_LOADER] Unloaded: " + name);
    }
    loadedDrivers.clear();
}
