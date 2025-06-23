#pragma once
#include <string>
#include <functional>
#include <mutex>
#include "TaskTypes.h"
#include "TaskManager.h"

using namespace std;

class TCB{
    private:
        uint32_t taskId;
        string taskName;
        Priority priority;
        TaskState state;
        function<void()> taskCallback;
        bool isValidTransition(TaskState from, TaskState to);
        mutex tcbMutex;

        int waitTicks;
        int currentWaitTicks;

        bool enableCountDownLogging;
    
    public:
        TCB(const string& name, Priority priority, function<void()> callback, int waitPeriod = 0) : taskId(TaskManager::generateTaskId()) , 
                                                                                taskName(name), 
                                                                                priority(priority), 
                                                                                state(TaskState::READY),
                                                                                taskCallback(callback),
                                                                                waitTicks(waitPeriod),
                                                                                currentWaitTicks(0) {}

        uint32_t getId() const {return taskId;}
        const string& getName() const {return taskName;}

        bool setState(TaskState newState);
        TaskState getState() const {return state;}

        Priority getPriority() const {return priority;}
        void setPriority(Priority newPriority) {priority = newPriority;}

        bool hasCallback() const {return taskCallback != nullptr;}

        string getStateString() const ;
        string getPriorityString() const;

        bool executeTask();

        void setWaitTicks(int ticks) {waitTicks = ticks;}
        int getWaitTicks(){return waitTicks;}
        void resetWaitTimers(){currentWaitTicks=0;}
        bool decrementWaitTimer();
        int getCurrentWaitTicks() const {return currentWaitTicks;}

        bool isCountDownLoggingEnabled(){return enableCountDownLogging;}
        void enableTimerCountdown(bool enable){enableCountDownLogging = enable;}
};