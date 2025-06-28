#pragma once
#define NOMINMAX
#undef byte  
#include <string>
#include <unordered_map>
#include <memory>
#include "Logger.h"

using namespace std;
#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE DllHandle;
    typedef FARPROC FunctionPtr;
#else
    #include <dlfcn.h>
    typedef void* DllHandle;
    typedef void* FunctionPtr;
#endif

struct DriverFunctions {
    FunctionPtr driverName;
    FunctionPtr driverInit;
    FunctionPtr driverCleanup;
    FunctionPtr driverVersion;
    FunctionPtr driverGetCapabilities;  
    FunctionPtr driverGetType;
    FunctionPtr driverGetStatus;
    FunctionPtr driverRead;
    FunctionPtr driverWrite;
    FunctionPtr driverConfigure;
};

class LoadedDriver {
public:
    std::string name;
    std::string filePath;  
    DllHandle handle;
    DriverFunctions functions;
    bool initialized;
    
    LoadedDriver(const std::string& n, const std::string& path, DllHandle h)
        : name(n), filePath(path), handle(h), initialized(false) {}
};

class DllLoader {
private:
    std::unordered_map<std::string, std::unique_ptr<LoadedDriver>> loadedDrivers;
    Logger& logger;
    
    DllHandle loadLibrary(const std::string& path);
    FunctionPtr getFunctionAddress(DllHandle handle, const std::string& functionName);
    void unloadLibrary(DllHandle handle);
    bool resolveFunctions(LoadedDriver& driver);
    bool validateDriver(const LoadedDriver& driver);

public:
    DllLoader(Logger& log);
    ~DllLoader();
    
    bool loadDriver(const std::string& dllPath);
    LoadedDriver* getDriver(const std::string& name);
    std::vector<std::string> getLoadedDriverNames() const;
    int getLoadedDriverCount() const;
    int loadAllDriversFromDirectory(const std::string& directory);
    void unloadAllDrivers();
    void displayLoadedDrivers() const;
};