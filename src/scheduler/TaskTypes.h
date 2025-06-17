#pragma once
#include<string>

using namespace std;

enum class Priority{
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2
};

enum class TaskState{
    READY,
    RUNNING,
    WAITING
};