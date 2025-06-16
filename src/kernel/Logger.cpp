#include "Logger.h"
#include "Kernel.h"
#include <cstdint>
#include<iostream>
#include <mutex>

using namespace std;

Logger::Logger():initialized(false), shuttingDown(false){}
Logger::~Logger(){
    shutdown();
}

bool Logger::initialize(){
    lock_guard<mutex> lock(logMutex);
    if (initialized) {
        return true;
    }
    shuttingDown = false;
    initialized = true;
    return true;
}

void Logger::shutdown(){
    lock_guard<mutex> lock(logMutex);
    shuttingDown = true;
    initialized = false;
}

void Logger::logHeartbeat(){
    lock_guard<mutex> lock(logMutex);
    if (!initialized || shuttingDown) {
        return;
    }
    uint64_t currentTick = Kernel::getTicks();
    cout<<"[Tick "<<currentTick<<"] System heartbeat"<<endl;
    cout.flush();
}

void Logger::log(const string& message){
    lock_guard<mutex> lock(logMutex);
    if(!initialized || shuttingDown){
        return;
    }
    cout<<message<<endl;
}

bool Logger::isInitialized() {
    lock_guard<mutex> lock(logMutex);
    return initialized && !shuttingDown;
}