#pragma once
#include "mutex"
#include "atomic"
#include<string>

using namespace std;
enum class MessageType{
    BOOT,
    INFO,
    ERRORS,
    SHUTDOWN,
    HEARTBEAT,
    PROMPT,
    STATUS,
    HEADER,
    USER_FEEDBACK,
    SCHEDULER,
    TIMER,
    UART,
    DLL_LOADER,
    INIT,
    VFS,
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