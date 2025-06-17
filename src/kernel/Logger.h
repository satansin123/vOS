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
    USER_FEEDBACK
};
class Logger{
    private:
        atomic<bool> initialized;
        mutex logMutex;
        atomic<bool> shuttingDown;

        string formatMessage(MessageType type, const string& message) const;
        
    public:
        Logger();
        ~Logger();

        bool initialize();
        void shutdown();
        void log(MessageType type,const string& message);

        bool isInitialized();

};