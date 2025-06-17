#include "TCB.h"
#include "TaskTypes.h"
#include <exception>
#include <mutex>

using namespace std;

bool TCB::setState(TaskState newState){
    lock_guard<mutex> lock(tcbMutex);
    if(!isValidTransition(state, newState)){
        return false;
    }
    state = newState;
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
