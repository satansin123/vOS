#pragma once
#include <chrono>
#include <string>
#include <functional>
#include <mutex>
#include "TaskTypes.h"
#include "TaskManager.h"

using namespace std;

class TCB{
    private:
        //important tcb parameters
        uint32_t taskId;
        string taskName;
        Priority priority;
        TaskState state;
        function<void()> taskCallback;

        mutex tcbMutex;

        //waitTicks - the number of ticks this task has to wait to execute
        //currentWaitTicks - the number of ticks that have passed
        int waitTicks;
        int currentWaitTicks;

        bool enableCountDownLogging;
        int timerActivations;
        chrono::steady_clock::time_point lastActivationTime;
        chrono::milliseconds totalWaitTIme;
        bool timerPaused;
    
    public:
        TCB(const string& name, Priority priority, function<void()> callback, int waitPeriod = 0) : 
                                                                                taskId(TaskManager::generateTaskId()) , 
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
        bool isValidTransition(TaskState from, TaskState to);

        Priority getPriority() const {return priority;}
        void setPriority(Priority newPriority) {priority = newPriority;}

        bool hasCallback() const {return taskCallback != nullptr;}

        string getStateString() const ;
        string getPriorityString() const;

        bool executeTask() const;

        bool setWaitTicks(int ticks) {
            if (ticks<=0) {
                return false;
            }
            waitTicks = ticks;
            return true;
        }
        int getWaitTicks(){return waitTicks;}
        void resetWaitTimers(){currentWaitTicks=0;}
        bool incrementCurrentWaitTimer();
        int getCurrentWaitTicks() const {return currentWaitTicks;}

        bool isCountDownLoggingEnabled(){return enableCountDownLogging;}
        void enableTimerCountdown(bool enable){enableCountDownLogging = enable;}

        int getTimerActivations() const {return timerActivations;}
        chrono::milliseconds getTotalWaitTime() const {return totalWaitTIme;}

        float getAvgWaitTime() const {
            return timerActivations>0 ? static_cast<float>(totalWaitTIme.count()) / timerActivations : 0.0f;
        }
        void incrementActivation();

        bool isTimerPaused(){
            return timerPaused;
        }
        void pauseTimer(){
            timerPaused = true;
        }
        void resumeTimer(){
            timerPaused = false;
        }
        
};