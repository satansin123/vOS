#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include<string>
#include "TaskTypes.h"
#include "TaskManager.h"
#include<mutex>
#include "../kernel/Kernel.h"
#include "../kernel/Logger.h"

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
    
    public:
        TCB(const string& name, Priority priority, function<void()> callback) : taskId(TaskManager::generateTaskId()) , 
                                                                                taskName(name), 
                                                                                priority(priority), 
                                                                                state(TaskState::READY),
                                                                                taskCallback(callback) {}
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
};