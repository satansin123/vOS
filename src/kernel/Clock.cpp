#include "Clock.h"
#include "Kernel.h"
#include <chrono>
#include <cstdint>
#include <thread>
#include<iostream>

using namespace std;

Clock::Clock() : running(false), initialized(false){}

Clock::~Clock(){
    stop();
}

bool Clock::initialise(){
    if (initialized) {
        return true;
    }
    running = true;
    clockThread = thread(&Clock::clockLoop, this);
    initialized = true;
    return true;
}
void Clock::stop(){
    if (running) {
        running = false;
        if (clockThread.joinable()) {
            clockThread.join();
        }
    }
    initialized = false;
}
void Clock::tick(){
    Kernel::incrementTicks();

    uint64_t currentTick = Kernel::getTicks();
    uint64_t timeMs = currentTick*100;

    cout<<"Tick "<< currentTick << " at"<<timeMs<<"ms"<<endl;
}

void Clock::clockLoop(){
    auto nextTick = chrono::steady_clock::now() + TICK_INTERVALS;

    while (running) {
        this_thread::sleep_until(nextTick);
        if (running) {
            tick();
            nextTick+=TICK_INTERVALS;
        }
    }
}