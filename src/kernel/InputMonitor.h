#pragma once
#include<atomic>
#include<thread>

using namespace std;

class InputMonitor{
    private:
        atomic<bool> shutdownRequested;
        atomic<bool> monitoring;
        thread inputThread;

        void inputLoop();
    public:
        InputMonitor();
        ~InputMonitor();

        void startMonitoring();
        void stopMonitoring();
        bool isShutdownRequested() const;
};