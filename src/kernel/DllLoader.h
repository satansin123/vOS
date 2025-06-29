#pragma once
#define NOMINMAX
#undef byte  
#include <string>
#include <unordered_map>
#include <memory>
#include "Logger.h"
#include <chrono>
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
    string name;
    string filePath;  
    DllHandle handle;
    DriverFunctions functions;
    bool initialized;
    chrono::steady_clock::time_point loadTime;
    chrono::steady_clock::time_point initTime;
    
    LoadedDriver(const string& n, const string& path, DllHandle h)
        : name(n), filePath(path), handle(h), initialized(false) {
            loadTime = chrono::steady_clock::now();
        }
};

class DllLoader {
private:
    unordered_map<string, unique_ptr<LoadedDriver>> loadedDrivers;
    Logger& logger;

    static constexpr int INIT_TIMEOUT_MS = 2000;
    static constexpr int MAX_DRIVER_NAME_LENGTH = 32;
    
    DllHandle loadLibrary(const string& path);
    FunctionPtr getFunctionAddress(DllHandle handle, const string& functionName);
    void unloadLibrary(DllHandle handle);
    bool resolveFunctions(LoadedDriver& driver);
    bool validateDriver(const LoadedDriver& driver);

public:
    DllLoader(Logger& log);
    ~DllLoader();
    
    bool loadDriver(const string& dllPath);
    LoadedDriver* getDriver(const string& name);
    vector<string> getLoadedDriverNames() const;
    int getLoadedDriverCount() const;
    int loadAllDriversFromDirectory(const string& directory);
    void unloadAllDrivers();
    void displayLoadedDrivers() const;
};