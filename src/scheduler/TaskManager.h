#pragma once
#include <cstdint>
#include <string>
#include<atomic>
using namespace std;

class TaskManager{
    private:
        static atomic<uint32_t> nextTaskId;
    public:
        static uint32_t generateTaskId(){
            return nextTaskId.fetch_add(1);
        }

};