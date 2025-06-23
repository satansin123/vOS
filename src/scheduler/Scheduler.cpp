#include "Scheduler.h"
#include <iterator>
#include<unordered_map>
#include<string>
#include<memory>
#include<mutex>
#include "TCB.h"
#include "../kernel/Kernel.h"
#include "../kernel/Logger.h"
#include <utility>
#include <vector>
#include "TCB.h"
#include"TaskTypes.h"
#include <algorithm>

using namespace std;

bool Scheduler::isValidTask(const unique_ptr<TCB>& task) const{

    if (!task) {
        return false;
    }
    if(task->getName().empty()){
        return false;
    }
    if (!task->hasCallback()) {
        return false;
    }
    return true;
}

bool Scheduler::registerTask(unique_ptr<TCB> task){
    lock_guard<mutex> lock(schedulerMutex);
    if(!isValidTask(task)){
        Kernel::getInstance().getLogger().log(MessageType::ERROR, "Task validation failed");
        return false;
    }
    string taskName = task->getName();
    if (registeredTasks.find(taskName) != registeredTasks.end()) {
        Kernel::getInstance().getLogger().log(MessageType::ERROR, "Task name is duplicate for :" + taskName);
        return false;
    }

    registeredTasks[taskName] = std::move(task);
    Kernel::getInstance().getLogger().log(MessageType::INFO, "Task registered successfully: " + taskName);
    return true;
}
bool Scheduler::isTaskRegistered(const string& name) const {
    lock_guard<mutex> lock(schedulerMutex);
    return registeredTasks.find(name) != registeredTasks.end();
}

TCB* Scheduler::findTaskByName(const string& name) const{
    lock_guard<mutex> lock(schedulerMutex);
    auto it = registeredTasks.find(name);
    if(it != registeredTasks.end() ){
        return it->second.get();
    }
    return nullptr;
}

void Scheduler::getAllRegisteredTasks() const{
    lock_guard<mutex> lock(schedulerMutex);
    
    if (registeredTasks.empty()) {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "no task registered");
        return;
    }
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Registered Tasks");

    for(const auto& it : registeredTasks){
        const string& name = it.first;
        const unique_ptr<TCB>& task = it.second;

        Kernel::getInstance().getLogger().log(MessageType::INFO, "Task: " + name + 
                                                                                " (ID: " + to_string(task->getId()) + 
                                                                                ", Priority: " + task->getPriorityString() + 
                                                                                ", State: " + task->getStateString() + ")");
        
    }
}

int Scheduler::getNumberOfRegisteredTasks() const{
    lock_guard<mutex> lock(schedulerMutex);
    return static_cast<int>(registeredTasks.size());
}

bool Scheduler::unregisterTask(const string& name){
    lock_guard<mutex> lock(schedulerMutex);
    auto it = registeredTasks.find(name);
    if (it != registeredTasks.end()) {
        registeredTasks.erase(it);
        Kernel::getInstance().getLogger().log(MessageType::INFO, 
            "Task unregistered: " + name);
        return true;
    }
    
    Kernel::getInstance().getLogger().log(MessageType::ERROR, 
        "Task not found for unregistration: " + name);
    return false;
}

void Scheduler::getRegistrationStats() const {
    lock_guard<mutex> lock(schedulerMutex);
    
    int totalTasks = registeredTasks.size();
    int readyTasks = getTaskCountByState(TaskState::READY);
    int runningTasks = getTaskCountByState(TaskState::RUNNING);
    int waitingTasks = getTaskCountByState(TaskState::WAITING);
    
    int lowPriority = getTaskCountByPriority(Priority::LOW);
    int mediumPriority = getTaskCountByPriority(Priority::MEDIUM);
    int highPriority = getTaskCountByPriority(Priority::HIGH);
    
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Task Registration Statistics");
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "Total Tasks: " + to_string(totalTasks));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "State Distribution:");
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "  - READY: " + to_string(readyTasks));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "  - RUNNING: " + to_string(runningTasks));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "  - WAITING: " + to_string(waitingTasks));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "Priority Distribution:");
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "  - LOW: " + to_string(lowPriority));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "  - MEDIUM: " + to_string(mediumPriority));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, "  - HIGH: " + to_string(highPriority));
}

int Scheduler::getTaskCountByPriority(Priority priority) const {
    int count = 0;
    for (const auto& pair : registeredTasks) {
        if (pair.second->getPriority() == priority) {
            count++;
        }
    }
    return count;
}

bool Scheduler::hasTaskWithPriority(Priority priority) const {
    lock_guard<mutex> lock(schedulerMutex);
    for (const auto& pair : registeredTasks) {
        if (pair.second->getPriority() == priority) {
            return true;
        }
    }
    return false;
}

vector<string> Scheduler::getTaskNamesByState(TaskState state) const {
    lock_guard<mutex> lock(schedulerMutex);
    vector<string> taskNames;
    
    for (const auto& pair : registeredTasks) {
        if (pair.second->getState() == state) {
            taskNames.push_back(pair.first);
        }
    }
    return taskNames;
}

int Scheduler::getTaskCountByState(TaskState state) const {
    int count = 0;
    for (const auto& pair : registeredTasks) {
        if (pair.second->getState() == state) {
            count++;
        }
    }
    return count;
}

void Scheduler::displayTaskSummary() const {
    lock_guard<mutex> lock(schedulerMutex);
    
    if (registeredTasks.empty()) {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "No tasks to summarize");
        return;
    }
    
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Task Summary");
    
    // HIGH Priority Tasks
    Kernel::getInstance().getLogger().log(MessageType::INFO, "HIGH Priority Tasks:");
    bool foundHigh = false;
    for (const auto& pair : registeredTasks) {
        if (pair.second->getPriority() == Priority::HIGH) {
            Kernel::getInstance().getLogger().log(MessageType::INFO, 
                "  - " + pair.first + " (" + pair.second->getStateString() + ")");
            foundHigh = true;
        }
    }
    if (!foundHigh) {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "  (none)");
    }
    
    // MEDIUM Priority Tasks
    Kernel::getInstance().getLogger().log(MessageType::INFO, "MEDIUM Priority Tasks:");
    bool foundMedium = false;
    for (const auto& pair : registeredTasks) {
        if (pair.second->getPriority() == Priority::MEDIUM) {
            Kernel::getInstance().getLogger().log(MessageType::INFO, 
                "  - " + pair.first + " (" + pair.second->getStateString() + ")");
            foundMedium = true;
        }
    }
    if (!foundMedium) {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "  (none)");
    }
    
    // LOW Priority Tasks
    Kernel::getInstance().getLogger().log(MessageType::INFO, "LOW Priority Tasks:");
    bool foundLow = false;
    for (const auto& pair : registeredTasks) {
        if (pair.second->getPriority() == Priority::LOW) {
            Kernel::getInstance().getLogger().log(MessageType::INFO, 
                "  - " + pair.first + " (" + pair.second->getStateString() + ")");
            foundLow = true;
        }
    }
    if (!foundLow) {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "  (none)");
    }
}

bool Scheduler::executeNextReadyTask(){
    lock_guard<mutex> lock(schedulerMutex);
    
    vector<string> readyTasks = getReadyTasksInOrder();
    if(readyTasks.empty()){
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SCHEDULER] No READY tasks available for execution");
        return false;
    }
    
    string taskToExecute = getNextTaskToExecute(readyTasks);
    
    // Find task WITHOUT calling findTaskByName (avoid deadlock)
    auto it = registeredTasks.find(taskToExecute);
    if(it == registeredTasks.end()){
        return false;
    }
    
    TCB* task = it->second.get();
    
    Kernel::getInstance().getLogger().log(MessageType::INFO, "[SCHEDULER] Executing " + taskToExecute + " (READY -> RUNNING)");
    
    if(!task->setState(TaskState::RUNNING)){
        Kernel::getInstance().getLogger().log(MessageType::ERROR, "[SCHEDULER] Failed to transition " + taskToExecute + " to RUNNING state");
        return false;
    }
    
    bool executionSuccess = task->executeTask();
    
    if(executionSuccess){
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SCHEDULER] " + taskToExecute + " completed successfully");
    } else {
        Kernel::getInstance().getLogger().log(MessageType::ERROR, "[SCHEDULER] " + taskToExecute + " execution failed");
    }
    
    if(!task->setState(TaskState::WAITING)){
        Kernel::getInstance().getLogger().log(MessageType::ERROR, "[SCHEDULER] Failed to transition " + taskToExecute + " to WAITING state");
    } else {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SCHEDULER] " + taskToExecute + " complete (RUNNING -> WAITING)");
    }
    
    lastExecutedTask = taskToExecute;
    return executionSuccess;
}


void Scheduler::runSchedulingCycle(){
    lock_guard<mutex> lock(schedulerMutex);
    vector<string> readyTasks = getTaskNamesByState(TaskState::READY);

    if (readyTasks.empty()) {
        Kernel::getInstance().getLogger().log(MessageType::INFO, "[SCHEDULER] No READY tasks available for execution");
    }
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Starting Scheduling Cycle - "+ to_string(readyTasks.size()) + " READY tasks");
    lock.~lock_guard();
    int taskExecuted = 0;
    for (const string& _ : readyTasks) {
        if (executeNextReadyTask()) {
            taskExecuted++;
        }
    } 
    Kernel::getInstance().getLogger().log(MessageType::INFO, 
        "[SCHEDULER] Scheduling cycle complete - " + to_string(taskExecuted) + 
        " tasks executed");   
}

string Scheduler::getNextTaskToExecute(vector<string> taskReady) const {
    if (taskReady.empty()) {
        return "";
    }
    if (lastExecutedTask.empty()) {
        return taskReady[0];
    }
    auto it = find(taskReady.begin(), taskReady.end(), lastExecutedTask);
    if (it == taskReady.end()) {
        return taskReady[0];
    }
    auto nextTask = next(it);
    if (nextTask == taskReady.end()) {
        return taskReady[0];
    }
    return *nextTask;
}

void Scheduler::updateTaskTimers() {
    lock_guard<mutex> lock(schedulerMutex);
    
    for(auto& pair : registeredTasks) {
        if(pair.second->getState() == TaskState::WAITING) {
            
            // Show countdown progress if enabled
            if(pair.second->isCountDownLoggingEnabled()) {
                int remaining = pair.second->getWaitTicks() - pair.second->getCurrentWaitTicks();
                Kernel::getInstance().getLogger().log(MessageType::INFO, 
                    "[TIMER] " + pair.first + " countdown: " + to_string(remaining) + " ticks remaining");
            }
            
            if(pair.second->decrementWaitTimer()) {
                Kernel::getInstance().getLogger().log(MessageType::INFO, 
                    "[TIMER] " + pair.first + " timer expired (WAITING → READY)");
            }
        }
    }
}

vector<string> Scheduler::getReadyTasksInOrder() const {
    vector<string> readyTasks;
    
    // Get all ready tasks
    for (const auto& pair : registeredTasks) {
        if (pair.second->getState() == TaskState::READY) {
            readyTasks.push_back(pair.first);
        }
    }
    
    // Sort for consistent ordering
    sort(readyTasks.begin(), readyTasks.end());
    
    return readyTasks;
}