#include "Logger.h"
#include "Kernel.h"
#include<iostream>
#include <mutex>
#include <string>

using namespace std;
// stopping = true - its not initialised
Logger::Logger():initialized(false), stopping(false){}
Logger::~Logger(){
    stop();
}

bool Logger::initialize(){
    lock_guard<mutex> lock(logMutex);
    if (initialized) {
        return true;
    }
    stopping = false;
    initialized = true;
    return true;
}

void Logger::stop(){
    lock_guard<mutex> lock(logMutex);
    stopping = true;
    initialized = false;
}

bool Logger::isInitialized() {
    lock_guard<mutex> lock(logMutex);
    return initialized && !stopping;
}

void Logger::log(MessageType type, const string& message){
    lock_guard<mutex> lock(logMutex);
    if(!initialized || stopping){
        return;
    }
    string formattedMessage = formatMessage(type,message);
    cout<<formattedMessage<<endl;
    if (type == MessageType::PROMPT) {
        cout<<"> "<<flush;
    }
    else{
        cout.flush();
    }
}

string Logger::formatMessage(MessageType type, const string& message) const {
    switch (type) {
        case MessageType::BOOT: return "[BOOT] " + message;
        case MessageType::INFO: return "[INFO] " + message;
        case MessageType::ERRORS: return "[ERROR] " + message;
        case MessageType::SHUTDOWN: return "[SHUTDOWN] " + message;
        case MessageType::HEARTBEAT: return "[TICK " + to_string(Kernel::getTicks()) + "] " + message;
        case MessageType::STATUS: return  message;
        case MessageType::HEADER: return "\n=== " + message + " ===";
        case MessageType::USER_FEEDBACK: return message;
        case MessageType::PROMPT: return "";
        case MessageType::SCHEDULER: return "[SCHEDULER] "+message;
        case MessageType::TIMER: return "[TIMER] "+message;
        case MessageType::UART: return "[UART] "+message;
        case MessageType::DLL_LOADER: return "[DLL_LOADER] "+message;
        case MessageType::INIT: return "[INIT] "+message;
        default: return message;
    }
}