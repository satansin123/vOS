#pragma once
#include<atomic>
#include <thread>
#include<chrono>

using namespace std;
class Scheduler;
class Clock{
    private:
        atomic<bool> running;
        atomic<bool> initialized;
        thread clockThread;

        void clockLoop();

    public:
        Clock();
        ~Clock();

        bool initialise();
        void stop();
        bool isRunning() const {
            return running;
        }

        void tick();

        static constexpr chrono::milliseconds TICK_INTERVALS{100};
};