#pragma once
#include "mutex"
#include "atomic"
#include<string>

using namespace std;

class Logger{
    private:
        atomic<bool> initialized;
        mutex logMutex;
        atomic<bool> shuttingDown;
        
    public:
        Logger();
        ~Logger();

        bool initialize();
        void shutdown();

        void logHeartbeat();
        void log(const string& message);

        bool isInitialized();
};