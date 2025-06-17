#include "Logger.h"
#include "Kernel.h"
#include<iostream>
#include <mutex>
#include <string>

using namespace std;

Logger::Logger():initialized(false), shuttingDown(false){}
Logger::~Logger(){
    shutdown();
}

bool Logger::initialize(){
    lock_guard<mutex> lock(logMutex);
    if (initialized) {
        return true;
    }
    shuttingDown = false;
    initialized = true;
    return true;
}

void Logger::shutdown(){
    lock_guard<mutex> lock(logMutex);
    shuttingDown = true;
    initialized = false;
}

void Logger::log(MessageType type, const string& message){
    lock_guard<mutex> lock(logMutex);
    if(!initialized || shuttingDown){
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

bool Logger::isInitialized() {
    lock_guard<mutex> lock(logMutex);
    return initialized && !shuttingDown;
}

string Logger::formatMessage(MessageType type, const string& message) const {
    switch (type) {
        case MessageType::BOOT: return "[BOOT] " + message;
        case MessageType::INFO: return "[INFO] " + message;
        case MessageType::ERROR: return "[ERROR] " + message;
        case MessageType::SHUTDOWN: return "[SHUTDOWN] " + message;
        case MessageType::HEARTBEAT: return "[TICK " + to_string(Kernel::getTicks()) + "] " + message;
        case MessageType::STATUS: return  message;
        case MessageType::HEADER: return "\n=== " + message + " ===";
        case MessageType::USER_FEEDBACK: return message;
        case MessageType::PROMPT: return "";
        default: return message;
    }
}