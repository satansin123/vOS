#pragma once
#include <unordered_map>
#include "TCB.h"
#include "TaskTypes.h"
#include<string>
#include<memory>
#include<mutex>
#include <utility>
#include <vector>

using namespace std;

class Scheduler{
    private:
        unordered_map<string, unique_ptr<TCB>> registeredTasks;
        mutable mutex schedulerMutex;
        string lastExecutedTask;
        mutable int timerOverheadMicroseconds;
        static constexpr int MAX_TIMER_VALUE = 1000;
    public:
            bool isValidTask(const unique_ptr<TCB>& task) const;
            bool registerTask(unique_ptr<TCB> task);
            bool isTaskRegistered(const string& name) const;
            TCB* findTaskByName(const string& name) const;
            void getAllRegisteredTasks() const;
            int getNumberOfRegisteredTasks() const;
            bool unregisterTask(const string& name);

            
            void getRegistrationStats() const;
            bool hasTaskWithPriority(Priority priority) const;
            vector<string> getTaskNamesByState(TaskState state) const;
            int getTaskCountByState(TaskState state) const;
            void displayTaskSummary() const;
            int getTaskCountByPriority(Priority priority) const;

            vector<string> getReadyTasksInOrder() const;
            bool executeNextReadyTask();
            string getNextTaskToExecute(vector<string> taskReady) const;

            void updateTaskTimers();

            void displayTimerStatistics() const;
            pair<string, int> getMostActiveTask()const;
            float getAvgTimerAccuracy () const;
            int getTotalTimerActivations() const;

            bool adjustTaskTimer(const string& taskName, int newPeriod);
            bool pauseTaskTimer(const string& taskName);
            bool resumeTaskTimer(const string& taskName);
            vector<pair<string, pair<int, int>>> getTimerStatus() const;

            bool validateTimerValue(const unique_ptr<TCB>& task) const ;
            void displayDetailedTimerStatus() const;
            void displayTimerEfficiency() const;

            
};