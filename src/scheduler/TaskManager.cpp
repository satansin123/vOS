#include "TaskManager.h"
#include<atomic>
#include <cstdint>
using namespace std;

atomic<uint32_t> TaskManager::nextTaskId{1};