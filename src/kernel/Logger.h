#pragma once
#include "mutex"
#include "atomic"
#include<string>

using namespace std;
enum class MessageType{
    BOOT,
    INFO,
    ERROR,
    SHUTDOWN,
    HEARTBEAT,
    PROMPT,
    STATUS,
    HEADER,
    USER_FEEDBACK,
    SCHEDULER,
    TIMER,
    UART,
};
class Logger{
    private:
        atomic<bool> initialized;
        atomic<bool> stopping;
        mutex logMutex;

        string formatMessage(MessageType type, const string& message) const;
        
    public:
        Logger();
        ~Logger();

        bool initialize();
        void stop();
        bool isInitialized();

        void log(MessageType type,const string& message);
};