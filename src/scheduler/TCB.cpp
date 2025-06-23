#include "TCB.h"
#include "TaskTypes.h"
#include <exception>
#include <mutex>
#include <sys/stat.h>

using namespace std;

bool TCB::setState(TaskState newState){
    lock_guard<mutex> lock(tcbMutex);
    if(!isValidTransition(state, newState)){
        return false;
    }
    state = newState;
    if (newState == TaskState::WAITING) {
        resetWaitTimers();
    }
    return true;
}

bool TCB::isValidTransition(TaskState from, TaskState to){
    switch (from) {
        case TaskState::READY:
            return (to == TaskState::RUNNING);
        case TaskState::RUNNING:
            return ((to == TaskState::WAITING) || (to == TaskState::READY));
        case TaskState::WAITING:
            return (to == TaskState::READY);
        default:
            return false;
    }
}

bool TCB::executeTask(){
    if (state!= TaskState::RUNNING || !taskCallback) {
        return false;
    }
    try{
        taskCallback();
        return true;
    }
    catch(const exception ex){
        return false;
    }
}

string TCB::getStateString() const {
    switch (state){
        case TaskState::READY: return "READY";
        case TaskState::RUNNING: return "RUNNING";
        case TaskState::WAITING: return "WAITING";
        default: return "UNKNOWN";
    }
}

string TCB::getPriorityString() const {
    switch (priority) {
        case Priority::LOW: return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}


bool TCB::decrementWaitTimer(){
    lock_guard<mutex> lock(tcbMutex);
    if (state != TaskState::WAITING) {
        return false;
    }
    currentWaitTicks++;
    if (currentWaitTicks>=waitTicks) {
        state = TaskState::READY;
        currentWaitTicks = 0;
        return true;
    }
    return false;
}
void TCB::incrementActivation(){
            timerActivations++;
            auto now = chrono::steady_clock::now();

            if (lastActivationTime != chrono::steady_clock::time_point{}) {
                totalWaitTIme+=chrono::duration_cast<chrono::milliseconds>(now-lastActivationTime);
            }
            lastActivationTime = now;
        }