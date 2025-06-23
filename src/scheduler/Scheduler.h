#pragma once
#include <unordered_map>
#include "TCB.h"
#include "TaskTypes.h"
#include<string>
#include<memory>
#include<mutex>
#include <vector>
using namespace std;

class Scheduler{
    private:
        unordered_map<string, unique_ptr<TCB>> registeredTasks;
        mutable mutex schedulerMutex;
        bool isValidTask(const unique_ptr<TCB>& task) const;
        string lastExecutedTask;
        vector<string> getReadyTasksInOrder() const;
        public:
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
            bool executeNextReadyTask();
            void runSchedulingCycle();
            string getNextTaskToExecute(vector<string> taskReady) const;

            void updateTaskTimers();
};