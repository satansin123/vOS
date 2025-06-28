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
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, "Task validation failed");
        return false;
    }
    string taskName = task->getName();
    if (registeredTasks.find(taskName) != registeredTasks.end()) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, "Task name is duplicate for :" + taskName);
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

        Kernel::getInstance().getLogger().log(MessageType::INFO, 
                                            "Task: " + name + 
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
    
    Kernel::getInstance().getLogger().log(MessageType::ERRORS, 
        "Task not found for unregistration: " + name);
    return false;
}

void Scheduler::getRegistrationStats() const {
    lock_guard<mutex> lock(schedulerMutex);
    
    int totalTasks = static_cast<int>(registeredTasks.size());
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

//round-robin, prioirty based execution logic

vector<string> Scheduler::getReadyTasksInOrder() const {
    vector<string> readyTasks;
    
    for (const auto& pair : registeredTasks) {
        if (pair.second->getState() == TaskState::READY) {
            readyTasks.push_back(pair.first);
        }
    }
    
    // sorting based on priority
    sort(readyTasks.begin(), readyTasks.end(), [this] (const string& a, const string& b){
        auto taskA = registeredTasks.at(a).get();
        auto taskB = registeredTasks.at(b).get();
        return static_cast<int>(taskA->getPriority()) > static_cast<int>(taskB->getPriority());
    }  );
    return readyTasks;
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
bool Scheduler::executeNextReadyTask(){
    lock_guard<mutex> lock(schedulerMutex);
    
    vector<string> readyTasks = getReadyTasksInOrder();
    if(readyTasks.empty()){
        Kernel::getInstance().getLogger().log(MessageType::SCHEDULER, "No READY tasks available for execution");
        return false;
    }
    
    string taskToExecute = getNextTaskToExecute(readyTasks);
    
    auto it = registeredTasks.find(taskToExecute);
    if(it == registeredTasks.end()){
        return false;
    }
    
    TCB* task = it->second.get();
    
    Kernel::getInstance().getLogger().log(MessageType::SCHEDULER, "Executing " + taskToExecute + " (READY -> RUNNING)");
    
    if(!task->setState(TaskState::RUNNING)){
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, "Failed to transition " + taskToExecute + " to RUNNING state");
        return false;
    }
    
    bool executionSuccess = task->executeTask();
    
    if(executionSuccess){
        Kernel::getInstance().getLogger().log(MessageType::SCHEDULER,  taskToExecute + " completed successfully");
    } else {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS,  taskToExecute + " execution failed");
    }
    
    if(!task->setState(TaskState::WAITING)){
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, "Failed to transition " + taskToExecute + " to WAITING state");
    } else {
        Kernel::getInstance().getLogger().log(MessageType::SCHEDULER,  taskToExecute + " complete (RUNNING -> WAITING)");
    }
    
    lastExecutedTask = taskToExecute;
    return executionSuccess;
}


void Scheduler::updateTaskTimers() {
    lock_guard<mutex> lock(schedulerMutex);
    
    for(auto& pair : registeredTasks) {
        if(pair.second->getState() == TaskState::WAITING) {
            if(pair.second->isCountDownLoggingEnabled()) {
                int remaining = pair.second->getWaitTicks() - pair.second->getCurrentWaitTicks();
                Kernel::getInstance().getLogger().log(MessageType::TIMER, 
                     pair.first + " countdown: " + to_string(remaining) + " ticks remaining");
            }
            
            if(pair.second->incrementCurrentWaitTimer()) {
                Kernel::getInstance().getLogger().log(MessageType::TIMER, 
                     pair.first + " timer expired (WAITING -> READY)");
            }
        }
    }
}

//statictics

void Scheduler::displayTimerStatistics() const{
    lock_guard<mutex> lock(schedulerMutex);
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Timer System Statistics");
    int totalActivations = 0;
    float totalAvgWait = 0.0f;
    for (const auto& pair : registeredTasks) {
        int activations = pair.second->getTimerActivations();
        float avgWait = pair.second->getAvgWaitTime();

        Kernel::getInstance().getLogger().log(MessageType::STATUS, pair.first+": "+
                                                to_string(activations) + " activations," +
                                            "avg wait: "+to_string(avgWait) + "ms");
        totalActivations+=activations;
        totalAvgWait+=avgWait;
    }
    Kernel::getInstance().getLogger().log(MessageType::STATUS, 
        "Total activations: " + to_string(totalActivations));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, 
        "System average wait: " + to_string(totalAvgWait / registeredTasks.size()) + "ms");
}

pair<string, int> Scheduler::getMostActiveTask() const{
    lock_guard<mutex> lock(schedulerMutex);

    string mostActive = "";
    int maxActivations = 0;
    for (const auto& pair : registeredTasks) {
        int activation = pair.second->getTimerActivations();
        if(activation>maxActivations){
            mostActive = pair.first;
            maxActivations = activation;
        }
    }
    return make_pair(mostActive, maxActivations);
}

bool Scheduler::adjustTaskTimer(const string& taskName, int newPeriod){
    lock_guard<mutex> lock(schedulerMutex);

    if (newPeriod<=0) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, "Invalid period " + to_string(newPeriod) + " for " + taskName);
        return false;
    }
    auto it = registeredTasks.find(taskName);
    if (it!=registeredTasks.end()) {
        it->second->setWaitTicks(newPeriod);
        Kernel::getInstance().getLogger().log(MessageType::TIMER, 
            taskName + " period adjusted to " + to_string(newPeriod) + " ticks");
        return true;
    }
    return false;
}

bool Scheduler::pauseTaskTimer(const string& taskName){
    lock_guard<mutex> lock(schedulerMutex);
    auto it = registeredTasks.find(taskName);
    if (it!= registeredTasks.end()) {
        it->second->pauseTimer();
        Kernel::getInstance().getLogger().log(MessageType::TIMER, 
             taskName + " timer paused");
        return true;
    }
    return false;
}

bool Scheduler::resumeTaskTimer(const string& taskName){
    lock_guard<mutex> lock(schedulerMutex);
    auto it = registeredTasks.find(taskName);
    if (it!=registeredTasks.end()) {
        it->second->resumeTimer();
        Kernel::getInstance().getLogger().log(MessageType::TIMER, 
             taskName + " timer resumed");
        return true;
    }
    return false;
}

vector<pair<string, pair<int, int>>> Scheduler::getTimerStatus() const{ //name, (current, total)
    lock_guard<mutex> lock(schedulerMutex);
    vector<pair<string, pair<int, int>>> status;
    for (const auto& pair: registeredTasks) {
        if (pair.second->getState() == TaskState::WAITING) {
            status.emplace_back(pair.first, make_pair(pair.second->getCurrentWaitTicks(),pair.second->getWaitTicks()));
        }
    }
    return status;
}

bool Scheduler::validateTimerValue(const unique_ptr<TCB>& task) const {
    if (task->getWaitTicks() < 0 || task->getWaitTicks()>MAX_TIMER_VALUE) {
        Kernel::getInstance().getLogger().log(MessageType::ERRORS, "Wait time is either too large or negative");
        return false;
    }
    return true;
}

void Scheduler::displayDetailedTimerStatus() const {
    lock_guard<mutex> lock(schedulerMutex);
    
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Detailed Timer Status");
    
    for(const auto& pair : registeredTasks) {
        const auto& task = pair.second;
        string status;
        
        if(task->getState() == TaskState::WAITING) {
            int remaining = task->getWaitTicks() - task->getCurrentWaitTicks();
            float progress = static_cast<float>(task->getCurrentWaitTicks()) / task->getWaitTicks() * 100.0f;
            
            status = task->getStateString() + " (" + to_string(remaining) + "/" + 
                    to_string(task->getWaitTicks()) + " ticks, " + 
                    to_string(static_cast<int>(progress)) + "% complete)";
        } else {
            status = task->getStateString();
        }
        
        Kernel::getInstance().getLogger().log(MessageType::STATUS, 
            pair.first + ": " + status + 
            " - Activations: " + to_string(task->getTimerActivations()) +
            (task->isTimerPaused() ? " [PAUSED]" : ""));
    }
    
    auto mostActive = getMostActiveTask();
    Kernel::getInstance().getLogger().log(MessageType::STATUS, 
        "Most active task: " + mostActive.first + " (" + to_string(mostActive.second) + " activations)");
}

void Scheduler::displayTimerEfficiency() const {
    lock_guard<mutex> lock(schedulerMutex);
    
    Kernel::getInstance().getLogger().log(MessageType::HEADER, "Timer System Efficiency");
    
    int totalTasks = static_cast<int>(registeredTasks.size());
    int waitingTasks = getTaskCountByState(TaskState::WAITING);
    int readyTasks = getTaskCountByState(TaskState::READY);
    
    float utilizationRate = totalTasks > 0 ? 
        static_cast<float>(readyTasks) / totalTasks * 100.0f : 0.0f;
    
    Kernel::getInstance().getLogger().log(MessageType::STATUS, 
        "Timer utilization: " + to_string(static_cast<int>(utilizationRate)) + "%");
    Kernel::getInstance().getLogger().log(MessageType::STATUS, 
        "Active timers: " + to_string(waitingTasks) + "/" + to_string(totalTasks));
    Kernel::getInstance().getLogger().log(MessageType::STATUS, 
        "Timer overhead: " + to_string(timerOverheadMicroseconds) + "μs per tick");
}
